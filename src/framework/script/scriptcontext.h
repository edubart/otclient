#ifndef SCRIPTCONTEXT_H
#define SCRIPTCONTEXT_H

#include <global.h>
#include "scriptobject.h"

#define reportFuncError(a) reportError(a, __FUNCTION__)
#define reportFuncErrorWithTraceback(a) reportErrorWithTraceback(a, __FUNCTION__)

struct lua_State;

class ScriptContext
{
public:
    ScriptContext() : L(NULL) { }

    void init();
    void terminate();

    void loadAllModules();
    bool loadFile(const std::string& fileName);
    bool loadBuffer(const std::string& text, const std::string& what = "luaBuffer");
    bool loadBufferAsFunction(const std::string& text, const std::string& what = "luaBuffer");
    void reportError(const std::string& errorDesc, const char *funcName = NULL);
    void reportErrorWithTraceback(const std::string& errorDesc, const char *funcName = NULL);

    void collectGarbage();

    int getStackSize();
    void insert(int index);
    void swap(int index);
    void remove(int index);
    bool next(int index = -2);
    void releaseRef(int ref);

    void newTable();
    void setTable(int index = -3);

    void *newUserdata(int size);

    void newMetatable(const std::string& name);
    void setMetatable(const std::string& name, int index = -1);

    void rawGet(const std::string& key);
    void rawSet(const std::string& key);
    void getField(const std::string& key);
    void setField(const std::string& key);

    void getScriptObjectField(const ScriptObjectPtr& scriptobject, const std::string& field);
    void setScriptObjectField(const ScriptObjectPtr& scriptobject, const std::string& field);

    void rawGetGlobalTableField(const std::string& globalTable, const std::string& key);
    void rawSetGlobalTableField(const std::string& globalTable, const std::string& key);

    void getGlobal(const std::string& key);
    void setGlobal(const std::string& key);

    bool isNil(int index = -1);
    bool isBoolean(int index = -1);
    bool isNumber(int index = -1);
    bool isString(int index = -1);
    bool isTable(int index = -1);
    bool isUserdata(int index = -1);
    bool isFunction(int index = -1);
    bool isCFunction(int index = -1);
    bool isLuaFunction(int index = -1);

    void pop(int n = 1);
    bool popBoolean();
    int popInteger();
    std::string popString();
    ScriptObjectPtr popClassInstance();
    int popRef();

    void pushNil();
    void pushBoolean(bool b);
    void pushInteger(int i);
    void pushString(const std::string& str);
    void pushClassInstance(const ScriptObjectPtr& object);
    void pushValue(int index = -1);
    void pushRef(int ref);

    std::string getFunctionSourcePath(bool functionIsOnStack, int level = 1);

    bool callFunction(int numArgs = 0, int numRets = 0);
    void callModuleField(const std::string& module, const std::string& field);

    typedef int (*LuaCFunction)();

    void setupPackageLoader();
    void registerClass(const std::string& klass, const std::string& baseClass = "");
    void registerMemberField(const std::string& field, LuaCFunction getFunction, LuaCFunction setFunction = NULL);
    void registerMemberFunction(const std::string& functionName, LuaCFunction function);
    void registerGlobalFunction(const std::string& functionName, LuaCFunction function);
    void registerModule(const std::string& module);

    static int luaPackageLoader(lua_State* L);

    static int luaIndexMetaMethod(lua_State* L);
    static int luaNewIndexMetaMethod(lua_State* L);
    static int luaEqualMetaMethod(lua_State* L);
    static int luaGarbageCollectMetaMethod(lua_State* L);

    static int luaFunctionCallback(lua_State* L);
    static int luaErrorHandler(lua_State *L);

private:
    std::vector<LuaCFunction> m_functions;
    std::string m_currentClass;
    lua_State *L;
};

extern ScriptContext g_lua;

#endif // SCRIPTCONTEXT_H
