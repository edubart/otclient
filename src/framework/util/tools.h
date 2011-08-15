#ifndef TOOLS_H
#define TOOLS_H

#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <exception>
#include <cxxabi.h>

namespace fw {

// read utilities for istream
inline uint8 getu8(std::istream& in) {
    uint8 tmp;
    in.read((char*)&tmp, 1);
    return tmp;
}

inline uint16 getu16(std::istream& in) {
    uint16 tmp;
    in.read((char*)&tmp, 2);
    return tmp;
}

inline uint32 getu32(std::istream& in) {
    uint32 tmp;
    in.read((char*)&tmp, 4);
    return tmp;
}

/// Fill an ostream by concatenating args
/// Usage:
///   fw::fill_ostream(stream, a1, a2, ..., aN);
inline void fill_ostream(std::ostringstream&) { }
template<class T, class... Args>
void fill_ostream(std::ostringstream& stream, const T& first, const Args&... rest) {
    stream << first;
    fill_ostream(stream, rest...);
}

/// Makes a std::string by concatenating args
/// Usage:
///   std::string str = fw::mkstr(a1, a2, ..., aN);
template<class... T>
std::string mkstr(const T&... args) {
    std::ostringstream buf;
    fill_ostream(buf, args...);
    return buf.str();
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

/// Utility for dumping variables
/// Usage:
///   fw::dump << v1, v2, ..., vN;
struct dumper {
    dumper() { }
    template<class T>
    dump_util operator<<(const T& v) const {
        dump_util d;
        d << v;
        return d;
    }
};

/// Utility for printing messages into stdout
/// Usage:
///  fw::print(v1, v2, ..., vN);
template<class... T>
void print(const T&... args) {
    std::ostringstream buf;
    fill_ostream(buf, args...);
    std::cout << buf.str();
}

/// Same as fw::print but adds a new line at the end
template<class... T>
void println(const T&... args) {
    print(args...);
    std::cout << std::endl;
}

/// Demangle names for GNU g++ compiler
inline std::string demangle_name(const char* name) {
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

/// Returns the name of a type
/// e.g. fw::demangle_type<Foo*>() returns a string containing 'Foo*'
template<typename T>
std::string demangle_type() {
    return demangle_name(typeid(T).name());
}

/// Cast a type to another type
/// @return whether the conversion was successful or not
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
class bad_cast : public std::bad_cast {
public:
    virtual ~bad_cast() throw() { }
    template<class T, class R>
    void setWhat() {
        m_what = mkstr("failed to cast value of type '", demangle_type<T>(),
                                "' to type '", demangle_type<R>(), "'");
    }
    virtual const char* what() { return m_what.c_str(); }
private:
    std::string m_what;
};

/// Cast a type to another type, any error throws a fw::bad_cast_exception
/// Usage:
///   R r = fw::safe_cast<R>(t);
template<typename R, typename T>
R safe_cast(const T& t) {
    R r;
    if(!cast(t, r)) {
        bad_cast e;
        e.setWhat<T,R>();
        throw e;
    }
    return r;
}

/// Cast a type to another type, cast errors are ignored
/// Usage:
///   R r = fw::unsafe_cast<R>(t);
template<typename R, typename T>
R unsafe_cast(const T& t) {
    R r;
    try {
        r = safe_cast<R,T>(t);
    } catch(bad_cast& e) {
        println(e.what());
    }
    return r;
}

// an empty string to use anywhere needed
const static std::string empty_string;

}

// shortcut for fw::dump
const static fw::dumper dump;

#endif
