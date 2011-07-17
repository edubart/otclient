#ifndef LOGGER_H
#define LOGGER_H

#include "makestring.h"

enum LogLevel {
    LogDebug = 0,
    LogInfo,
    LogWarning,
    LogError,
    LogFatal
};
void log(LogLevel level, const std::string& message, std::string prettyFunction = "");

// specialized logging
#define logDebug(...) log(LogDebug, make_string(__VA_ARGS__))
#define logInfo(...) log(LogInfo, make_string(__VA_ARGS__))
#define logWarning(...) log(LogWarning, make_string(__VA_ARGS__))
#define logError(...) log(LogError, make_string(__VA_ARGS__))
#define logFatal(...) log(LogFatal, make_string(__VA_ARGS__))

#define trace() log(LogDebug, "", __PRETTY_FUNCTION__)
#define traceDebug(...) log(LogDebug, make_string(__VA_ARGS__), __PRETTY_FUNCTION__)
#define traceInfo(...) log(LogInfo, make_string(__VA_ARGS__), __PRETTY_FUNCTION__)
#define traceWarning(...) log(LogWarning, make_string(__VA_ARGS__), __PRETTY_FUNCTION__)
#define traceError(...) log(LogError, make_string(__VA_ARGS__), __PRETTY_FUNCTION__)

// dump utility
struct Dump {
    ~Dump() { logDebug(s.str().c_str()); }
    template<class T>
    Dump& operator<<(const T& v) { s << v << " "; return *this; }
    std::ostringstream s;
};
#define dump Dump()

#endif // LOGGER_H
