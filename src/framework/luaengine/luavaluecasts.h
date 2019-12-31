/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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

#ifndef LUAVALUECASTS_H
#define LUAVALUECASTS_H

// this file is and must be included only from luainterface.h

#include "declarations.h"
#include <framework/otml/declarations.h>

template<typename T>
int push_internal_luavalue(T v);

// bool
int push_luavalue(bool b);
bool luavalue_cast(int index, bool& b);

// int
int push_luavalue(int i);
bool luavalue_cast(int index, int& i);

// double
int push_luavalue(double d);
bool luavalue_cast(int index, double& d);

// float
inline int push_luavalue(float f) { push_luavalue((double)f); return 1; }
inline bool luavalue_cast(int index, float& f) { double d; bool r = luavalue_cast(index, d); f = d; return r; }

// int8
inline int push_luavalue(int8 v) { push_luavalue((int)v); return 1; }
inline bool luavalue_cast(int index, int8& v) { int i; bool r = luavalue_cast(index, i); v = i; return r; }
// uint8
inline int push_luavalue(uint8 v) { push_luavalue((int)v); return 1; }
inline bool luavalue_cast(int index, uint8& v){ int i; bool r = luavalue_cast(index, i); v = i; return r; }
// int16
inline int push_luavalue(int16 v) { push_luavalue((int)v); return 1; }
inline bool luavalue_cast(int index, int16& v){ int i; bool r = luavalue_cast(index, i); v = i; return r; }
// uint16
inline int push_luavalue(uint16 v) { push_luavalue((int)v); return 1; }
inline bool luavalue_cast(int index, uint16& v){ int i; bool r = luavalue_cast(index, i); v = i; return r; }
// uint32
inline int push_luavalue(uint32 v) { push_luavalue((double)v); return 1; }
inline bool luavalue_cast(int index, uint32& v) { double d; bool r = luavalue_cast(index, d); v = d; return r; }
// int64
inline int push_luavalue(int64 v) { push_luavalue((double)v); return 1; }
inline bool luavalue_cast(int index, int64& v) { double d; bool r = luavalue_cast(index, d); v = d; return r; }
// uint64
inline int push_luavalue(uint64 v) { push_luavalue((double)v); return 1; }
inline bool luavalue_cast(int index, uint64& v) { double d; bool r = luavalue_cast(index, d); v = d; return r; }

// string
int push_luavalue(const char* cstr);
int push_luavalue(const std::string& str);
bool luavalue_cast(int index, std::string& str);

// lua cpp function
int push_luavalue(const LuaCppFunction& func);

// color
int push_luavalue(const Color& color);
bool luavalue_cast(int index, Color& color);

// rect
int push_luavalue(const Rect& rect);
bool luavalue_cast(int index, Rect& rect);

// point
int push_luavalue(const Point& point);
bool luavalue_cast(int index, Point& point);

// size
int push_luavalue(const Size& size);
bool luavalue_cast(int index, Size& size);

// otml nodes
int push_luavalue(const OTMLNodePtr& node);
bool luavalue_cast(int index, OTMLNodePtr& node);

// enum
template<class T>
typename std::enable_if<std::is_enum<T>::value, int>::type
push_luavalue(T e) { return push_luavalue((int)e); }

template<class T>
typename std::enable_if<std::is_enum<T>::value, bool>::type
luavalue_cast(int index, T& myenum);

// LuaObject pointers
template<class T>
typename std::enable_if<std::is_base_of<LuaObject, typename T::element_type>::value, int>::type
push_luavalue(const T& obj);

bool luavalue_cast(int index, LuaObjectPtr& obj);

template<class T>
typename std::enable_if<std::is_base_of<LuaObject, T>::value, bool>::type
luavalue_cast(int index, stdext::shared_object_ptr<T>& ptr);

// std::function
template<typename Ret, typename... Args>
int push_luavalue(const std::function<Ret(Args...)>& func);

template<typename... Args>
bool luavalue_cast(int index, std::function<void(Args...)>& func);

template<typename Ret, typename... Args>
typename std::enable_if<!std::is_void<Ret>::value, bool>::type
luavalue_cast(int index, std::function<Ret(Args...)>& func);

// list
template<typename T>
int push_luavalue(const std::list<T>& list);

template<typename T>
bool luavalue_cast(int index, std::list<T>& list);

