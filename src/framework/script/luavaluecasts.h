#ifndef LUAVALUECASTS_H
#define LUAVALUECASTS_H

#include "luadeclarations.h"
#include "luaexception.h"

// convert from common types
bool to_luavalue(const LuaValuePtr& v, LuaValuePtr& o);
bool to_luavalue(bool v, LuaValuePtr& o);
bool to_luavalue(int v, LuaValuePtr& o);
bool to_luavalue(double v, LuaValuePtr& o);
bool to_luavalue(const std::string& v, LuaValuePtr& o);
bool to_luavalue(const char* v, LuaValuePtr& o);
bool to_luavalue(int (*v)(LuaState*), LuaValuePtr& o);
bool to_luavalue(const LuaCppFunction& v, LuaValuePtr& o);

// convert from LuaObject
template<class T>
typename std::enable_if<std::is_base_of<LuaObject, typename T::element_type>::value, bool>::type
to_luavalue(const T& v, LuaValuePtr& o) {
    if(v) {
        o = v->toLuaValue();
        return true;
    }
    return false;
}

/// Safe convert, if any errors occurs throws an LuaException
template<class T>
LuaValuePtr safe_to_luavalue(const T& v) {
    LuaValuePtr o;
    if(!to_luavalue(v, o))
        throw LuaBadTypeConversinException();
    return o;
}

// common types casting
bool luavalue_cast(const LuaValuePtr& v, bool& o);
bool luavalue_cast(const LuaValuePtr& v, int& o);
bool luavalue_cast(const LuaValuePtr& v, double& o);
bool luavalue_cast(const LuaValuePtr& v, std::string& o);
bool luavalue_cast(const LuaValuePtr& v, LuaObjectPtr& o);

// convert enums
template<class T>
typename std::enable_if<std::is_enum<T>::value, bool>::type
luavalue_cast(const LuaValuePtr& v, T& o) {
    return luavalue_cast(v, (int&)o);
}

// cast pointer
template<class T>
bool luavalue_cast(const LuaValuePtr& v, T*& o) {
    LuaObjectPtr obj;
    if(!luavalue_cast(v, obj))
        return false;
    o = std::dynamic_pointer_cast<T>(obj).get();
    return !!o;
}

// cast shared pointers
template<class T>
bool luavalue_cast(const LuaValuePtr& v, std::shared_ptr<T>& o) {
    LuaObjectPtr obj;
    if(!luavalue_cast(v, obj))
        return false;
    o = std::dynamic_pointer_cast<T>(obj);
    return !!o;
}

/// Safe casting, if any errors occurs throws an LuaException
template<class T>
T safe_luavalue_cast(const LuaValuePtr& v) {
    T o;
    if(!luavalue_cast(v, o))
        throw LuaBadValueCastException(v);
    return o;
}

#endif // LUAVALUECASTS_H
