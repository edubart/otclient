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

#ifndef LUAINTERFACE_H
#define LUAINTERFACE_H

#include "declarations.h"

struct lua_State;
typedef int (*LuaCFunction) (lua_State *L);

/// Class that manages LUA stuff
class LuaInterface
{
public:
    LuaInterface();
    ~LuaInterface();

    void init();
    void terminate();

    /// Register core script functions
    void registerFunctions();

    // functions that will register all script stuff in lua global environment
    void registerSingletonClass(const std::string& className);
    void registerClass(const std::string& className, const std::string& baseClass = "LuaObject");

    void registerClassStaticFunction(const std::string& className,
                                     const std::string& functionName,
                                     const LuaCppFunction& function);

    void registerClassMemberFunction(const std::string& className,
                                     const std::string& functionName,
                                     const LuaCppFunction& function);

    void registerClassMemberField(const std::string& className,
                                  const std::string& field,
                                  const LuaCppFunction& getFunction,
                                  const LuaCppFunction& setFunction);

    void registerGlobalFunction(const std::string& functionName,
                                const LuaCppFunction& function);

    // register shortcuts using templates
    template<class C, class B = LuaObject>
    void registerClass() {
        registerClass(stdext::demangle_class<C>(), stdext::demangle_class<B>());
    }

    template<class C>
    void registerClassStaticFunction(const std::string& functionName, const LuaCppFunction& function) {
        registerClassStaticFunction(stdext::demangle_class<C>(), functionName, function);
    }

    template<class C>
    void registerClassMemberFunction(const std::string& functionName, const LuaCppFunction& function) {
        registerClassMemberFunction(stdext::demangle_class<C>(), functionName, function);
    }

    template<class C>
    void registerClassMemberField(const std::string& field,
                                  const LuaCppFunction& getFunction,
                                  const LuaCppFunction& setFunction) {
        registerClassMemberField(stdext::demangle_class<C>(), field, getFunction, setFunction);
    }

    // methods for binding functions
    template<class C, typename F>
    void bindSingletonFunction(const std::string& functionName, F C::*function, C *instance);
    template<class C, typename F>
    void bindSingletonFunction(const std::string& className, const std::string& functionName, F C::*function, C *instance);

    template<class C, typename F>
    void bindClassStaticFunction(const std::string& functionName, const F& function);
    template<typename F>
    void bindClassStaticFunction(const std::string& className, const std::string& functionName, const F& function);

    template<class C, typename F, class FC>
    void bindClassMemberFunction(const std::string& functionName, F FC::*function);
    template<class C, typename F, class FC>
    void bindClassMemberFunction(const std::string& className, const std::string& functionName, F FC::*function);

    template<class C, typename F1, typename F2, class FC>
    void bindClassMemberField(const std::string& fieldName, F1 FC::*getFunction, F2 FC::*setFunction);
    template<class C, typename F1, typename F2, class FC>
    void bindClassMemberField(const std::string& className, const std::string& fieldName, F1 FC::*getFunction, F2 FC::*setFunction);

    template<class C, typename F, class FC>
    void bindClassMemberGetField(const std::string& fieldName, F FC::*getFunction);
    template<class C, typename F, class FC>
    void bindClassMemberGetField(const std::string& className, const std::string& fieldName, F FC::*getFunction);

    template<class C, typename F, class FC>
    void bindClassMemberSetField(const std::string& fieldName, F FC::*setFunction);
    template<class C, typename F, class FC>
    void bindClassMemberSetField(const std::string& className, const std::string& fieldName, F FC::*setFunction);

    template<typename F>
    void bindGlobalFunction(const std::string& functionName, const F& function);

private:
    /// Metamethod that will retrieve fields values (that include functions) from the object when using '.' or ':'
    static int luaObjectGetEvent(LuaInterface* lua);
    /// Metamethod that is called when setting a field of the object by using the keyword '='
    static int luaObjectSetEvent(LuaInterface* lua);
    /// Metamethod that will check equality of objects by using the keyword '=='
    static int luaObjectEqualEvent(LuaInterface* lua);
    /// Metamethod that is called every two lua garbage collections
    /// for any LuaObject that have no references left in lua environment
    /// anymore, thus this creates the possibility of holding an object
    /// existence by lua until it got no references left
    static int luaObjectCollectEvent(LuaInterface* lua);

public:
    /// Loads and runs a script, any errors are printed to stdout and returns false
    bool safeRunScript(const std::string& fileName);