// vector
template<typename T>
int push_luavalue(const std::vector<T>& vec);

template<typename T>
bool luavalue_cast(int index, std::vector<T>& vec);

// deque
template<class T>
int push_luavalue(const std::deque<T>& vec);

template<typename T>
bool luavalue_cast(int index, std::deque<T>& vec);

// map
template<class K, class V>
int push_luavalue(const std::map<K, V>& map);

template<class K, class V>
bool luavalue_cast(int index, std::map<K, V>& map);

// tuple
template<typename... Args>
int push_luavalue(const std::tuple<Args...>& tuple);

template<typename... Args>
int push_internal_luavalue(const std::tuple<Args...>& tuple);

// start definitions

#include "luaexception.h"
#include "luainterface.h"
#include "luaobject.h"

template<typename T>
int push_internal_luavalue(T v) {
    return push_luavalue(v);
}

template<class T>
typename std::enable_if<std::is_enum<T>::value, bool>::type
luavalue_cast(int index, T& myenum) {
    int i;
    if(luavalue_cast(index, i)) {
        myenum = (T)i;
        return true;
    }
    return false;
}

template<class T>
typename std::enable_if<std::is_base_of<LuaObject, typename T::element_type>::value, int>::type
push_luavalue(const T& obj) {
    if(obj)
        g_lua.pushObject(obj);
    else
        g_lua.pushNil();
    return 1;
}

template<class T>
typename std::enable_if<std::is_base_of<LuaObject, T>::value, bool>::type
luavalue_cast(int index, stdext::shared_object_ptr<T>& ptr) {
    LuaObjectPtr obj;
    if(!luavalue_cast(index, obj))
        return false;
    if(obj)
        ptr = obj->dynamic_self_cast<T>();
    else
        ptr = nullptr;
    return true;
}

template<typename Ret, typename... Args>
int push_luavalue(const std::function<Ret(Args...)>& func) {
    if(func) {
        LuaCppFunction f = luabinder::bind_fun(func);
        g_lua.pushCppFunction(f);
    } else
        g_lua.pushNil();
    return 1;
}

template<typename... Args>
bool luavalue_cast(int index, std::function<void(Args...)>& func) {
    if(g_lua.isFunction(index)) {
        g_lua.pushValue(index);
        // weak references are used here, this means that the script must hold another reference
        // to this function, otherwise it will expire
        int funcWeakRef = g_lua.weakRef();
        func = [=](Args... args) {
            // note that we must catch exceptions, because this lambda can be called from anywhere
            // and most of them won't catch exceptions (e.g. dispatcher)
            g_lua.getWeakRef(funcWeakRef);
            try {
                if(g_lua.isFunction()) {
                    int numArgs = g_lua.polymorphicPush(args...);
                    int rets = g_lua.safeCall(numArgs);
                    g_lua.pop(rets);
                } else {
                    throw LuaException("attempt to call an expired lua function from C++,"
                                       "did you forget to hold a reference for that function?", 0);
                }
            } catch(LuaException& e) {
                g_logger.error(stdext::format("lua function callback failed: %s", e.what()));
            }
        };
        return true;
    } else if(g_lua.isNil(index)) {
        func = std::function<void(Args...)>();
        return true;
    }
    return false;
}

template<typename Ret, typename... Args>
typename std::enable_if<!std::is_void<Ret>::value, bool>::type
luavalue_cast(int index, std::function<Ret(Args...)>& func) {
    if(g_lua.isFunction(index)) {
        g_lua.pushValue(index);
        // weak references are used here, this means that the script must hold another reference
        // to this function, otherwise it will expire
        int funcWeakRef = g_lua.weakRef();
        func = [=](Args... args) -> Ret {
            // note that we must catch exceptions, because this lambda can be called from anywhere
            // and most of them won't catch exceptions (e.g. dispatcher)
            try {
                g_lua.getWeakRef(funcWeakRef);
                if(g_lua.isFunction()) {
                    int numArgs = g_lua.polymorphicPush(args...);
                    if(g_lua.safeCall(numArgs) != 1)
                        throw LuaException("a function from lua didn't retrieve the expected number of results", 0);
                    return g_lua.polymorphicPop<Ret>();
                } else {
                    throw LuaException("attempt to call an expired lua function from C++,"
                                       "did you forget to hold a reference for that function?", 0);
                }
            } catch(LuaException& e) {
                g_logger.error(stdext::format("lua function callback failed: %s", e.what()));
            }
            return Ret();
        };
        return true;
    } else if(g_lua.isNil(index)) {
        func = std::function<Ret(Args...)>();
        return true;
    }
    return false;
}

