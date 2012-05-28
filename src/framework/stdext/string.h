/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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

#ifndef STDEXT_STRING_H
#define STDEXT_STRING_H

#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <boost/algorithm/string.hpp>

#include "types.h"
#include "cast.h"
#include "exception.h"

namespace stdext {

// casts declaration, definition will be included at the end of the file
template<typename R, typename T> R safe_cast(const T& t);
template<typename R, typename T> R unsafe_cast(const T& t, R def = R());

/// Convert any type to std::string
template<typename T>
std::string to_string(const T& t) { return unsafe_cast<std::string, T>(t); }

/// Convert any type from std::string
template<typename T>
T from_string(const std::string& str, T def = T()) { return unsafe_cast<T, std::string>(str, def); }

/// Cast non-class types like int, char, float, double and pointers
template<typename T>
typename std::enable_if<std::is_integral<T>::value ||
                        std::is_pointer<T>::value ||
                        std::is_floating_point<T>::value ||
                        std::is_enum<T>::value, T>::type sprintf_cast(const T& t) { return t; }

/// Cast any class or struct convertible to std::string
inline const char *sprintf_cast(const std::string& s) { return s.c_str(); }

template<int N>
struct expand_snprintf{
    template<typename Tuple, typename... Args>
    static int call(char *s, size_t maxlen, const char *format, const Tuple& tuple, const Args&... args) {
        return expand_snprintf<N-1>::call(s, maxlen, format, tuple, sprintf_cast(std::get<N-1>(tuple)), args...);
    }
};
template<>
struct expand_snprintf<0> {
    template<typename Tuple, typename... Args>
    static int call(char *s, size_t maxlen, const char *format, const Tuple& tuple, const Args&... args) {
        return snprintf(s, maxlen, format, args...);
    }
};

template<class T>
struct replace_extent { typedef T type; };

template<class T>
struct replace_extent<T[]> { typedef const T* type; };

template<class T, std::size_t N>
struct replace_extent<T[N]> { typedef const T* type;};

/// Improved sprintf that accepts std::string and other types
template<typename... Args>
int snprintf(char *s, size_t maxlen, const char *format, const Args&... args) {
    typedef typename std::tuple<typename replace_extent<Args>::type...> Tuple;
    enum { N = std::tuple_size<Tuple>::value };
    Tuple tuple(args...);
    return expand_snprintf<N>::call(s, maxlen, format, tuple);
}

/// Format strings with the sprintf style, accepting std::string and string convertible types for %s
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

inline void fill_ostream(std::ostringstream&) { }

/// Fills an ostream by concatenating args
template<class T, class... Args>
void fill_ostream(std::ostringstream& stream, const T& first, const Args&... rest) {
    stream << first;
    fill_ostream(stream, rest...);
}

/// Makes a std::string by concatenating args
template<class... T>
std::string mkstr(const T&... args) {
    std::ostringstream buf;
    fill_ostream(buf, args...);
    return buf.str();
}

/// Easy of use split
template<typename T = std::string>
std::vector<T> split(const std::string& str, const std::string& separators = " ") {
    std::vector<std::string> splitted;
    boost::split(splitted, str, boost::is_any_of(std::string(separators)));
    std::vector<T> results(splitted.size());
    for(uint i=0;i<splitted.size();++i)
        results[i] = safe_cast<T>(splitted[i]);
    return results;
}

/// Resolve a file path by combining sourcePath with filePath
inline std::string resolve_path(const std::string& filePath, std::string sourcePath) {
    if(boost::starts_with(filePath, "/"))
        return filePath;
    if(!boost::ends_with(sourcePath, "/")) {
        std::size_t slashPos = sourcePath.find_last_of("/");
        if(slashPos == std::string::npos)
            throw_exception(format("invalid source path '%s', for file '%s'", sourcePath, filePath));
        sourcePath = sourcePath.substr(0, slashPos + 1);
    }
    return sourcePath + filePath;
}

/// Get current date and time in a std::string
inline std::string date_time_string() {
    char date[32];
    std::time_t tnow;
    std::time(&tnow);
    std::tm *ts = std::localtime(&tnow);
    std::strftime(date, 32, "%b %d %Y %H:%M:%S", ts);
    return std::string(date);
}

/// Convert decimal to hexadecimal
inline std::string dec_to_hex(unsigned int num) {
    std::string str;
    std::ostringstream o;
    o << std::hex << num;
    str = o.str();
    return str;
}

/// Convert hexadecimal to decimal
inline unsigned int hex_to_dec(const std::string& str) {
    unsigned int num;
    std::istringstream i(str);
    i >> std::hex >> num;
    return num;
}

/// Convert ip to string
inline std::string ip_to_string(uint32 ip) {
    char host[16];
    sprintf(host, "%d.%d.%d.%d", (uint8)ip, (uint8)(ip >> 8), (uint8)(ip >> 16), (uint8)(ip >> 24));
    return std::string(host);
}

/// Convert utf8 characters to latin1
inline char utf8CharToLatin1(uchar *utf8, int *read) {
    char c = '?';
    uchar opt1 = utf8[0];
    *read = 1;
    if(opt1 == 0xc3) {
        *read = 2;
        uchar opt2 = utf8[1];
        c = 64 + opt2;
    } else if(opt1 == 0xc2) {
        *read = 2;
        uchar opt2 = utf8[1];
        if(opt2 > 0xa1 && opt2 < 0xbb)
            c = opt2;
    } else if(opt1 < 0xc2) {
        c = opt1;
    }
    return c;
}

/// Convert utf8 strings to latin1
inline std::string utf8StringToLatin1(uchar *utf8) {
    std::string out;
    int len = strlen((char*)utf8);
    for(int i=0; i<len;) {
        int read = 0;
        uchar *utf8char = &utf8[i];
        out += utf8CharToLatin1(utf8char, &read);
        i += read;
    }
    return out;
}

// utility for printing messages into stdout
template<class... T>
void print(const T&... args) {
    std::ostringstream buf;
    fill_ostream(buf, args...);
    std::cout << buf.str();
}

template<class... T>
void println(const T&... args) {
    print(args...);
    std::cout << std::endl;
}

}

#include "cast.h"

#endif
