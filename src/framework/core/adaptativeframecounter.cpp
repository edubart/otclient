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

#include "adaptativeframecounter.h"
#include "clock.h"

AdaptativeFrameCounter::AdaptativeFrameCounter()
{
    m_frames = 0;
    m_partialFrames = 0;
    m_partialFps = 0;
    m_maxPartialFps = 0;
    m_frameDelaySum = 0;
    m_mediumFrameDelay = 0;
    m_lastFps = 0;
    m_lastFrame = 0;
    m_maxFps = 0;
    m_sleepMicros = 0;
    m_mediumFrameDelay = 0;
    m_lastFpsUpdate = g_clock.micros();
    m_lastPartialFpsUpdate = g_clock.micros();
}

bool AdaptativeFrameCounter::shouldProcessNextFrame()
{
    if(m_maxFps == 0)
        return true;

    ticks_t now = g_clock.micros();
    if(now - m_lastFrame < m_bestFrameDelay)
        return false;
    return true;
}

void AdaptativeFrameCounter::processNextFrame()
{
    ticks_t now = g_clock.micros();
    m_frames++;
    m_partialFrames++;
    m_frameDelaySum += now - m_lastFrame;
    m_lastFrame = now ;
}

bool AdaptativeFrameCounter::update()
{
    ticks_t now = g_clock.micros();
    ticks_t delta = now - m_lastPartialFpsUpdate;
    if(delta > 41000 && m_partialFrames > 0) {
        m_partialFps = m_partialFrames / (delta / 1000000.0f);
        m_lastPartialFpsUpdate = now;
        m_partialFrames = 0;
    }

    delta =  now - m_lastFpsUpdate;
    if(delta >= 1000000) {
        m_lastFps = m_frames;
        if(m_frames > 0)
            m_mediumFrameDelay = m_frameDelaySum / m_frames;
        else
            m_mediumFrameDelay = 0;
        m_lastFpsUpdate = now;
        m_frames = 0;
        m_frameDelaySum = 0;

        //dump << stdext::format("FPS => %d  Partial FPS => %d  Frame Delay Hit => %.2f%%", m_lastFps, (int)m_partialFps, getFrameDelayHit());
        return true;
    }
    return false;
}

void AdaptativeFrameCounter::setMaxFps(int maxFps)
{
    maxFps = stdext::clamp<int>(maxFps, 0, 1000);

    if(maxFps != 0) {
        m_bestFrameDelay = 1000000 / maxFps;
    } else {
        m_maxPartialFps = 0;
        m_bestFrameDelay = 0;
    }
    m_maxFps = maxFps;
}

int AdaptativeFrameCounter::getMaximumSleepMicros()
{
    if(m_maxFps == 0)
        return 0;
    return m_lastFrame + m_bestFrameDelay - g_clock.micros();
}

float AdaptativeFrameCounter::getFrameDelayHit()
{
    if(m_bestFrameDelay > 0)
        return ((m_bestFrameDelay - std::abs(m_bestFrameDelay - m_mediumFrameDelay)) * 100.0f) / (float)m_bestFrameDelay;
    else
        return 100.0f;
}
