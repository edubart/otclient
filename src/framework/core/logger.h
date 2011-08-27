#ifndef LOGGER_H
#define LOGGER_H

#include "../util/tools.h"
#include "../const.h"
#include <vector>
#include <functional>

struct LogMessage {
  LogMessage(LogLevel level, const std::string& message, std::size_t when) : level(level), message(message), when(when) { }
  LogLevel level;
  std::string message;
  std::size_t when;
};

class Logger
{
    typedef std::function<void(LogLevel, std::string, std::size_t)> OnLogCallback;

public:
    Logger();

    void log(LogLevel level, std::string message);
    void logFunc(LogLevel level, const std::string& message, std::string prettyFunction);

    void fireOldMessages();
    void setOnLog(const OnLogCallback& onLog) { m_onLog = onLog; }

private:
    std::vector<LogMessage> m_logMessages;
    OnLogCallback m_onLog;
    bool m_terminated;
};

extern Logger g_logger;

// specialized logging
#define logDebug(...) g_logger.log(LogDebug, fw::mkstr(__VA_ARGS__))
#define logInfo(...) g_logger.log(LogInfo, fw::mkstr(__VA_ARGS__))
#define logWarning(...) g_logger.log(LogWarning, fw::mkstr(__VA_ARGS__))
#define logError(...) g_logger.log(LogError, fw::mkstr(__VA_ARGS__))
#define logFatal(...) g_logger.log(LogFatal, fw::mkstr(__VA_ARGS__))

#define logTrace() g_logger.logFunc(LogDebug, "", __PRETTY_FUNCTION__)
#define logTraceDebug(...) g_logger.logFunc(LogDebug, fw::mkstr(__VA_ARGS__), __PRETTY_FUNCTION__)
#define logTraceInfo(...) g_logger.logFunc(LogInfo, fw::mkstr(__VA_ARGS__), __PRETTY_FUNCTION__)
#define logTraceWarning(...) g_logger.logFunc(LogWarning, fw::mkstr(__VA_ARGS__), __PRETTY_FUNCTION__)
#define logTraceError(...) g_logger.logFunc(LogError, fw::mkstr(__VA_ARGS__), __PRETTY_FUNCTION__)

#endif