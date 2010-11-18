#ifndef LOGGER_H
#define LOGGER_H

#include <string>

enum ELogLevel {
    LFATAL = 0,
    LERROR,
    LWARNING,
    LNOTICE,
    LDEBUG
};

void _log(int level, const char *trace, const char *format, ...);

#define fatal(...) _log(LFATAL, NULL, ## __VA_ARGS__)
#define error(...) _log(LERROR, NULL, ## __VA_ARGS__)
#define warning(...) _log(LWARNING, NULL, ## __VA_ARGS__)
#define debug(...) _log(LDEBUG, NULL, ## __VA_ARGS__)
#define notice(...) _log(LNOTICE, NULL, ## __VA_ARGS__)

#endif
