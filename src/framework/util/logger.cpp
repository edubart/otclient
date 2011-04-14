/* The MIT License
 *
 * Copyright (c) 2010 OTClient, https://github.com/edubart/otclient
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
#include "util.h"

#include <iostream>
#include <cstdlib>
#include <stdarg.h>

void Logger::_log(int level, const char *trace, const char *format, ...)
{
    va_list args;
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
        std::cout << colors[level];
#endif

    if(!strace.empty())
        std::cout << "[" << strace << "] ";

    static char const *prefixes[] = { "FATAL ERROR: ", "ERROR: ", "WARNING: ", "", "", "" };
    std::cout << prefixes[level];
    std::cout << text;

#ifdef linux
    if(colored)
        std::cout << "\033[0m";
#endif

    std::cout << std::endl;

    if(level == LFATAL)
        exit(-1);
}
