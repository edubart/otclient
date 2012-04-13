/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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
#include <framework/core/clock.h>

StreamSoundSource::StreamSoundSource()
{
    for(auto& buffer : m_buffers)
        buffer = SoundBufferPtr(new SoundBuffer);
    m_fadeState = NoFading;
}

StreamSoundSource::~StreamSoundSource()
{
    stop();

    ALint queued;
    alGetSourcei(m_sourceId, AL_BUFFERS_QUEUED, &queued);
    for(int i = 0; i < queued; ++i) {
        ALuint buffer;
        alSourceUnqueueBuffers(m_sourceId, 1, &buffer);
    }
    m_buffers.fill(nullptr);
}

void StreamSoundSource::setSoundFile(const SoundFilePtr& soundFile)
{
    m_soundFile = soundFile;

    ALint queued;
    alGetSourcei(m_sourceId, AL_BUFFERS_QUEUED, &queued);
    for(int i = 0; i < STREAM_FRAGMENTS - queued; ++i) {
        if(!fillBufferAndQueue(m_buffers[i]->getBufferId()))
            break;
    }
}

void StreamSoundSource::update()
{
    ALint processed = 0;
    alGetSourcei(m_sourceId, AL_BUFFERS_PROCESSED, &processed);
    for(ALint i = 0; i < processed; ++i) {
        ALuint buffer;
        alSourceUnqueueBuffers(m_sourceId, 1, &buffer);
        //SoundManager::check_al_error("Couldn't unqueue audio buffer: ");

        if(!fillBufferAndQueue(buffer))
            break;
    }

    if(!isPlaying()) {
        if(processed == 0 || !m_looping)
            return;

        // we might have to restart the source if we had a buffer underrun
        //log_info << "Restarting audio source because of buffer underrun" << std::endl;
        //play();
    }

    float realTime = g_clock.asyncTime();
    if(m_fadeState == FadingOn) {
        float time = realTime - m_fadeStartTime;
        if(time >= m_fadeTime) {
            setGain(1.0);
            m_fadeState = NoFading;
        } else {
            setGain(time / m_fadeTime);
        }
    } else if(m_fadeState == FadingOff) {
        float time = realTime - m_fadeStartTime;
        if(time >= m_fadeTime) {
            stop();
            m_fadeState = NoFading;
        } else {
            setGain((m_fadeTime - time) / m_fadeTime);
        }
    }
}

bool StreamSoundSource::fillBufferAndQueue(ALuint buffer)
{
    // fill buffer
    DataBuffer<char> bufferData(STREAM_FRAGMENT_SIZE);

    int bytesRead = 0;
    do {
        bytesRead += m_soundFile->read(&bufferData[bytesRead], STREAM_FRAGMENT_SIZE - bytesRead);

        // end of sound file
        if(bytesRead < STREAM_FRAGMENT_SIZE) {
            if(m_looping)
                m_soundFile->reset();
            else
                break;
        }
    } while(bytesRead < STREAM_FRAGMENT_SIZE);

    if(bytesRead > 0) {
        ALenum format = m_soundFile->getSampleFormat();
        alBufferData(buffer, format, &bufferData[0], bytesRead, m_soundFile->getRate());
        ALenum err = alGetError();
        if(err != AL_NO_ERROR)
            logError("unable to refill audio buffer for '", m_soundFile->getName(), "': ", alGetString(err));

        alSourceQueueBuffers(m_sourceId, 1, &buffer);
        err = alGetError();
        if(err != AL_NO_ERROR)
            logError("unable to queue audio buffer for '", m_soundFile->getName(), "': ", alGetString(err));
    }

    // return false if there aren't more buffers to fill
    return bytesRead >= STREAM_FRAGMENT_SIZE;
}

void StreamSoundSource::setFading(FadeState state, float fadeTime)
{
    m_fadeState = state;
    m_fadeTime = fadeTime;
    m_fadeStartTime = g_clock.asyncTime();
}
