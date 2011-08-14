#ifndef LUAVALUECASTS_H
#define LUAVALUECASTS_H

// this file is and must be included only from luainterface.h
#include "luainterface.h"
#include "luaexception.h"

/// Pushes bool
inline void push_luavalue(bool v) {
    g_lua.pushBoolean(v);
}

/// Pushes int
inline void push_luavalue(int v) {
    g_lua.pushInteger(v);
}

/// Pushes double
inline void push_luavalue(double v) {
    g_lua.pushNumber(v);
}

/// Pushes std::string
inline void push_luavalue(const std::string& v) {
    g_lua.pushString(v);
}

/// Pushes const char*
inline void push_luavalue(const char* v) {
    g_lua.pushCString(v);
}

/// Pushes LuaCppFunction
inline void push_luavalue(const LuaCppFunction& v) {
    g_lua.pushCppFunction(v);
}

/// Pushes LuaObjectPtr
template<class T>
typename std::enable_if<std::is_base_of<LuaObject, typename T::element_type>::value, void>::type
push_luavalue(const T& v) {
    if(v)
        return g_lua.pushObject(v);
    return g_lua.pushNil();
}

/// Push std::function types
template<typename Ret, typename... Args>
void push_luavalue(const std::function<Ret(Args...)>& v) {
    if(v) {
        LuaCppFunction f = luabinder::bind_fun(v);
        g_lua.pushCppFunction(f);
    } else
        g_lua.pushNil();
}

/// Casts lua value to bool
inline bool luavalue_cast(int index, bool& o) {
    o = g_lua.toBoolean(index);
    return true;
}

/// Casts lua value to int
inline bool luavalue_cast(int index, int& o) {
    o = g_lua.toInteger(index);
    if(o == 0 && !g_lua.isNumber(index))
        return false;
    return true;
}

/// Casts lua value to double
inline bool luavalue_cast(int index, double& o) {
    o = g_lua.toNumber(index);
    if(o == 0 && !g_lua.isNumber(index))
        return false;
    return true;
}

/// Casts lua value to std::string
inline bool luavalue_cast(int index, std::string& o) {
    o = g_lua.toString(index);
    if(o.empty() && !g_lua.isString(index))
        return false;
    return true;
}

/// Casts lua value to LuaObjectPtr
inline bool luavalue_cast(int index, LuaObjectPtr& o) {
    if(g_lua.isUserdata(index)) {
        o = g_lua.toObject(index);
        return true;
    } else if(g_lua.isNil(index)) {
        o = nullptr;
        return true;
    }
    return false;
}

/// Casts lua value to enum
template<class T>
typename std::enable_if<std::is_enum<T>::value, bool>::type
luavalue_cast(int index, T& o) {
    return luavalue_cast(index, (int&)o);
}

/// Cast lua userdata to a class pointer
template<class T>
typename std::enable_if<std::is_base_of<LuaObject, T>::value, bool>::type
luavalue_cast(int index, T*& o) {
    LuaObjectPtr obj;
    if(!luavalue_cast(index, obj))
        return false;
    o = std::dynamic_pointer_cast<T>(obj).get();
    return !!o;
}

/// Cast lua userdata to a class shared pointer
template<class T>
bool luavalue_cast(int index, std::shared_ptr<T>& o) {
    LuaObjectPtr obj;
    if(!luavalue_cast(index, obj))
        return false;
    o = std::dynamic_pointer_cast<T>(obj);
    return !!o;
}

/// Cast a lua function to a std::function
template<typename... Args>
bool luavalue_cast(int index, std::function<void(Args...)>& o) {
    if(g_lua.isFunction(index)) {
        g_lua.pushValue(index);
        // weak references are used here, this means that the script must hold another reference
        // to this function, otherwise it will expire
        int funcWeakRef = g_lua.weakRef();
        o = [=](Args... args...) {
            // note that we must catch exceptions, because this lambda can be called from anywhere
            // and most of them won't catch exceptions (e.g. dispatcher)
            g_lua.getWeakRef(funcWeakRef);
            try {
                if(g_lua.isFunction()) {
                    g_lua.polymorphicPush(args...);
                    assert(g_lua.safeCall(sizeof...(Args)) == 0);
                } else {
                    throw LuaException("attempt to call an expired lua function from C++,"
                                       "did you forget to hold a reference for that function?", 0);
                }
            } catch(std::exception& e) {
                logError(e.what());
            }
        };
        return true;
    } else if(g_lua.isNil(index)) {
        o = std::function<void(Args...)>();
        return true;
    }
    return false;
}

/// Cast a lua function to a std::function that can return
template<typename Ret, typename... Args>
typename std::enable_if<!std::is_void<Ret>::value, bool>::type
luavalue_cast(int index, std::function<Ret(Args...)>& o) {
    if(g_lua.isFunction(index)) {
        g_lua.pushValue(index);
        // weak references are used here, this means that the script must hold another reference
        // to this function, otherwise it will expire
        int funcWeakRef = g_lua.weakRef();
        o = [=](Args... args...) -> Ret {
            // note that we must catch exceptions, because this lambda can be called from anywhere
            // and most of them won't catch exceptions (e.g. dispatcher)
            try {
                g_lua.getWeakRef(funcWeakRef);
                if(g_lua.isFunction()) {
                    g_lua.polymorphicPush(args...);
                    if(g_lua.safeCall(sizeof...(Args)) != 1)
                        throw LuaException("a function from lua didn't retrieve the expected number of results", 0);
                    return g_lua.polymorphicPop<Ret>();
                } else {
                    throw LuaException("attempt to call an expired lua function from C++,"
                                       "did you forget to hold a reference for that function?", 0);
                }
            } catch(std::exception& e) {
                logError(e.what());
            }
            return Ret();
        };
        return true;
    } else if(g_lua.isNil(index)) {
        o = std::function<Ret(Args...)>();
        return true;
    }
    return false;
}

// additional casts


inline void push_luavalue(const Color& v) {
    g_lua.newTable();
    g_lua.pushInteger(v.r());
    g_lua.setField("r");
    g_lua.pushInteger(v.g());
    g_lua.setField("g");
    g_lua.pushInteger(v.b());
    g_lua.setField("b");
    g_lua.pushInteger(v.a());
    g_lua.setField("a");
}

inline bool luavalue_cast(int index, Color& o) {
    if(!g_lua.isTable(index))
        return false;

    g_lua.getField("r", index);
    o.setRed(g_lua.popInteger());
    g_lua.getField("g", index);
    o.setGreen(g_lua.popInteger());
    g_lua.getField("b", index);
    o.setBlue(g_lua.popInteger());
    g_lua.getField("a", index);
    o.setAlpha(g_lua.popInteger());
    return true;
}

#endif
