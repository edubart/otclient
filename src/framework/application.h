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

    virtual void init(const std::vector<std::string>& args);
    virtual void registerLuaFunctions();
    virtual void terminate();
    virtual void run();
    virtual void exit();
    virtual void poll();
    virtual void close();

    void setFrameSleep(int delay) { m_frameSleep = delay; }

    bool isRunning() { return m_running; }
    bool isStopping() { return m_stopping; }
    int getFrameSleep() { return m_frameSleep; }
    const std::string& getName() { return m_appName; }
    const std::string& getVersion() { return m_appVersion; }

    std::string getBuildCompiler() { return BUILD_COMPILER; }
    std::string getBuildDate() { return BUILD_DATE; }
    std::string getBuildRevision() { return BUILD_REVISION; }
    std::string getBuildType() { return BUILD_TYPE; }

protected:
    virtual void render();
    virtual void resize(const Size& size);
    virtual void inputEvent(const InputEvent& event);

    std::string m_appName;
    std::string m_appVersion;
    std::string m_appBuildDate;
    int m_frameSleep;
    Boolean<false> m_initialized;
    Boolean<false> m_running;
    Boolean<false> m_stopping;
};

extern Application *g_app;

#endif

