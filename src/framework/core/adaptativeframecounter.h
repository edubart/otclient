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

#ifndef ADAPTATIVEFRAMECOUNTER_H
#define ADAPTATIVEFRAMECOUNTER_H

#include <framework/global.h>

/**
 * Class that help counting and limiting frames per second in a application,
 * to make fps limit work as desired, this class requires 2 platforms prerequisites:
 *  - the platform must have timer with a precision of at least 1ms
 *  - the platform sleep must have a precision of at least 4ms
 */
class AdaptativeFrameCounter
{
public:
    enum {
        // 4ms because most platforms has kernel timer of 250Hz
        MINIMUM_MICROS_SLEEP = 4000
    };

    AdaptativeFrameCounter();

    bool shouldProcessNextFrame();
    void processNextFrame();
    bool update();
    void setMaxFps(int maxFps);
    bool isFpsLimitActive() { return m_maxFps != 0; }

    int getMaximumSleepMicros();
    float getFrameDelayHit();
    int getLastFps() { return m_lastFps; }
    int getPartialFps() { return (int)m_partialFps; }
    int getMaxFps() { return m_maxFps; }
    int getFrames() { return m_frames; }
    float getMediumFrameDelay() { return m_mediumFrameDelay; }

private:
    int m_frames;
    int m_partialFrames;
    float m_partialFps;
    float m_maxPartialFps;
    ticks_t m_frameDelaySum;
    ticks_t m_mediumFrameDelay;
    ticks_t m_lastFrame;
    int m_lastFps;
    int m_maxFps;
    ticks_t m_bestFrameDelay;
    ticks_t m_lastFpsUpdate;
    ticks_t m_lastPartialFpsUpdate;
    float m_sleepMicros;
};

#endif
