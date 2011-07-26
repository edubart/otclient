#include "luaexception.h"
#include "luainterface.h"

LuaException::LuaException(const std::string& error, int traceLevel)
{
    generateLuaErrorMessage(error, traceLevel);
}

void LuaException::generateLuaErrorMessage(const std::string& error, int traceLevel)
{
    // clear stack, to prevent further errors
    g_lua.clearStack();

    // append trace level to error message
    if(traceLevel >= 0)
        m_what = make_string("LUA ERROR: ", g_lua.getTraceback(error, traceLevel));
    else
        m_what = make_string("LUA ERROR: ", error);
}

LuaBadNumberOfArgumentsException::LuaBadNumberOfArgumentsException(int expected, int got)
{
    std::string error = "attempt to call a function with wrong number of arguments";
    if(expected >= 0 && got >= 0)
        error = make_string(error, " (expected ", expected, ", but got ", got, ")");
    generateLuaErrorMessage(error, 1);
}

LuaBadTypeConversinException::LuaBadTypeConversinException(const std::string& typeName)
{
    std::string error = make_string("attempt to convert ", typeName, " to a lua value");
    generateLuaErrorMessage(error, 0);
}

LuaBadValueCastException::LuaBadValueCastException(const LuaValuePtr& value, const std::string& typeName)
{
    std::string error = make_string("attempt to cast a ", value->getTypeName(), " lua value to ", typeName);
    generateLuaErrorMessage(error, 0);
}
