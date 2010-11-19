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

#include <string>

enum ELogLevel {
    LFATAL = 0,
    LERROR,
    LWARNING,
    LNOTICE,
    LDEBUG
};

void _log(int level, const char *trace, const char *format, ...);

#define fatal(...) _log(LFATAL, NULL, __VA_ARGS__)
#define error(...) _log(LERROR, NULL, __VA_ARGS__)
#define warning(...) _log(LWARNING, NULL, __VA_ARGS__)
#define debug(...) _log(LDEBUG, NULL, __VA_ARGS__)
#define notice(...) _log(LNOTICE, NULL, __VA_ARGS__)

#define trace() _log(LDEBUG, __PRETTY_FUNCTION__, "")
#define tdebug(...) _log(LDEBUG, __PRETTY_FUNCTION__, __VA_ARGS__)

#endif
