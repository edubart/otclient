#ifndef LUAINTERFACE_H
#define LUAINTERFACE_H

#include "luabinder.h"
#include "luaobject.h"

class LuaInterface : public LuaState
{
public:
    void init();
    void terminate();

    /// Load a function from buffef into a LuaValue
    LuaValuePtr loadFunction(const std::string& buffer, const std::string& source = "lua function buffer");

    /// Evaluate a lua expression to a LuaValue
    LuaValuePtr evaluateExpression(const std::string& expression, const std::string& source = "lua expression");

    LuaValuePtr createEnvironment();

    /// Register core script functions
    void registerFunctions();

    // methods for registring classes and functions
    void registerClass(const std::string& className, const std::string& baseClass = "LuaObject");
    void registerClassStaticFunction(const std::string& className, const std::string& functionName, const LuaCppFunction& function);
    void registerClassMemberFunction(const std::string& className, const std::string& functionName, const LuaCppFunction& function);
    void registerClassMemberField(const std::string& className, const std::string& field, const LuaCppFunction& getFunction, const LuaCppFunction& setFunction);
    void registerGlobalFunction(const std::string& functionName, const LuaCppFunction& function);

    // methods for binding functions
    template<typename F>
    void bindClassStaticFunction(const std::string& className, const std::string& functionName, const F& function) {
        registerClassStaticFunction(className, functionName, luabinder::bind_fun(function));
    }

    template<class C, typename F>
    void bindClassMemberFunction(const std::string& className, const std::string& functionName, F C::*function) {
        registerClassMemberFunction(className, functionName, luabinder::bind_mem_fun(function));
    }

    template<class C, typename F1, typename F2>
    void bindClassMemberField(const std::string& className, const std::string& fieldName, F1 C::*getFunction, F2 C::*setFunction) {
        registerClassMemberField(className, fieldName, luabinder::bind_mem_fun(getFunction), luabinder::bind_mem_fun(setFunction));
    }

    template<class C, typename F>
    void bindClassMemberGetField(const std::string& className, const std::string& fieldName, F C::*getFunction) {
        registerClassMemberField(className, fieldName, luabinder::bind_mem_fun(getFunction), LuaCppFunction());
    }

    template<class C, typename F>
    void bindClassMemberSetField(const std::string& className, const std::string& fieldName, F C::*setFunction) {
        registerClassMemberField(className, fieldName, LuaCppFunction(), luabinder::bind_mem_fun(setFunction));
    }

    template<typename F>
    void bindGlobalFunction(const std::string& functionName, const F& function) {
        registerGlobalFunction(functionName, luabinder::bind_fun(function));
    }

private:
    // installed lua callbacks by this interface
    static int luaScriptLoader(LuaState* lua);
    static int luaObjectGetEvent(LuaState* lua);
    static int luaObjectSetEvent(LuaState* lua);
    static int luaObjectEqualEvent(LuaState* lua);
    static int luaObjectCollectEvent(LuaState* lua);
};

extern LuaInterface g_lua;

#endif // LUAINTERFACE_H