    /// Loads and runs a script
    /// @exception LuaException is thrown on any lua error
    void runScript(const std::string& fileName);

    /// Loads and runs the script from buffer
    /// @exception LuaException is thrown on any lua error
    void runBuffer(const std::string& buffer, const std::string& source);

    /// Loads a script file and pushes it's main function onto stack,
    /// @exception LuaException is thrown on any lua error
    void loadScript(const std::string& fileName);

    /// Loads a function from buffer and pushes it onto stack,
    /// @exception LuaException is thrown on any lua error
    void loadFunction(const std::string& buffer, const std::string& source = "lua function buffer");

    /// Evaluates a lua expression and pushes the result value onto the stack
    /// @exception LuaException is thrown on any lua error
    void evaluateExpression(const std::string& expression, const std::string& source = "lua expression");

    /// Generates a traceback message for the current call stack
    /// @param errorMessage is an additional error message
    /// @param level is the level of the traceback, 0 means trace from calling function
    /// @return the generated traceback message
    std::string traceback(const std::string& errorMessage = "", int level = 0);

    /// Throw a lua error if inside a lua call or generates an C++ stdext::exception
    /// @param message is the error message wich will be displayed before the error traceback
    /// @exception stdext::exception is thrown with the error message if the error is not captured by lua
    void throwError(const std::string& message);

    /// Searches for the source of the current running function
    std::string getCurrentSourcePath(int level = 0);

    /// @brief Calls a function
    /// The function and arguments must be on top of the stack in order,
    /// results are pushed onto the stack.
    /// @exception LuaException is thrown on any lua error
    /// @return number of results
    int safeCall(int numArgs = 0, int numRets = -1);

    /// Same as safeCall but catches exceptions and can also calls a table of functions,
    /// if any error occurs it will be reported to stdout and returns 0 results
    /// @param requestedResults is the number of results requested to pushes onto the stack,
    /// if supplied, the call will always pushes that number of results, even if it fails
    int signalCall(int numArgs = 0, int numRets = -1);

    /// @brief Creates a new environment table
    /// The new environment table is redirected to the global environment (aka _G),
    /// this allows to access global variables from _G in the new environment and
    /// prevents new variables in this new environment to be set on the global environment
    int newSandboxEnv();

    template<typename... T>
    int luaCallGlobalField(const std::string& global, const std::string& field, const T&... args);

    template<typename... T>
    void callGlobalField(const std::string& global, const std::string& field, const T&... args);

    template<typename R, typename... T>
    R callGlobalField(const std::string& global, const std::string& field, const T&... args);

    bool isInCppCallback() { return m_cppCallbackDepth != 0; }

private:
    /// Load scripts requested by lua 'require'
    static int luaScriptLoader(lua_State* L);
    /// Run scripts requested by lua 'dofile'
    static int lua_dofile(lua_State* L);
    /// Run scripts requested by lua 'dofiles'
    static int lua_dofiles(lua_State* L);
    /// Run scripts requested by lua 'dofiles'
    static int lua_loadfile(lua_State* L);
    /// Handle lua errors from safeCall
    static int luaErrorHandler(lua_State* L);
    /// Handle bound cpp functions callbacks
    static int luaCppFunctionCallback(lua_State* L);
    /// Collect bound cpp function pointers
    static int luaCollectCppFunction(lua_State* L);

public:
    void createLuaState();
    void closeLuaState();

    void collectGarbage();

    void loadBuffer(const std::string& buffer, const std::string& source);

    int pcall(int numArgs = 0, int numRets = 0, int errorFuncIndex = 0);
    void call(int numArgs = 0, int numRets = 0);
    void error();

    int ref();
    int weakRef();
    void unref(int ref);
    void useValue() { pushValue(); ref(); }

    const char* typeName(int index = -1);
    std::string functionSourcePath();

    void insert(int index);
    void remove(int index);
    bool next(int index = -2);

    void checkStack() { assert(getTop() <= 20); }
    void getStackFunction(int level = 0);

