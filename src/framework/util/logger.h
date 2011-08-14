#ifndef LOGGER_H
#define LOGGER_H

#include "auxiliary.h"

class Logger
{
public:
    enum LogLevel {
        LogDebug = 0,
        LogInfo,
        LogWarning,
        LogError,
        LogFatal
    };
    static void log(LogLevel level, const std::string& message, std::string prettyFunction = "");
};

// specialized logging
#define logDebug(...) Logger::log(Logger::LogDebug, aux::make_string(__VA_ARGS__))
#define logInfo(...) Logger::log(Logger::LogInfo, aux::make_string(__VA_ARGS__))
#define logWarning(...) Logger::log(Logger::LogWarning, aux::make_string(__VA_ARGS__))
#define logError(...) Logger::log(Logger::LogError, aux::make_string(__VA_ARGS__))
#define logFatal(...) Logger::log(Logger::LogFatal, aux::make_string(__VA_ARGS__))

#define logTrace() Logger::log(Logger::LogDebug, "", __PRETTY_FUNCTION__)
#define logTraceDebug(...) Logger::log(Logger::LogDebug, aux::make_string(__VA_ARGS__), __PRETTY_FUNCTION__)
#define logTraceInfo(...) Logger::log(Logger::LogInfo, aux::make_string(__VA_ARGS__), __PRETTY_FUNCTION__)
#define logTraceWarning(...) log(Logger::LogWarning, aux::make_string(__VA_ARGS__), __PRETTY_FUNCTION__)
#define logTraceError(...) Logger::log(Logger::LogError, aux::make_string(__VA_ARGS__), __PRETTY_FUNCTION__)

#endif
