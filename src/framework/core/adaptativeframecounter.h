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

#include <framework/global.h>

 /**
  * Class that help counting and limiting frames per second in a application,
  */
class AdaptativeFrameCounter
{
public:
    AdaptativeFrameCounter() : m_interval(stdext::millis()) {}

    bool update();
    bool canRefresh();

    uint getFps() const { return m_fps; }
    uint getMaxFps() const { return m_maxFps; }

    void setMaxFps(const uint max) { m_maxFps = max; }

private:
    double getMaxPeriod() { return 1.0 / m_maxFps; }

    uint m_fps{ 0 },
        m_maxFps{ 0 },
        m_fpsCount{ 0 },
        m_interval{ 0 };

    ticks_t m_currentTime{ 0 }, m_lastTime{ 0 };
};
