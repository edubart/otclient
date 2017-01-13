/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
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

#ifndef STDEXT_FORMAT_H
#define STDEXT_FORMAT_H

#include "traits.h"

#include <string>
#include <cstdio>
#include <cassert>
#include <tuple>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace stdext {

template<class T> void print_ostream(std::ostringstream& stream, const T& last) { stream << last; }
template<class T, class... Args>
void print_ostream(std::ostringstream& stream, const T& first, const Args&... rest) { stream << "\t" << first; print_ostream(stream, rest...); }
template<class... T>

// Utility for printing variables just like lua
void print(const T&... args) { std::ostringstream buf; print_ostream(buf, args...); std::cout << buf.str() << std::endl; }

template<typename T>
typename std::enable_if<std::is_integral<T>::value ||
                        std::is_pointer<T>::value ||
                        std::is_floating_point<T>::value ||
                        std::is_enum<T>::value, T>::type sprintf_cast(const T& t) { return t; }
inline const char *sprintf_cast(const char *s) { return s; }
inline const char *sprintf_cast(const std::string& s) { return s.c_str(); }

template<int N> struct expand_snprintf {
    template<typename Tuple, typename... Args> static int call(char *s, size_t maxlen, const char *format, const Tuple& tuple, const Args&... args) {
        return expand_snprintf<N-1>::call(s, maxlen, format, tuple, sprintf_cast(std::get<N-1>(tuple)), args...); }};
template<> struct expand_snprintf<0> {
    template<typename Tuple, typename... Args> static int call(char *s, size_t maxlen, const char *format, const Tuple& tuple, const Args&... args) {
#ifdef _MSC_VER
        return _snprintf(s, maxlen, format, args...);
#else
        return snprintf(s, maxlen, format, args...);
#endif
    }
};

// Improved snprintf that accepts std::string and other types
template<typename... Args>
int snprintf(char *s, size_t maxlen, const char *format, const Args&... args) {
    std::tuple<typename replace_extent<Args>::type...> tuple(args...);
    return expand_snprintf<std::tuple_size<decltype(tuple)>::value>::call(s, maxlen, format, tuple);
}

template<typename... Args>
inline int snprintf(char *s, size_t maxlen, const char *format) {
    std::strncpy(s, format, maxlen);
    s[maxlen-1] = 0;
    return strlen(s);
}

template<typename... Args>
inline std::string format() { return std::string(); }

template<typename... Args>
inline std::string format(const std::string& format) { return format; }

// Format strings with the sprintf style, accepting std::string and string convertible types for %s
template<typename... Args>
std::string format(const std::string& format, const Args&... args) {
    int n, size = 1024;
    std::string str;
    while(true) {
        str.resize(size);
        n = snprintf(&str[0], size, format.c_str(), args...);
        assert(n != -1);
        if(n < size) {
            str.resize(n);
            return str;
        }
        size *= 2;
    }
}

}

#endif
