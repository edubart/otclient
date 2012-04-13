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

#ifndef CLOCK_H
#define CLOCK_H

#include "declarations.h"

class Clock
{
public:
    Clock();

    ticks_t updateTicks();
    void sleep(int ms);

    ticks_t asyncTicks();
    ticks_t ticks() { return m_currentTicks; }
    ticks_t ticksElapsed(long prevTicks) { return m_currentTicks - prevTicks; }
    ticks_t ticksFor(int delay) { return m_currentTicks + delay; }

    float asyncTime() { return asyncTicks()/1000.0f; }
    float time() { return m_currentTime; }
    float timeElapsed(float prevTime) { return m_currentTime - prevTime; }
    float timeFor(float delay) { return m_currentTime + delay; }

private:
    ticks_t m_currentTicks;
    float m_currentTime;
    std::chrono::system_clock::time_point m_startupTime;
};

extern Clock g_clock;

#endif

