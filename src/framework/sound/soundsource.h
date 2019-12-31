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

#ifndef SOUNDSOURCE_H
#define SOUNDSOURCE_H

#include "declarations.h"
#include "soundbuffer.h"
#include <framework/luaengine/luaobject.h>

class SoundSource : public LuaObject
{
protected:
    SoundSource(uint sourceId) : m_sourceId(sourceId) { }

public:
    enum FadeState { NoFading, FadingOn, FadingOff };

    SoundSource();
    virtual ~SoundSource();

    virtual void play();
    virtual void stop();

    virtual bool isBuffering();
    virtual bool isPlaying() { return isBuffering(); }

    void setName(const std::string& name) { m_name = name; }
    virtual void setLooping(bool looping);
    virtual void setRelative(bool relative);
    virtual void setReferenceDistance(float distance);
    virtual void setGain(float gain);
    virtual void setPitch(float pitch);
    virtual void setPosition(const Point& pos);
    virtual void setVelocity(const Point& velocity);
    virtual void setFading(FadeState state, float fadetime);

    std::string getName() { return m_name; }
    uchar getChannel() { return m_channel; }
    float getGain() { return m_gain; }

protected:
    void setBuffer(const SoundBufferPtr& buffer);
    void setChannel(uchar channel) { m_channel = channel; }

    virtual void update();
    friend class SoundManager;
    friend class CombinedSoundSource;

    uint m_sourceId;
    uchar m_channel;
    std::string m_name;
    SoundBufferPtr m_buffer;
    FadeState m_fadeState;
    float m_fadeStartTime;
    float m_fadeTime;
    float m_fadeGain;
    float m_gain;
};

#endif
