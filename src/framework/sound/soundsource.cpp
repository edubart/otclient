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

#include "soundsource.h"
#include "soundbuffer.h"

#include <framework/core/clock.h>

SoundSource::SoundSource()
{
    m_sourceId = 0;
    m_channel = 0;
    m_fadeState = NoFading;
    m_fadeTime = 0;
    m_fadeStartTime = 0;
    m_fadeGain = 0;
    m_gain = 1.0f;

    alGenSources(1, &m_sourceId);
    assert(alGetError() == AL_NO_ERROR);
    setReferenceDistance(128);
}

SoundSource::~SoundSource()
{
    if(m_sourceId != 0) {
        stop();
        alDeleteSources(1, &m_sourceId);
        assert(alGetError() == AL_NO_ERROR);
    }
}

void SoundSource::play()
{
    alSourcePlay(m_sourceId);
    assert(alGetError() == AL_NO_ERROR);
}

void SoundSource::stop()
{
    alSourceStop(m_sourceId);
    assert(alGetError() == AL_NO_ERROR);
    if(m_buffer) {
        alSourcei(m_sourceId, AL_BUFFER, AL_NONE);
        assert(alGetError() == AL_NO_ERROR);
        m_buffer = nullptr;
    }
}

bool SoundSource::isBuffering()
{
    int state = AL_PLAYING;
    alGetSourcei(m_sourceId, AL_SOURCE_STATE, &state);
    return state != AL_STOPPED;
}

void SoundSource::setBuffer(const SoundBufferPtr& buffer)
{
    alSourcei(m_sourceId, AL_BUFFER, buffer->getBufferId());
    assert(alGetError() == AL_NO_ERROR);
    m_buffer = buffer;
}

void SoundSource::setLooping(bool looping)
{
    alSourcei(m_sourceId, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
}

void SoundSource::setRelative(bool relative)
{
    alSourcei(m_sourceId, AL_SOURCE_RELATIVE, relative ? AL_TRUE : AL_FALSE);
}

void SoundSource::setReferenceDistance(float distance)
{
    alSourcef(m_sourceId, AL_REFERENCE_DISTANCE, distance);
}

void SoundSource::setGain(float gain)
{
    alSourcef(m_sourceId, AL_GAIN, gain);
    m_gain = gain;
}

void SoundSource::setPitch(float pitch)
{
    alSourcef(m_sourceId, AL_PITCH, pitch);
}

void SoundSource::setPosition(const Point& pos)
{
    alSource3f(m_sourceId, AL_POSITION, pos.x, pos.y, 0);
}

void SoundSource::setVelocity(const Point& velocity)
{
    alSource3f(m_sourceId, AL_VELOCITY, velocity.x, velocity.y, 0);
}

void SoundSource::setFading(FadeState state, float fadeTime)
{
    float now = stdext::millis() / 1000.0f;
    if(m_fadeState != NoFading) {
        float elapsed = now - m_fadeStartTime;
        float add;
        if(m_fadeState == FadingOn)
            add = -(1-(elapsed / m_fadeTime))*fadeTime;
        else
            add = -(elapsed / m_fadeTime)*fadeTime;
        m_fadeStartTime = now + add;
    } else
        m_fadeStartTime = now;

    m_fadeState = state;
    m_fadeTime = fadeTime;
    m_fadeGain = m_gain;

    if(m_fadeState == FadingOn)
        setGain(0.0);
}

void SoundSource::update()
{
    float now = stdext::millis() / 1000.0f;
    if(m_fadeState == FadingOn) {
        float elapsed = now - m_fadeStartTime;
        if(elapsed >= m_fadeTime) {
            m_fadeState = NoFading;
        } else {
            setGain((elapsed / m_fadeTime)  * m_fadeGain);
        }
    } else if(m_fadeState == FadingOff) {
        float elapsed = now - m_fadeStartTime;
        if(elapsed >= m_fadeTime) {
            setGain(m_fadeGain);
            stop();
            m_fadeState = NoFading;
        } else {
            setGain(((m_fadeTime - elapsed) / m_fadeTime) * m_fadeGain);
        }
    }
}
