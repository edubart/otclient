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

#ifndef LOGGER_H
#define LOGGER_H

#include "../global.h"

struct LogMessage {
    LogMessage(Fw::LogLevel level, const std::string& message, std::size_t when) : level(level), message(message), when(when) { }
    Fw::LogLevel level;
    std::string message;
    std::size_t when;
};

class Logger
{
    typedef std::function<void(Fw::LogLevel, std::string, std::size_t)> OnLogCallback;

public:
    void log(Fw::LogLevel level, const std::string& message);
    void logFunc(Fw::LogLevel level, const std::string& message, std::string prettyFunction);

    void fireOldMessages();
    void setLogFile(const std::string& file);
    void setOnLog(const OnLogCallback& onLog) { m_onLog = onLog; }

private:
    std::list<LogMessage> m_logMessages;
    OnLogCallback m_onLog;
    std::ofstream m_outFile;
};

extern Logger g_logger;

// specialized logging
#define logDebug(...) g_logger.log(Fw::LogDebug, Fw::mkstr(__VA_ARGS__))
#define logInfo(...) g_logger.log(Fw::LogInfo, Fw::mkstr(__VA_ARGS__))
#define logWarning(...) g_logger.log(Fw::LogWarning, Fw::mkstr(__VA_ARGS__))
#define logError(...) g_logger.log(Fw::LogError, Fw::mkstr(__VA_ARGS__))
#define logFatal(...) g_logger.log(Fw::LogFatal, Fw::mkstr(__VA_ARGS__))

#define logTrace() g_logger.logFunc(Fw::LogDebug, "", __PRETTY_FUNCTION__)
#define logTraceDebug(...) g_logger.logFunc(Fw::LogDebug, Fw::mkstr(__VA_ARGS__), __PRETTY_FUNCTION__)
#define logTraceInfo(...) g_logger.logFunc(Fw::LogInfo, Fw::mkstr(__VA_ARGS__), __PRETTY_FUNCTION__)
#define logTraceWarning(...) g_logger.logFunc(Fw::LogWarning, Fw::mkstr(__VA_ARGS__), __PRETTY_FUNCTION__)
#define logTraceError(...) g_logger.logFunc(Fw::LogError, Fw::mkstr(__VA_ARGS__), __PRETTY_FUNCTION__)

#endif
