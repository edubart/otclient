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

class GraphicalApplication : public Application
{
    enum {
        POLL_CYCLE_DELAY = 10
    };

public:
    void init(std::vector<std::string>& args);
    void deinit();
    void terminate();
    void run();
    void poll();
    void close();

    bool willRepaint() { return m_mustRepaint; }
    void repaint() { m_mustRepaint = true; }

    void setForegroundPaneMaxFps(int maxFps) { m_foregroundFrameCounter.setMaxFps(maxFps); }
    void setBackgroundPaneMaxFps(int maxFps) { m_backgroundFrameCounter.setMaxFps(maxFps); }

    int getForegroundPaneFps() { return m_foregroundFrameCounter.getLastFps(); }
    int getBackgroundPaneFps() { return m_backgroundFrameCounter.getLastFps(); }
    int getForegroundPaneMaxFps() { return m_foregroundFrameCounter.getMaxFps(); }
    int getBackgroundPaneMaxFps() { return m_backgroundFrameCounter.getMaxFps(); }

    bool isOnInputEvent() { return m_onInputEvent; }

protected:
    void resize(const Size& size);
    void inputEvent(const InputEvent& event);

private:
    stdext::boolean<false> m_onInputEvent;
    stdext::boolean<false> m_mustRepaint;
    AdaptativeFrameCounter m_backgroundFrameCounter;
    AdaptativeFrameCounter m_foregroundFrameCounter;
    TexturePtr m_foreground;
};

extern GraphicalApplication g_app;

#endif
