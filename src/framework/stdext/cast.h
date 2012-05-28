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

#ifndef STDEXT_CAST_H
#define STDEXT_CAST_H

#include "string.h"
#include "exception.h"
#include "demangle.h"

namespace stdext {

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
class cast_exception : public exception {
public:
    virtual ~cast_exception() throw() { }
    template<class T, class R>
    void update_what() {
        m_what = format("failed to cast value of type '%s' to type '%s'", demangle_type<T>(), demangle_type<R>());
    }
    virtual const char* what() { return m_what.c_str(); }
private:
    std::string m_what;
};

// cast a type to another type, any error throws a cast_exception
template<typename R, typename T>
R safe_cast(const T& t) {
    R r;
    if(!cast(t, r)) {
        cast_exception e;
        e.update_what<T,R>();
        throw e;
    }
    return r;
}

// cast a type to another type, cast errors are ignored
template<typename R, typename T>
R unsafe_cast(const T& t, R def = R()) {
    try {
        return safe_cast<R,T>(t);
    } catch(cast_exception& e) {
        std::cout << "CAST ERROR: " << e.what() << std::endl;
        return def;
    }
}

}

#endif
