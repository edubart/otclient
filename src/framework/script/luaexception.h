#ifndef LUAEXCEPTION_H
#define LUAEXCEPTION_H

#include "luadeclarations.h"

/// LuaException, all lua errors are throwed by it
class LuaException : public std::exception
{
public:
    LuaException(const std::string& error, int traceLevel = -1);
    virtual ~LuaException() throw() { };

    void generateLuaErrorMessage(const std::string& error, int traceLevel);

    virtual const char* what() const throw() { return m_what.c_str(); }

protected:
    LuaException() { }

    std::string m_what;
};

class LuaBadNumberOfArgumentsException : public LuaException {
public:
    LuaBadNumberOfArgumentsException(int expected = -1, int got = -1);
};

class LuaBadTypeConversinException : public LuaException {
public:
    LuaBadTypeConversinException(const std::string& typeName = "an incompatible lua type");
};

class LuaBadValueCastException : public LuaException {
public:
    LuaBadValueCastException(const LuaValuePtr& value, const std::string& typeName = "an incompatible lua type");
};

#endif
