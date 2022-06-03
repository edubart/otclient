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

#include "logger.h"
#include "eventdispatcher.h"

#include <framework/core/asyncdispatcher.h>
#include <framework/core/resourcemanager.h>

#ifdef FW_GRAPHICS
#include <framework/luaengine/luainterface.h>
#include <framework/platform/platform.h>
#include <framework/platform/platformwindow.h>
#endif

Logger g_logger;

namespace
{
    constexpr std::string_view s_logPrefixes[] = { "", "", "WARNING: ", "ERROR: ", "FATAL ERROR: " };
#if ENABLE_ENCRYPTION == 1
    bool s_ignoreLogs = true;
#else
    bool s_ignoreLogs = false;
#endif
}

void Logger::log(Fw::LogLevel level, const std::string_view message)
{
    std::lock_guard lock(m_mutex);

#ifdef NDEBUG
    if (level == Fw::LogDebug)
        return;
#endif

    if (s_ignoreLogs)
        return;

    std::string outmsg{ std::string{s_logPrefixes[level]} + message.data() };

    std::cout << outmsg << std::endl;

    if (m_outFile.good()) {
        m_outFile << outmsg << std::endl;
        m_outFile.flush();
    }

    std::size_t now = std::time(nullptr);
    m_logMessages.emplace_back(level, outmsg, now);
    if (m_logMessages.size() > MAX_LOG_HISTORY)
        m_logMessages.pop_front();

    if (m_onLog) {
        // schedule log callback, because this callback can run lua code that may affect the current state
        g_dispatcher.addEvent([this, level, outmsg, now] {
            if (m_onLog)
                m_onLog(level, outmsg, now);
        });
    }

    if (level == Fw::LogFatal) {
    #ifdef FW_GRAPHICS
        g_window.displayFatalError(message);
    #endif
        s_ignoreLogs = true;

        // NOTE: Threads must finish before the process can exit.
        g_asyncDispatcher.terminate();

        exit(-1);
    }
}

void Logger::logFunc(Fw::LogLevel level, const std::string_view message, const std::string_view prettyFunction)
{
    std::lock_guard lock(m_mutex);

    auto fncName = prettyFunction.substr(0, prettyFunction.find_first_of('('));
    if (fncName.find_last_of(' ') != std::string::npos)
        fncName = fncName.substr(fncName.find_last_of(' ') + 1);

    std::stringstream ss;
    ss << message;

    if (!fncName.empty()) {
        if (g_lua.isInCppCallback())
            ss << g_lua.traceback("", 1);
        ss << g_platform.traceback(fncName, 1, 8);
    }

    log(level, ss.str());
}

void Logger::fireOldMessages()
{
    std::lock_guard lock(m_mutex);

    if (m_onLog) {
        for (const LogMessage& logMessage : m_logMessages) {
            m_onLog(logMessage.level, logMessage.message, logMessage.when);
        }
    }
}

void Logger::setLogFile(const std::string_view file)
{
    std::lock_guard lock(m_mutex);

    m_outFile.open(stdext::utf8_to_latin1(file), std::ios::out | std::ios::app);
    if (!m_outFile.is_open() || !m_outFile.good()) {
        g_logger.error(stdext::format("Unable to save log to '%s'", file));
        return;
    }
    m_outFile.flush();
}
