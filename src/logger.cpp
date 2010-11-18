#include "logger.h"
#include "util.h"

#include <iostream>
#include <sstream>
#include <cstdarg>
#include <cstdlib>
#include <boost/algorithm/string.hpp>

void _log(int level, const char *trace, const char *format, ...)
{
    va_list args;
    std::stringstream out;
    std::string strace;

    va_start(args, format);
    std::string text = vformat(format, args);
    va_end(args);

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
        out << colors[level];
#endif

    if(!strace.empty())
        out << "[" << strace << "] ";

    static char const *prefixes[] = { "FATAL ERROR: ", "ERROR: ", "WARNING: ", "", "", "" };
    out << prefixes[level];
    out << text;

#ifdef linux
    if(colored)
        out << "\033[0m";
#endif

    if(level <= LWARNING)
        std::cerr << out.str() << std::endl;
    else
        std::cout << out.str() << std::endl;

    if(level == LFATAL)
        exit(-1);
}
