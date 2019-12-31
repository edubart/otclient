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

#ifndef COMBINEDSOUNDSOURCE_H
#define COMBINEDSOUNDSOURCE_H

#include "soundsource.h"

class CombinedSoundSource : public SoundSource
{
public:
    CombinedSoundSource();

    void addSource(const SoundSourcePtr& source);
    std::vector<SoundSourcePtr> getSources() { return m_sources; }

    void play();
    void stop();

    bool isBuffering();
    bool isPlaying();

    void setLooping(bool looping);
    void setRelative(bool relative);
    void setReferenceDistance(float distance);
    void setGain(float gain);
    void setPitch(float pitch);
    void setPosition(const Point& pos);
    void setVelocity(const Point& velocity);
    void setFading(FadeState state, float fadetime);

protected:
    virtual void update();

private:
    std::vector<SoundSourcePtr> m_sources;
};

#endif
