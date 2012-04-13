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

#ifndef STREAMSOUNDSOURCE_H
#define STREAMSOUNDSOURCE_H

#include "soundsource.h"

class StreamSoundSource : public SoundSource
{
    enum {
        STREAM_BUFFER_SIZE = 1024 * 500,
        STREAM_FRAGMENTS = 5,
        STREAM_FRAGMENT_SIZE = STREAM_BUFFER_SIZE / STREAM_FRAGMENTS
    };

public:
    enum FadeState { NoFading, FadingOn, FadingOff };

    StreamSoundSource();
    virtual ~StreamSoundSource();

    void setSoundFile(const SoundFilePtr& soundFile);

    void setFading(FadeState state, float fadetime);
    FadeState getFadeState() {  return m_fadeState; }

    void update();

private:
    bool fillBufferAndQueue(ALuint buffer);

    SoundFilePtr m_soundFile;
    std::array<SoundBufferPtr,STREAM_FRAGMENTS> m_buffers;
    FadeState m_fadeState;
    float m_fadeStartTime;
    float m_fadeTime;
    Boolean<false> m_looping;
};

#endif