    void getRef(int ref);
    void getWeakRef(int weakRef);

    int getGlobalEnvironment() { return m_globalEnv; }
    void setGlobalEnvironment(int env);
    void resetGlobalEnvironment() { setGlobalEnvironment(m_globalEnv); }

    void setMetatable(int index = -2);
    void getMetatable(int index = -1);

    void getField(const char* key, int index = -1);
    void getField(const std::string& key, int index = -1) { return getField(key.c_str(), index); }
    void setField(const char* key, int index = -2);
    void setField(const std::string& key, int index = -2) { return setField(key.c_str(), index); }

    void getTable(int index = -2);
    void setTable(int index = -3);
    void clearTable(int index = -1);

    void getEnv(int index = -1);
    void setEnv(int index = -2);

    void getGlobal(const std::string& key);
    void getGlobalField(const std::string& globalKey, const std::string& fieldKey);
    void setGlobal(const std::string& key);

    void rawGet(int index = -1);
    void rawGeti(int n, int index = -1);
    void rawSet(int index = -3);
    void rawSeti(int n, int index = -2);

    void newTable();
    void createTable(int narr, int nrec);
    void* newUserdata(int size);

    void pop(int n = 1);
    long popInteger();
    double popNumber();
    bool popBoolean();
    std::string popString();
    void* popUserdata();
    void* popUpvalueUserdata();
    LuaObjectPtr popObject();

    void pushNil();
    void pushInteger(long v);
    void pushNumber(double v);
    void pushBoolean(bool v);
    void pushCString(const char* v);
    void pushString(const std::string& v);
    void pushLightUserdata(void* p);
    void pushThread();
    void pushValue(int index = -1);
    void pushObject(const LuaObjectPtr& obj);
    void pushCFunction(LuaCFunction func, int n = 0);
    void pushCppFunction(const LuaCppFunction& func);

    bool isNil(int index = -1);
    bool isBoolean(int index = -1);
    bool isNumber(int index = -1);
    bool isString(int index = -1);
    bool isTable(int index = -1);
    bool isFunction(int index = -1);
    bool isCFunction(int index = -1);
    bool isLuaFunction(int index = -1)  { return (isFunction(index) && !isCFunction(index)); }
    bool isUserdata(int index = -1);

    bool toBoolean(int index = -1);
    int toInteger(int index = -1);
    double toNumber(int index = -1);
    const char* toCString(int index = -1);
    std::string toString(int index = -1);
    void* toUserdata(int index = -1);
    LuaObjectPtr toObject(int index = -1);

    int getTop();
    int stackSize() { return getTop(); }
    void clearStack() { pop(stackSize()); }
    bool hasIndex(int index) { return (stackSize() >= (index < 0 ? -index : index) && index != 0); }

    void loadFiles(std::string directory, bool recursive = false, std::string contains = "");

    /// Pushes any type onto the stack
    template<typename T, typename... Args>
    int polymorphicPush(const T& v, const Args&... args);
    int polymorphicPush() { return 0; }

    /// Casts a value from stack to any type
    /// @exception LuaBadValueCastException thrown if the cast fails
    template<class T>
    T castValue(int index = -1);

    /// Same as castValue but also pops
    template<class T>
    T polymorphicPop() { T v = castValue<T>(); pop(1); return v; }

private:
    lua_State* L;
    int m_weakTableRef;
    int m_cppCallbackDepth;
    int m_totalObjRefs;
    int m_totalFuncRefs;
    int m_globalEnv;
};

extern LuaInterface g_lua;

// must be included after, because they need LuaInterface fully declared
#include "luaexception.h"
#include "luabinder.h"
#include "luavaluecasts.h"

template<typename T, typename... Args>
int LuaInterface::polymorphicPush(const T& v, const Args&... args) {
    int r = push_luavalue(v);
    return r + polymorphicPush(args...);
}

// next templates must be defined after above includes

template<class C, typename F>
void LuaInterface::bindSingletonFunction(const std::string& functionName, F C::*function, C *instance) {
    registerClassStaticFunction<C>(functionName, luabinder::bind_singleton_mem_fun(function, instance));
}

