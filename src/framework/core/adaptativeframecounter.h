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

#ifndef ADAPTATIVEFRAMECOUNTER_H
#define ADAPTATIVEFRAMECOUNTER_H

#include <framework/global.h>

 /**
  * Class that help counting and limiting frames per second in a application,
  */
class AdaptativeFrameCounter
{
public:
    inline AdaptativeFrameCounter() : m_interval(GetTickCount()) {}

    bool update();
    bool canRefresh();

    size_t getMaximumSleepMicros() { return static_cast<long>(getMaxPeriod()) * 1000.0; }

    uint getFps() const { return m_fps; }
    uint getMaxFps() const { return m_maxFps; }

    void setMaxFps(const uint max) { m_maxFps = max; }

private:
    double getMaxPeriod() { return 1.0 / m_maxFps; }
    double getTime()
    {
        return std::chrono::duration_cast<std::chrono::duration<double>>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
            ).count();
    }

    uint m_fps{ 0 },
        m_maxFps{ 0 },
        m_fpsCount{ 0 },
        m_interval{ 0 };

    double m_currentTime{ 0.f }, m_lastTime{ 0.f };
};

#endif
