#include "luavaluecasts.h"
#include "luainterface.h"

bool to_luavalue(const LuaValuePtr& v, LuaValuePtr& o) {
    o = v;
    return true;
}

bool to_luavalue(bool v, LuaValuePtr& o) {
    o = g_lua.createBoolean(v);
    return true;
}

bool to_luavalue(int v, LuaValuePtr& o) {
    o = g_lua.createInteger(v);
    return true;
}

bool to_luavalue(double v, LuaValuePtr& o) {
    o = g_lua.createNumber(v);
    return true;
}

bool to_luavalue(const std::string& v, LuaValuePtr& o) {
    o = g_lua.createString(v);
    return true;
}

bool to_luavalue(const char* v, LuaValuePtr& o) {
    o = g_lua.createString(v);
    return true;
}

bool to_luavalue(int (*v)(LuaState*), LuaValuePtr& o) {
    o = g_lua.createCppFunction(v);
    return true;
}

bool to_luavalue(const LuaCppFunction& v, LuaValuePtr& o) {
    o = g_lua.createCppFunction(v);
    return true;
}

bool luavalue_cast(const LuaValuePtr& v, bool& o) {
    o = v->toBoolean();
    return true;
}

bool luavalue_cast(const LuaValuePtr& v, int& o) {
    o = v->toInteger();
    return true;
}

bool luavalue_cast(const LuaValuePtr& v, double& o) {
    o = v->toNumber();
    return true;
}

bool luavalue_cast(const LuaValuePtr& v, std::string& o) {
    o = v->toString();
    return true;
}

bool luavalue_cast(const LuaValuePtr& v, LuaObjectPtr& o) {
    if(v->isUserdata()) {
        LuaObjectPtr* objRef = static_cast<LuaObjectPtr*>(v->toUserdata());
        if(objRef && *objRef) {
            o = *objRef;
            return true;
        }
    } else if(v->isNil()) {
        o = LuaObjectPtr();
        return true;
    }
    return false;
}