template<class C, typename F>
void LuaInterface::bindSingletonFunction(const std::string& className, const std::string& functionName, F C::*function, C *instance) {
    registerClassStaticFunction(className, functionName, luabinder::bind_singleton_mem_fun(function, instance));
}

template<class C, typename F>
void LuaInterface::bindClassStaticFunction(const std::string& functionName, const F& function) {
    registerClassStaticFunction<C>(functionName, luabinder::bind_fun(function));
}
template<typename F>
void LuaInterface::bindClassStaticFunction(const std::string& className, const std::string& functionName, const F& function) {
    registerClassStaticFunction(className, functionName, luabinder::bind_fun(function));
}

template<class C, typename F, class FC>
void LuaInterface::bindClassMemberFunction(const std::string& functionName, F FC::*function) {
    registerClassMemberFunction<C>(functionName, luabinder::bind_mem_fun<C>(function));
}
template<class C, typename F, class FC>
void LuaInterface::bindClassMemberFunction(const std::string& className, const std::string& functionName, F FC::*function) {
    registerClassMemberFunction(className, functionName, luabinder::bind_mem_fun<C>(function));
}

template<class C, typename F1, typename F2, class FC>
void LuaInterface::bindClassMemberField(const std::string& fieldName, F1 FC::*getFunction, F2 FC::*setFunction) {
    registerClassMemberField<C>(fieldName, luabinder::bind_mem_fun<C>(getFunction), luabinder::bind_mem_fun<C>(setFunction));
}
template<class C, typename F1, typename F2, class FC>
void LuaInterface::bindClassMemberField(const std::string& className, const std::string& fieldName, F1 FC::*getFunction, F2 FC::*setFunction) {
    registerClassMemberField(className, fieldName, luabinder::bind_mem_fun<C>(getFunction), luabinder::bind_mem_fun<C>(setFunction));
}

template<class C, typename F, class FC>
void LuaInterface::bindClassMemberGetField(const std::string& fieldName, F FC::*getFunction) {
    registerClassMemberField<C>(fieldName, luabinder::bind_mem_fun<C>(getFunction), LuaCppFunction());
}
template<class C, typename F, class FC>
void LuaInterface::bindClassMemberGetField(const std::string& className, const std::string& fieldName, F FC::*getFunction) {
    registerClassMemberField(className, fieldName, luabinder::bind_mem_fun<C>(getFunction), LuaCppFunction());
}

template<class C, typename F, class FC>
void LuaInterface::bindClassMemberSetField(const std::string& fieldName, F FC::*setFunction) {
    registerClassMemberField<C>(fieldName, LuaCppFunction(), luabinder::bind_mem_fun<C>(setFunction));
}
template<class C, typename F, class FC>
void LuaInterface::bindClassMemberSetField(const std::string& className, const std::string& fieldName, F FC::*setFunction) {
    registerClassMemberField(className, fieldName, LuaCppFunction(), luabinder::bind_mem_fun<C>(setFunction));
}

template<typename F>
void LuaInterface::bindGlobalFunction(const std::string& functionName, const F& function) {
    registerGlobalFunction(functionName, luabinder::bind_fun(function));
}

template<class T>
T LuaInterface::castValue(int index) {
    T o;
    if(!luavalue_cast(index, o))
        throw LuaBadValueCastException(typeName(index), stdext::demangle_type<T>());
    return o;
}

template<typename... T>
int LuaInterface::luaCallGlobalField(const std::string& global, const std::string& field, const T&... args) {
    g_lua.getGlobalField(global, field);
    if(!g_lua.isNil()) {
        int numArgs = g_lua.polymorphicPush(args...);
        return g_lua.signalCall(numArgs);
    } else
        g_lua.pop(1);
    return 0;
}

template<typename... T>
void LuaInterface::callGlobalField(const std::string& global, const std::string& field, const T&... args) {
    int rets = luaCallGlobalField(global, field, args...);
    if(rets > 0)
        pop(rets);
}

template<typename R, typename... T>
R LuaInterface::callGlobalField(const std::string& global, const std::string& field, const T&... args) {
    R result;
    int rets = luaCallGlobalField(global, field, args...);
    if(rets > 0) {
        assert(rets == 1);
        result = g_lua.polymorphicPop<R>();
    } else
        result = R();
    return result;
}

#endif
