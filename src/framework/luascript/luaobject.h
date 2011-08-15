#ifndef LUAOBJECT_H
#define LUAOBJECT_H

#include "declarations.h"

/// LuaObject, all script-able classes have it as base
class LuaObject : public std::enable_shared_from_this<LuaObject>
{
public:
    LuaObject();
    virtual ~LuaObject();

    /// Calls a function or table of functions stored in a lua field, results are pushed onto the stack,
    /// if any lua error occurs, it will be reported to stdout and return 0 results
    /// @return the number of results
    template<typename... T>
    int callLuaField(const std::string& field, const T&... args);

    /// Sets a field in this lua object
    template<typename T>
    void setLuaField(const std::string& key, const T& value);

    /// Gets a field from this lua object
    template<typename T>
    T getLuaField(const std::string& key);

    /// Release fields table reference
    void luaReleaseFieldsTable();

    /// Sets a field from this lua object, the value must be on the stack
    void luaSetField(const std::string& key);

    /// Gets a field from this lua object, the result is pushed onto the stack
    void luaGetField(const std::string& key);

    /// Returns the number of references of this object
    /// @note each userdata of this object on lua counts as a reference
    int getUseCount();

    /// Returns the class name used in Lua
    virtual std::string getLuaObjectName() const {
        // this could later be cached for more performance
        return fw::demangle_name(typeid(*this).name());
    }

    LuaObjectPtr asLuaObject() { return shared_from_this(); }

private:
    int m_fieldsTableRef;
};

#include "luainterface.h"

template<typename... T>
int LuaObject::callLuaField(const std::string& field, const T&... args) {
    if(m_fieldsTableRef != -1) {
        // note that the field must be retrieved from this object lua value
        // to force using the __index metamethod of it's metatable
        // so cannot use LuaObject::getField here
        // push field
        g_lua.pushObject(asLuaObject());
        g_lua.getField(field);

        // the first argument is always this object (self)
        g_lua.insert(-2);
        g_lua.polymorphicPush(args...);
        return g_lua.protectedCall(1 + sizeof...(args));
    } else
        return 0;
}

template<typename T>
void LuaObject::setLuaField(const std::string& key, const T& value) {
    g_lua.polymorphicPush(value);
    luaSetField(key);
}

template<typename T>
T LuaObject::getLuaField(const std::string& key) {
    luaGetField(key);
    return g_lua.polymorphicPop<T>();
}

#endif
