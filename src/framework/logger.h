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


#ifndef LOGGER_H
#define LOGGER_H

#include <sstream>

namespace Logger {

enum ELogLevel {
    LFATAL = 0,
    LERROR,
    LWARNING,
    LNOTICE,
    LDEBUG
};

void _log(int level, const char *trace, const char *format, ...);

}

#define logFatal(...) Logger::_log(Logger::LFATAL, NULL, __VA_ARGS__)
#define logError(...) Logger::_log(Logger::LERROR, NULL, __VA_ARGS__)
#define logWarning(...) Logger::_log(Logger::LWARNING, NULL, __VA_ARGS__)
#define logDebug(...) Logger::_log(Logger::LDEBUG, NULL, __VA_ARGS__)
#define logInfo(...) Logger::_log(Logger::LNOTICE, NULL, __VA_ARGS__)

#define logTrace() Logger::_log(Logger::LDEBUG, __PRETTY_FUNCTION__, "")
#define logDebugTrace(...) Logger::_log(Logger::LDEBUG, __PRETTY_FUNCTION__, __VA_ARGS__)

struct Dump {
public:
    Dump() { }
    ~Dump() { logDebug(m_buf.str().c_str()); }
    template<class T>  Dump &operator<<(const T &x) { m_buf << x << " "; return *this; }
private:
    std::ostringstream m_buf;
};

#define dump Dump()

#endif
