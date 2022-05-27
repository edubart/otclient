/*
 * Copyright (c) 2010-2022 OTClient <https://github.com/edubart/otclient>
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

#pragma once

#include "soundsource.h"

class CombinedSoundSource : public SoundSource
{
public:
    CombinedSoundSource();

    void addSource(const SoundSourcePtr& source);
    std::vector<SoundSourcePtr> getSources() { return m_sources; }

    void play() override;
    void stop() override;

    bool isBuffering() override;
    bool isPlaying() override;

    void setLooping(bool looping) override;
    void setRelative(bool relative) override;
    void setReferenceDistance(float distance) override;
    void setGain(float gain) override;
    void setPitch(float pitch) override;
    void setPosition(const Point& pos) override;
    void setVelocity(const Point& velocity) override;
    void setFading(FadeState state, float fadetime) override;

protected:
    void update() override;

private:
    std::vector<SoundSourcePtr> m_sources;
};
