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

#ifndef GRAPHICALAPPLICATION_H
#define GRAPHICALAPPLICATION_H

#include "application.h"
#include <framework/graphics/declarations.h>
#include <framework/core/inputevent.h>
#include <framework/core/timer.h>

class GraphicalApplication : public Application
{
    enum {
        POLL_CYCLE_DELAY = 10
    };

public:
    void init(std::vector<std::string>& args) override;
    void deinit() override;
    void terminate() override;
    void run() override;
    void poll() override;
    void close() override;

    void repaint() { m_mustRepaint = true; }

    void setMaxFps(int maxFps) { m_frameCounter.setMaxFps(maxFps); }

    int getFps() { return m_frameCounter.getFps(); }
    int getMaxFps() { return m_frameCounter.getMaxFps(); }

    bool isOnInputEvent() { return m_onInputEvent; }
    bool canOptimize() { return m_optimize && getFps() < 60; }

    void optimize(const bool optimize) { m_optimize = optimize; }

protected:
    void resize(const Size& size);
    void inputEvent(const InputEvent& event);

private:
    bool foregroundCanUpdate() { return m_mustRepaint && m_refreshTime.ticksElapsed() >= 16; }

    bool m_onInputEvent{ false },
        m_mustRepaint{ false },
        m_optimize{ false };

    Timer m_refreshTime;

    AdaptativeFrameCounter m_frameCounter;

    PoolFramedPtr m_foregroundFramed;
};

extern GraphicalApplication g_app;

#endif
