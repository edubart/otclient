#include "logger.h"

#include <iostream>
#include <cstdlib>

void log(LogLevel level, const std::string& message, const char *trace)
{
    std::string strace;
    if(trace) {
        strace = trace;
        strace = strace.substr(0, strace.find_first_of('('));
        if(strace.find_last_of(' ') != std::string::npos)
            strace = strace.substr(strace.find_last_of(' ') + 1);
    }

#ifdef linux
    static char const *colors[] = { "\033[01;31m ", "\033[01;31m", "\033[01;33m", "\033[0;32m", "\033[01;34m" };
    static bool colored = getenv("COLORED_OUTPUT");
    if(colored)
        std::cout << colors[level];
#endif

    if(!strace.empty())
        std::cout << "[" << strace << "] ";

    std::cout << message;

#ifdef linux
    if(colored)
        std::cout << "\033[0m";
#endif

    std::cout << std::endl;

    if(level == LogFatal)
        exit(-1);
}
