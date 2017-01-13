/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "streamsoundsource.h"
#include "soundbuffer.h"
#include "soundfile.h"

#include <framework/util/databuffer.h>
#include <boost/concept_check.hpp>

StreamSoundSource::StreamSoundSource()
{
    for(auto& buffer : m_buffers)
        buffer = SoundBufferPtr(new SoundBuffer);
    m_downMix = NoDownMix;
}

StreamSoundSource::~StreamSoundSource()
{
    stop();
}

void StreamSoundSource::setSoundFile(const SoundFilePtr& soundFile)
{
    m_soundFile = soundFile;
    if(m_waitingFile) {
        m_waitingFile = false;
        play();
    }
}

void StreamSoundSource::play()
{
    m_playing = true;

    if(!m_soundFile) {
        m_waitingFile = true;
        return;
    }

    if(m_eof) {
        m_soundFile->reset();
        m_eof = false;
    }

    queueBuffers();

    SoundSource::play();
}

void StreamSoundSource::stop()
{
    m_playing = false;

    if(m_waitingFile)
        return;

    SoundSource::stop();
    unqueueBuffers();
}

void StreamSoundSource::queueBuffers()
{
    int queued;
    alGetSourcei(m_sourceId, AL_BUFFERS_QUEUED, &queued);
    for(int i = 0; i < STREAM_FRAGMENTS - queued; ++i) {
        if(!fillBufferAndQueue(m_buffers[i]->getBufferId()))
            break;
    }
}

void StreamSoundSource::unqueueBuffers()
{
    int queued;
    alGetSourcei(m_sourceId, AL_BUFFERS_QUEUED, &queued);
    for(int i = 0; i < queued; ++i) {
        uint buffer;
        alSourceUnqueueBuffers(m_sourceId, 1, &buffer);
    }
}

void StreamSoundSource::update()
{
    if(m_waitingFile)
        return;

    SoundSource::update();

    int processed = 0;
    alGetSourcei(m_sourceId, AL_BUFFERS_PROCESSED, &processed);
    for(int i = 0; i < processed; ++i) {
        uint buffer;
        alSourceUnqueueBuffers(m_sourceId, 1, &buffer);
        //SoundManager::check_al_error("Couldn't unqueue audio buffer: ");

        if(!fillBufferAndQueue(buffer))
            break;
    }

    if(!isBuffering() && m_playing) {
        if(!m_looping && m_eof) {
            stop();
        } else if(processed == 0) {
            g_logger.traceError("audio buffer underrun");
            play();
        } else if(m_looping) {
            play();
        }
    }
}

bool StreamSoundSource::fillBufferAndQueue(uint buffer)
{
    if(m_waitingFile)
        return false;

    // fill buffer
    static DataBuffer<char> bufferData(2*STREAM_FRAGMENT_SIZE);
    ALenum format = m_soundFile->getSampleFormat();

    int maxRead = STREAM_FRAGMENT_SIZE;
    if(m_downMix != NoDownMix)
        maxRead *= 2;

    int bytesRead = 0;
    do {
        bytesRead += m_soundFile->read(&bufferData[bytesRead], maxRead - bytesRead);

        // end of sound file
        if(bytesRead < maxRead) {
            if(m_looping)
                m_soundFile->reset();
            else {
                m_eof = true;
                break;
            }
        }
    } while(bytesRead < maxRead);

    if(bytesRead > 0) {
        if(m_downMix != NoDownMix) {
            if(format == AL_FORMAT_STEREO16) {
                assert(bytesRead % 2 == 0);
                bytesRead /= 2;
                uint16_t *data = (uint16_t*)bufferData.data();
                for(int i=0;i<bytesRead/2;i++)
                    data[i] = data[2*i + (m_downMix == DownMixLeft ? 0 : 1)];
                format = AL_FORMAT_MONO16;
            }
        }

        alBufferData(buffer, format, &bufferData[0], bytesRead, m_soundFile->getRate());
        ALenum err = alGetError();
        if(err != AL_NO_ERROR)
            g_logger.error(stdext::format("unable to refill audio buffer for '%s': %s", m_soundFile->getName(), alGetString(err)));

        alSourceQueueBuffers(m_sourceId, 1, &buffer);
        err = alGetError();
        if(err != AL_NO_ERROR)
            g_logger.error(stdext::format("unable to queue audio buffer for '%s': %s", m_soundFile->getName(), alGetString(err)));
    }

    // return false if there aren't more buffers to fill
    return (bytesRead >= STREAM_FRAGMENT_SIZE && !m_eof);
}

void StreamSoundSource::downMix(StreamSoundSource::DownMix downMix)
{
    m_downMix = downMix;
}