template<typename T>
int push_luavalue(const std::list<T>& list) {
    g_lua.createTable(list.size(), 0);
    int i = 1;
    for(const T& v : list) {
        push_internal_luavalue(v);
        g_lua.rawSeti(i);
        i++;
    }
    return 1;
}

template<typename T>
bool luavalue_cast(int index, std::list<T>& list)
{
    if(g_lua.isTable(index)) {
        g_lua.pushNil();
        while(g_lua.next(index < 0 ? index-1 : index)) {
            T value;
            if(luavalue_cast(-1, value))
                list.push_back(value);
            g_lua.pop();
        }
        return true;
    }
    return false;
}

template<typename T>
int push_luavalue(const std::vector<T>& vec) {
    g_lua.createTable(vec.size(), 0);
    int i = 1;
    for(const T& v : vec) {
        push_internal_luavalue(v);
        g_lua.rawSeti(i);
        i++;
    }
    return 1;
}

template<typename T>
bool luavalue_cast(int index, std::vector<T>& vec)
{
    if(g_lua.isTable(index)) {
        g_lua.pushNil();
        while(g_lua.next(index < 0 ? index-1 : index)) {
            T value;
            if(luavalue_cast(-1, value))
                vec.push_back(value);
            g_lua.pop();
        }
        return true;
    }
    return false;
}

template<typename T>
int push_luavalue(const std::deque<T>& vec) {
    g_lua.createTable(vec.size(), 0);
    int i = 1;
    for(const T& v : vec) {
        push_internal_luavalue(v);
        g_lua.rawSeti(i);
        i++;
    }
    return 1;
}

template<typename T>
bool luavalue_cast(int index, std::deque<T>& vec)
{
    if(g_lua.isTable(index)) {
        g_lua.pushNil();
        while(g_lua.next(index < 0 ? index-1 : index)) {
            T value;
            if(luavalue_cast(-1, value))
                vec.push_back(value);
            g_lua.pop();
        }
        return true;
    }
    return false;
}

template<class K, class V>
int push_luavalue(const std::map<K, V>& map)
{
    g_lua.newTable();
    for(auto& it : map) {
        push_internal_luavalue(it.first);
        push_internal_luavalue(it.second);
        g_lua.rawSet();
    }
    return 1;
}

template<class K, class V>
bool luavalue_cast(int index, std::map<K, V>& map)
{
    if(g_lua.isTable(index)) {
        g_lua.pushNil();
        while(g_lua.next(index < 0 ? index-1 : index)) {
            K key;
            V value;
            if(luavalue_cast(-1, value) && luavalue_cast(-2, key))
                map[key] = value;
            g_lua.pop();
        }
        return true;
    }
    return false;
}


template<int N>
struct push_tuple_internal_luavalue {
    template<typename Tuple>
    static void call(const Tuple& tuple) {
        push_internal_luavalue(std::get<N-1>(tuple));
        g_lua.rawSeti(N);
        push_tuple_internal_luavalue<N-1>::call(tuple);
    }
};

template<>
struct push_tuple_internal_luavalue<0> {
    template<typename Tuple>
    static void call(const Tuple& tuple) { }
};

template<typename... Args>
int push_internal_luavalue(const std::tuple<Args...>& tuple) {
    g_lua.newTable();
    push_tuple_internal_luavalue<sizeof...(Args)>::call(tuple);
    return 1;
}

template<int N>
struct push_tuple_luavalue {
    template<typename Tuple>
    static void call(const Tuple& tuple) {
        push_internal_luavalue(std::get<std::tuple_size<Tuple>::value - N>(tuple));
        push_tuple_luavalue<N-1>::call(tuple);
    }
};

template<>
struct push_tuple_luavalue<0> {
    template<typename Tuple>
    static void call(const Tuple& tuple) { }
};

template<typename... Args>
int push_luavalue(const std::tuple<Args...>& tuple) {
    push_tuple_luavalue<sizeof...(Args)>::call(tuple);
    return sizeof...(Args);
}

#endif
