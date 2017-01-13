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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <framework/global.h>
#include <framework/core/adaptativeframecounter.h>

//@bindsingleton g_app
class Application
{
public:
    Application();
    virtual ~Application() {}

    virtual void init(std::vector<std::string>& args);
    virtual void deinit();
    virtual void terminate();
    virtual void run() = 0;
    virtual void poll();
    virtual void exit();
    virtual void close();

    void setName(const std::string& name) { m_appName = name; }
    void setCompactName(const std::string& compactName) { m_appCompactName = compactName; }
    void setVersion(const std::string& version) { m_appVersion = version; }

    bool isRunning() { return m_running; }
    bool isStopping() { return m_stopping; }
    bool isTerminated() { return m_terminated; }
    const std::string& getName() { return m_appName; }
    const std::string& getCompactName() { return m_appCompactName; }
    const std::string& getVersion() { return m_appVersion; }

    std::string getCharset() { return m_charset; }
    std::string getBuildCompiler() { return BUILD_COMPILER; }
    std::string getBuildDate() { return std::string(__DATE__); }
    std::string getBuildRevision() { return BUILD_REVISION; }
    std::string getBuildCommit() { return BUILD_COMMIT; }
    std::string getBuildType() { return BUILD_TYPE; }
    std::string getBuildArch() { return BUILD_ARCH; }
    std::string getOs();
    std::string getStartupOptions() { return m_startupOptions; }

protected:
    void registerLuaFunctions();

    std::string m_charset;
    std::string m_appName;
    std::string m_appCompactName;
    std::string m_appVersion;
    std::string m_startupOptions;
    stdext::boolean<false> m_running;
    stdext::boolean<false> m_stopping;
    stdext::boolean<false> m_terminated;
};

#ifdef FW_GRAPHICS
#include "graphicalapplication.h"
#else
#include "consoleapplication.h"
#endif

#endif
