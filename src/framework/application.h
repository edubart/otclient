/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <framework/core/inputevent.h>

class Application
{
    enum {
        POLL_CYCLE_DELAY = 10
    };

public:
    Application(const std::string& appName);
    ~Application();

    virtual void init(const std::vector<std::string>& args, int appFlags);
    virtual void registerLuaFunctions();
    virtual void terminate();
    virtual void run();
    virtual void exit();
    virtual void poll();
    virtual void close() { exit(); }

    void setPollCycleDelay(int delay) { m_pollCycleDelay = delay; }

    bool isRunning() { return m_running; }
    bool isStopping() { return m_stopping; }
    std::string getAppName() { return m_appName; }

protected:
    virtual void render();
    virtual void resize(const Size& size);
    virtual void inputEvent(const InputEvent& event);

private:
    std::string m_appName;
    int m_appFlags;
    int m_pollCycleDelay;
    Boolean<false> m_running;
    Boolean<false> m_stopping;
};

extern Application *g_app;

#endif

