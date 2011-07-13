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
void log(LogLevel level, const std::string& message, const char *trace = NULL);

// specialized logging
template<class... T>
void debug(const T&... args) { log(LogInfo, make_string(args...)); }
template<class... T>
void info(const T&... args) { log(LogDebug, make_string(args...)); }
template<class... T>
void warning(const T&... args) { log(LogWarning, make_string(args...)); }
template<class... T>
void error(const T&... args) { log(LogError, make_string(args...)); }
template<class... T>
void fatal(const T&... args) { log(LogFatal, make_string(args...)); }

#define trace() debug(LogDebug, "", __PRETTY_FUNCTION__)

// dump utility
struct Dump {
    ~Dump() { debug(s.str().c_str()); }
    template<class T>
    Dump& operator<<(const T& v) { s << v << " "; return *this; }
    std::ostringstream s;
};
#define dump Dump()

#endif // LOGGER_H
