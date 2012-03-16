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

#ifndef TOOLS_H
#define TOOLS_H

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <exception>
#include <cxxabi.h>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "types.h"
#include "exception.h"

namespace Fw {

// read utilities for istream
inline uint8 getU8(std::istream& in) {
    uint8 tmp;
    in.read((char*)&tmp, 1);
    return tmp;
}

inline uint16 getU16(std::istream& in) {
    uint16 tmp;
    in.read((char*)&tmp, 2);
    return tmp;
}

inline uint32 getU32(std::istream& in) {
    uint32 tmp;
    in.read((char*)&tmp, 4);
    return tmp;
}

// fills an ostream by concatenating args
inline void fillOstream(std::ostringstream&) { }
template<class T, class... Args>
void fillOstream(std::ostringstream& stream, const T& first, const Args&... rest) {
    stream << first;
    fillOstream(stream, rest...);
}

// makes a std::string by concatenating args
template<class... T>
std::string mkstr(const T&... args) {
    std::ostringstream buf;
    fillOstream(buf, args...);
    return buf.str();
}

// throw a generic expcetion
template<typename... T>
void throwException(const T&... args) {
    throw Exception(mkstr(args...));
}

// used by dumper
struct dump_util {
    ~dump_util() { std::cout << std::endl; }
    template<class T>
    dump_util& operator<<(const T& v) {
        std::cout << v << " ";
        return *this;
    }
};

// utility for dumping variables
struct dumper {
    dumper() { }
    template<class T>
    dump_util operator<<(const T& v) const {
        dump_util d;
        d << v;
        return d;
    }
};

// utility for printing messages into stdout
template<class... T>
void print(const T&... args) {
    std::ostringstream buf;
    fillOstream(buf, args...);
    std::cout << buf.str();
}

template<class... T>
void println(const T&... args) {
    print(args...);
    std::cout << std::endl;
}

// useful std::string version of sprintf :)
template<typename... Args>
std::string formatString(const std::string& format, Args... args) {
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

// demangle names for GNU g++ compiler
inline std::string demangleName(const char* name) {
    size_t len;
    int status;
    std::string ret;
    char* demangled = abi::__cxa_demangle(name, 0, &len, &status);
    if(demangled) {
        ret = demangled;
        free(demangled);
    }
    return ret;
}

// returns the name of a type
template<typename T>
std::string demangleType() {
    return demangleName(typeid(T).name());
}

// cast a type to another type
template<typename T, typename R>
bool cast(const T& in, R& out) {
    std::stringstream ss;
    ss << in;
    ss >> out;
    return !!ss && ss.eof();
}

// cast a type to string
template<typename T>
bool cast(const T& in, std::string& out) {
    std::stringstream ss;
    ss << in;
    out = ss.str();
    return true;
}

// cast string to string
template<>
inline bool cast(const std::string& in, std::string& out) {
    out = in;
    return true;
}

// special cast from string to boolean
template<>
inline bool cast(const std::string& in, bool& b) {
    static std::string validNames[2][4] = {{"true","yes","on","1"}, {"false","no","off","0"}};
    bool ret = false;
    for(int i=0;i<4;++i) {
        if(in == validNames[0][i]) {
            b = true;
            ret = true;
            break;
        } else if(in == validNames[1][i]) {
            b = false;
            ret = true;
            break;
        }
    }
    return ret;
}

// special cast from boolean to string
template<>
inline bool cast(const bool& in, std::string& out) {
    out = (in ? "true" : "false");
    return true;
}

// used by safe_cast
class cast_exception : public Exception {
public:
    virtual ~cast_exception() throw() { }
    template<class T, class R>
    void setWhat() {
        m_what = mkstr("failed to cast value of type '", demangleType<T>(),
                                "' to type '", demangleType<R>(), "'");
    }
    virtual const char* what() { return m_what.c_str(); }
private:
    std::string m_what;
};

// cast a type to another type, any error throws a cast_exception
template<typename R, typename T>
R safeCast(const T& t) {
    R r;
    if(!cast(t, r)) {
        cast_exception e;
        e.setWhat<T,R>();
        throw e;
    }
    return r;
}

// cast a type to another type, cast errors are ignored
template<typename R, typename T>
R unsafeCast(const T& t, R def = R()) {
    try {
        return safeCast<R,T>(t);
    } catch(cast_exception& e) {
        println("CAST ERROR: ", e.what());
        return def;
    }
}

template<typename T>
std::string tostring(const T& t) {
    return unsafeCast<std::string, T>(t);
}

template<typename T>
T fromstring(const std::string& str, T def = T()) {
    return unsafeCast<T, std::string>(str, def);
}

inline std::string dec2hex(unsigned int num) {
    std::string str;
    std::ostringstream o;
    o << std::hex << num;
    str = o.str();
    return str;
}

inline unsigned int hex2dec(const std::string& str) {
    unsigned int num;
    std::istringstream i(str);
    i >> std::hex >> num;
    return num;
}

inline std::string ip2str(uint32 ip) {
    char host[16];
    sprintf(host, "%d.%d.%d.%d", (uint8)ip, (uint8)(ip >> 8), (uint8)(ip >> 16), (uint8)(ip >> 24));
    return std::string(host);
}

template<typename T = std::string>
std::vector<T> split(const std::string& str, const std::string& separators = " ") {
    std::vector<std::string> splitted;
    boost::split(splitted, str, boost::is_any_of(std::string(separators)));
    std::vector<T> results(splitted.size());
    for(uint i=0;i<splitted.size();++i)
        results[i] = safeCast<T>(splitted[i]);
    return results;
}

inline std::string resolvePath(const std::string& file, std::string sourcePath) {
    if(boost::starts_with(file, "/"))
        return file;
    if(!boost::ends_with(sourcePath, "/")) {
        std::size_t slashPos = sourcePath.find_last_of("/");
        if(slashPos == std::string::npos)
            throwException("invalid source path '", sourcePath, "' for file '", file, "'");
        sourcePath = sourcePath.substr(0, slashPos + 1);
    }
    return sourcePath + file;
}

template<typename T>
T randomRange(T min, T max);

template<>
inline int randomRange<int>(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> dis(0, 2147483647);
    return min + (dis(gen) % (max - min + 1));
}

template<>
inline float randomRange<float>(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0, 1.0);
    return min + (max - min)*dis(gen);
}

}

// shortcut for Fw::dump
const static Fw::dumper dump;

#endif
