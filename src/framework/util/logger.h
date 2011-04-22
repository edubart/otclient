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
#include <boost/format.hpp>

class Logger {
public:
    enum ELogLevel {
        LFATAL = 0,
        LERROR,
        LWARNING,
        LINFO,
        LDEBUG
    };

    static void log(int level, const std::string& text = "", const char *trace = NULL);
};

#define logFatal(a) Logger::log(Logger::LFATAL, a)
#define logError(a) Logger::log(Logger::LERROR, a)
#define logWarning(a) Logger::log(Logger::LWARNING, a)
#define logDebug(a) Logger::log(Logger::LDEBUG, a)
#define logInfo(a) Logger::log(Logger::LINFO, a)

#define flogFatal(a,b) Logger::log(Logger::LFATAL, (boost::format(a) % b).str())
#define flogError(a,b) Logger::log(Logger::LERROR, (boost::format(a) % b).str())
#define flogWarning(a,b) Logger::log(Logger::LWARNING, (boost::format(a) % b).str())
#define flogDebug(a,b) Logger::log(Logger::LDEBUG, (boost::format(a) % b).str())
#define flogInfo(a,b) Logger::log(Logger::LINFO, (boost::format(a) % b).str())

#define logTrace() Logger::log(Logger::LDEBUG, "", __PRETTY_FUNCTION__)
#define logTraceFatal(a) Logger::log(Logger::LFATAL, a, __PRETTY_FUNCTION__)
#define logTraceError(a) Logger::log(Logger::LERROR, a, __PRETTY_FUNCTION__)
#define logTraceWarning(a) Logger::log(Logger::LWARNING, a, __PRETTY_FUNCTION__)
#define logTraceDebug(a) Logger::log(Logger::LDEBUG, a, __PRETTY_FUNCTION__)
#define logTraceInfo(a) Logger::log(Logger::LINFO, a, __PRETTY_FUNCTION__)

struct Dump {
public:
    Dump() { }
    ~Dump() { logDebug(m_buf.str().c_str()); }
    template<class T>  Dump &operator<<(const T &x) { m_buf << x << " "; return *this; }
private:
    std::ostringstream m_buf;
};

#define dump Dump()

#endif // LOGGER_H
