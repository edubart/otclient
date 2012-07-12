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

#include "logger.h"
#include "eventdispatcher.h"
#include <framework/platform/platformwindow.h>

Logger g_logger;

void Logger::log(Fw::LogLevel level, const std::string& message)
{
#ifdef NDEBUG
    if(level == Fw::LogDebug)
        return;
#endif

    static bool ignoreLogs = false;
    if(ignoreLogs)
        return;

    const static std::string logPrefixes[] = { "", "", "WARNING: ", "ERROR: ", "FATAL ERROR: " };

    std::string outmsg = logPrefixes[level] + message;
    std::cout << outmsg << std::endl;

    if(m_outFile.good()) {
        m_outFile << outmsg << std::endl;
        m_outFile.flush();
    }

    std::size_t now = std::time(NULL);
    m_logMessages.push_back(LogMessage(level, outmsg, now));
    if(m_logMessages.size() > MAX_LOG_HISTORY)
        m_logMessages.pop_front();

    if(m_onLog) {
        // schedule log callback, because this callback can run lua code that may affect the current state
        g_dispatcher.addEvent([=] {
            if(m_onLog)
                m_onLog(level, outmsg, now);
        });
    }

    if(level == Fw::LogFatal) {
#ifdef OTCLIENT
        g_window.displayFatalError(message);
#endif
        ignoreLogs = true;
        exit(-1);
    }
}

void Logger::logFunc(Fw::LogLevel level, const std::string& message, std::string prettyFunction)
{
    std::stringstream ss;
    prettyFunction = prettyFunction.substr(0, prettyFunction.find_first_of('('));
    if(prettyFunction.find_last_of(' ') != std::string::npos)
        prettyFunction = prettyFunction.substr(prettyFunction.find_last_of(' ') + 1);

    if(!prettyFunction.empty())
        ss << "[" << prettyFunction << "] ";

    ss << message;
    log(level, ss.str());
}

void Logger::fireOldMessages()
{
    if(m_onLog) {
        auto backup = m_logMessages;
        for(const LogMessage& logMessage : backup) {
            m_onLog(logMessage.level, logMessage.message, logMessage.when);
        }
    }
}

void Logger::setLogFile(const std::string& file)
{
    m_outFile.open(file.c_str(), std::ios::out | std::ios::app);
    if(!m_outFile.is_open() || !m_outFile.good()) {
        g_logger.error(stdext::format("Unable to save log to '%s'", file));
        return;
    }

    m_outFile << "\n== application started at " << stdext::date_time_string() << std::endl;
    m_outFile.flush();
}
