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
    template<typename R, typename... T>
    R callLuaField(const std::string& field, const T&... args);

    /// Sets a field in this lua object
    template<typename T>
    void setLuaField(const std::string& key, const T& value);

    /// Gets a field from this lua object
    template<typename T>
    T getLuaField(const std::string& key);

    /// Release fields table reference
    void releaseLuaFieldsTable();

    /// Sets a field from this lua object, the value must be on the stack
    void luaSetField(const std::string& key);

    /// Gets a field from this lua object, the result is pushed onto the stack
    void luaGetField(const std::string& key);

    /// Returns the number of references of this object
    /// @note each userdata of this object on lua counts as a reference
    int getUseCount();

    /// Returns the class name used in Lua
    virtual std::string getLuaObjectName() const {
        // TODO: this could be cached for more performance
        return Fw::demangleName(typeid(*this).name());
    }

    LuaObjectPtr asLuaObject() { return shared_from_this(); }

private:
    int m_fieldsTableRef;
};

#include "luainterface.h"

template<typename... T>
int LuaObject::callLuaField(const std::string& field, const T&... args) {
    // note that the field must be retrieved from this object lua value
    // to force using the __index metamethod of it's metatable
    // so cannot use LuaObject::getField here
    // push field
    g_lua.pushObject(asLuaObject());
    g_lua.getField(field);

    if(!g_lua.isNil()) {
        // the first argument is always this object (self)
        g_lua.insert(-2);
        g_lua.polymorphicPush(args...);
        return g_lua.protectedCall(1 + sizeof...(args));
    } else {
        g_lua.pop(2);
    }
    return 0;
}

template<typename R, typename... T>
R LuaObject::callLuaField(const std::string& field, const T&... args) {
    R result;
    int rets = callLuaField(field, args...);
    if(rets > 0) {
        assert(rets == 1);
        result = g_lua.polymorphicPop<R>();
    } else
        result = R();
    return result;
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
