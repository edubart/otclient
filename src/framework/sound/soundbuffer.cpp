/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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

#include "soundbuffer.h"
#include "soundfile.h"

SoundBuffer::SoundBuffer()
{
    m_bufferId = 0;
    alGenBuffers(1, &m_bufferId);
    assert(alGetError() == AL_NO_ERROR);
}

SoundBuffer::~SoundBuffer()
{
    alDeleteBuffers(1, &m_bufferId);
    assert(alGetError() == AL_NO_ERROR);
}

bool SoundBuffer::fillBuffer(const SoundFilePtr& soundFile)
{
    ALenum format = soundFile->getSampleFormat();
    if(format == AL_UNDETERMINED) {
        g_logger.error(stdext::format("unable to determine sample format for '%s'", soundFile->getName()));
        return false;
    }

    DataBuffer<char> samples(soundFile->getSize());
    int read = soundFile->read(&samples[0], soundFile->getSize());
    if(read == 0) {
        g_logger.error(stdext::format("unable to fill audio buffer data for '%s'", soundFile->getName()));
        return false;
    }

    return fillBuffer(format, samples, samples.size(), soundFile->getRate());
}

bool SoundBuffer::fillBuffer(ALenum sampleFormat, const DataBuffer<char>& data, int size, int rate)
{
    alBufferData(m_bufferId, sampleFormat, &data[0], size, rate);
    ALenum err = alGetError();
    if(err != AL_NO_ERROR) {
        g_logger.error(stdext::format("unable to fill audio buffer data: %s", alGetString(err)));
        return false;
    }
    return true;
}
