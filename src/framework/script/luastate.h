#ifndef LUASTATE_H
#define LUASTATE_H

#include "luadeclarations.h"

struct lua_State;

class LuaState
{
public:
    LuaState();
    virtual ~LuaState();

    // high level functions
    void collectGarbage();

    LuaValuePtr loadBuffer(const std::string& buffer, const std::string& source);
    LuaValuePtr loadScript(const std::string& fileName);
    void runBuffer(const std::string& buffer, const std::string& source);
    void runScript(const std::string& fileName);

    LuaValuePtr getGlobal(const std::string& name) { getGlobalField(name); return popValue(); }
    void setGlobal(const std::string& name, const LuaValuePtr& value) { pushValue(value); setGlobalField(name); }

    LuaValuePtr createTable() { newTable(); return popValue(); }
    LuaValuePtr createTable(const std::string& name) { newTable(); copy(); setGlobalField(name); return popValue(); }
    LuaValuePtr createCppFunction(const LuaCppFunction& v) { pushCppFunction(v); return popValue(); }
    LuaValuePtr createNil() { pushNil(); return popValue(); }
    LuaValuePtr createNumber(double v) { pushNumber(v); return popValue(); }
    LuaValuePtr createInteger(int v) { pushInteger(v); return popValue(); }
    LuaValuePtr createString(const std::string& v) { pushString(v); return popValue(); }
    LuaValuePtr createBoolean(bool v) { pushBoolean(v); return popValue(); }

    std::string getTraceback(const std::string& errorMessage = "", int depth = 0);
    std::string getCurrentSource();
    std::string getFunctionSource(LuaValuePtr func);
    LuaValuePtr getStackFunction(int level);

    LuaValuePtr getGlobalEnvironment();
    void setGlobalEnvironment(const LuaValuePtr& env);

    // low level lua functions
    std::string typeName(int index = -1);

    int ref();
    void unref(int ref);

    void call(int numArgs = 0, int numRets = 0);
    int safeCall(const std::string& functionName, int numArgs = 0);

    void insert(int index);
    void remove(int index);
    bool next(int index = -2);
    void copy(int index = -1);

    void setMetatable(int index = -2);
    void setMetatable(const std::string& name);
    void getMetatable(int index = -1);

    void getField(const std::string& key, int index = -1);
    void setField(const std::string& key, int index = -2);

    void getTable(int index = -2);
    void setTable(int index = -3);

    void getEnv(int index = -1);
    void setEnv(int index = -2);

    void getGlobalField(const std::string& key);
    void setGlobalField(const std::string& key);

    void rawGet(int index = -1);
    void rawGeti(int n, int index = -1);
    void rawSet(int index = -3);
    void rawSeti(int n, int index = -2);

    void newTable();
    void* newUserdata(int size);

    void pop(int n = 1);
    int popInteger();
    double popNumber();
    bool popBoolean();
    std::string popString();
    void* popUserdata();
    LuaValuePtr popValue();
    LuaValueList popValues();

    void pushNil();
    void pushInteger(int v);
    void pushNumber(double v);
    void pushBoolean(bool v);
    void pushString(const std::string& v);
    void pushRef(int ref);
    void pushLightUserdata(void* p);
    void pushCppFunction(const LuaCppFunction& func);
    void pushValue(const LuaValuePtr& value);
    void pushValues(const LuaValueList& values);

    bool isNil(int index = -1);
    bool isBoolean(int index = -1);
    bool isNumber(int index = -1);
    bool isString(int index = -1);
    bool isTable(int index = -1);
    bool isFunction(int index = -1);
    bool isCFunction(int index = -1);
    bool isLuaFunction(int index = -1)  { return (isFunction() && !isCFunction()); }
    bool isUserdata(int index = -1);

    bool toBoolean(int index = -1);
    int toInteger(int index = -1);
    double toNumber(int index = -1);
    std::string toString(int index = -1);
    void* toUserdata(int index = -1);

    int stackSize();
    void clearStack() { pop(stackSize()); }
    bool hasIndex(int index) { return (stackSize() >= (index < 0 ? -index : index)); }

private:
    lua_State* L;
    bool m_collecting;
    std::deque<std::string> m_callStack;
};

#endif // LUASTATE_H
