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

#include "../global.h"

#include <fstream>
#include <mutex>

struct LogMessage
{
    LogMessage(Fw::LogLevel level, std::string_view message, std::size_t when) : level(level), message(message), when(when) {}
    Fw::LogLevel level;
    std::string message;
    std::size_t when;
};

// @bindsingleton g_logger
class Logger
{
    enum
    {
        MAX_LOG_HISTORY = 1000
    };

    using OnLogCallback = std::function<void(Fw::LogLevel, const std::string_view, int64_t)>;

public:
    void log(Fw::LogLevel level, const std::string_view message);
    void logFunc(Fw::LogLevel level, const std::string_view message, const std::string_view prettyFunction);

    void debug(const std::string_view what) { log(Fw::LogDebug, what); }
    void info(const std::string_view what) { log(Fw::LogInfo, what); }
    void warning(const std::string_view what) { log(Fw::LogWarning, what); }
    void error(const std::string_view what) { log(Fw::LogError, what); }
    void fatal(const std::string_view what) { log(Fw::LogFatal, what); }

    void fireOldMessages();
    void setLogFile(const std::string_view file);
    void setOnLog(const OnLogCallback& onLog) { m_onLog = onLog; }

private:
    std::deque<LogMessage> m_logMessages;
    OnLogCallback m_onLog;
    std::ofstream m_outFile;
    std::recursive_mutex m_mutex;
};

extern Logger g_logger;

#define trace() logFunc(Fw::LogDebug, "", __PRETTY_FUNCTION__)
#define traceDebug(a) logFunc(Fw::LogDebug, a, __PRETTY_FUNCTION__)
#define traceInfo(a) logFunc(Fw::LogInfo, a, __PRETTY_FUNCTION__)
#define traceWarning(a) logFunc(Fw::LogWarning, a, __PRETTY_FUNCTION__)
#define traceError(a) logFunc(Fw::LogError, a, __PRETTY_FUNCTION__)

#define logTraceCounter() { \
    static int __count = 0; \
    static Timer __timer; \
    __count++; \
    if(__timer.ticksElapsed() >= 1000) { \
        logTraceDebug(__count); \
        __count = 0; \
        __timer.restart(); \
    } \
}

#define logTraceFrameCounter() { \
    static int __count = 0; \
    static Timer __timer; \
    __count++; \
    if(__timer.ticksElapsed() > 0) { \
        logTraceDebug(__count); \
        __count = 0; \
        __timer.restart(); \
    } \
}
