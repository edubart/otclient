#include "luastate.h"
#include "luavalue.h"

#include <lua.hpp>
#include <core/resources.h>

/// Retrive LuaState from L
LuaState* retriveLuaState(lua_State* L)
{
    lua_getfield(L, LUA_REGISTRYINDEX, "LuaStateInstance");
    auto lua = static_cast<LuaState*>(const_cast<void*>(lua_topointer(L, -1)));
    lua_pop(L, 1);
    return lua;
}

/// Handle errors by protected lua calls (pcall)
int luaErrorHandler(lua_State* L)
{
    // pops the error message
    auto lua = retriveLuaState(L);
    auto error = lua->popString();

    // prevents repeated tracebacks
    if(error.find("stack traceback:") != std::string::npos)
        error = lua->getTraceback(error, 1);

    // pushes the new error message with traceback information
    lua->pushString(error);
    return 1;
}

/// Handle bound cpp functions callbacks
int luaCppFunctionCallback(lua_State* L)
{
    auto lua = retriveLuaState(L);
    auto funcPtr = static_cast<LuaCppFunctionPtr*>(lua_touserdata(L, lua_upvalueindex(1)));
    assert(funcPtr);

    try {
        return (*(funcPtr->get()))(lua);
    } catch(LuaException &e) {
        logError(e.what());
        return 0;
    }
}

/// Free cpp function pointer when it expires
int luaCppFunctionCallbackPointerExpire(lua_State* L)
{
    auto funcPtr = static_cast<LuaCppFunctionPtr*>(lua_touserdata(L, -1));
    assert(funcPtr);
    funcPtr->reset();
    return 0;
}

LuaState::LuaState() : m_collecting(false)
{
    // create lua state
    L = luaL_newstate();
    if(!L)
        throw LuaException("failed to create lua state");

    // load lua standard libraries
    luaL_openlibs(L);

    // store this into lua
    pushLightUserdata(this);
    lua_setfield(L, LUA_REGISTRYINDEX, "LuaStateInstance");
}

LuaState::~LuaState()
{
    // close lua state
    m_collecting = true;
    lua_close(L);
}

void LuaState::collectGarbage()
{
    // prevents recursive collects
    if(!m_collecting) {
        m_collecting = true;

        // we must collect two times for __gc metamethod be called on uservalues
        for(int i=0;i<2;++i)
            lua_gc(L, LUA_GCCOLLECT, 0);

        m_collecting = false;
    }
}

LuaValuePtr LuaState::loadBuffer(const std::string& buffer, const std::string& source)
{
    // load buffer from lua, the "@" means that the source is a file path, thus lua will
    // print file paths in tracebacks
    int ret = luaL_loadbuffer(L, buffer.c_str(), buffer.length(), ("@" + source).c_str());
    if(ret != 0)
        throw LuaException(popString(), 0);
    return popValue();
}

LuaValuePtr LuaState::loadScript(const std::string &fileName)
{
    std::stringstream fin;
    if(!g_resources.loadFile(fileName, fin))
        throw LuaException(make_string("failed to open lua script ", g_resources.resolvePath(fileName)), 0);
    return loadBuffer(fin.str(), g_resources.resolvePath(fileName));
}

void LuaState::runBuffer(const std::string& buffer, const std::string& source)
{
    auto main = loadBuffer(buffer, source);
    main->call(source);
}

void LuaState::runScript(const std::string& fileName)
{
    auto main = loadScript(fileName);
    main->call(fileName);
}

int LuaState::safeCall(const std::string& functionName, int numArgs)
{
    assert(hasIndex(-numArgs-1));

    int previousStackSize = stackSize();

    // push error function
    int errorFuncIndex = previousStackSize - numArgs;
    lua_pushcfunction(L, luaErrorHandler);
    insert(errorFuncIndex);

    m_callStack.push_front(functionName);

    // call the func
    int ret = lua_pcall(L, numArgs, LUA_MULTRET, errorFuncIndex);

    m_callStack.pop_front();

    // pop error func
    remove(errorFuncIndex);

    if(ret != 0)
        throw LuaException(popString(), 0);

    // return the number of results
    return (stackSize() + numArgs + 1) - previousStackSize;
}

LuaValuePtr LuaState::popValue()
{
    assert(hasIndex(-1));

    LuaValuePtr value;

    int refId = ref();
    if(refId < 0) {
        // refs < 0 are considered as nil values
        value = LuaValuePtr(new LuaValue());
    } else {
        pushRef(refId);
        value = LuaValuePtr(new LuaValue(refId));
        pop();
    }
    return value;
}

LuaValueList LuaState::popValues()
{
    int numValues = stackSize();
    LuaValueList values(numValues);
    for(int i=numValues-1;i>=0;--i)
       values[i] = popValue();
    return values;
}

void LuaState::pushCppFunction(const LuaCppFunction& func)
{
    // create a pointer to func (this pointer will hold the function existence)
    new(newUserdata(sizeof(LuaCppFunctionPtr))) LuaCppFunctionPtr(new LuaCppFunction(func));

    // set the userdata __gc metamethod
    newTable();
    lua_pushcfunction(L, luaCppFunctionCallbackPointerExpire);
    setField("__gc");
    setMetatable();

    // pushes the func
    lua_pushcclosure(L, luaCppFunctionCallback, 1);
}

void LuaState::pushValue(const LuaValuePtr& value)
{
    if(value->isNil())
        pushNil();
    else
        pushRef(value->ref);
}

void LuaState::pushValues(const LuaValueList& values)
{
    foreach(const LuaValuePtr& value, values)
        pushValue(value);
}

std::string LuaState::getTraceback(const std::string& errorMessage, int depth)
{
    // get traceback from debug.traceback
    getGlobalField("debug");
    getField("traceback");
    remove(-2);
    pushString(errorMessage);
    pushInteger(depth);
    call(2,1);
    std::string luaTraceback = popString();
    boost::replace_first(luaTraceback, "stack traceback:", "lua stack traceback:");

    std::stringstream ss;
    ss << luaTraceback;

    // add application call traceback
    if(m_callStack.size() > 0) {
        ss << "\napplication call stack traceback:";
        foreach(const std::string& func, m_callStack)
            ss << "\n\t" << func;
    }

    return ss.str();
}

std::string LuaState::getCurrentSource()
{
    std::string path;
    int level = 0;

    // get the current running script path
    LuaValuePtr func;
    do {
        func = getStackFunction(level);
        if(func->isLuaFunction()) {
            path = getFunctionSource(func);
            break;
        }
        level++;
    } while(!func->isNil());

    return path;
}

std::string LuaState::getFunctionSource(LuaValuePtr func)
{
    std::string path;

    pushValue(func);

    lua_Debug ar;
    lua_getinfo(L, ">Sn", &ar);
    if(ar.source) {
        path = ar.source;
        path = path.substr(1, std::string::npos);
    }

    return path;
}

LuaValuePtr LuaState::getGlobalEnvironment()
{
    lua_pushthread(L);
    getEnv();
    remove(-2);
    return popValue();
}

void LuaState::setGlobalEnvironment(const LuaValuePtr& env)
{
    lua_pushthread(L);
    pushValue(env);
    setEnv();
    pop();
}

LuaValuePtr LuaState::getStackFunction(int level)
{
    // get current stack func
    lua_Debug ar;
    if(lua_getstack(L, level, &ar) == 1) {
        lua_getinfo(L, "f", &ar);
        return popValue();
    }
    return createNil();
}

std::string LuaState::typeName(int index)
{
    assert(hasIndex(index));
    int type = lua_type(L, index);
    std::string name = lua_typename(L, type);
    return name;
}

int LuaState::ref()
{
    return luaL_ref(L, LUA_REGISTRYINDEX);
}

void LuaState::unref(int ref)
{
    if(ref > 0)
        luaL_unref(L, LUA_REGISTRYINDEX, ref);
}

void LuaState::call(int numArgs, int numRets)
{
    assert(hasIndex(-numArgs - 1));
    lua_call(L, numArgs, numRets);
}

void LuaState::insert(int index)
{
    assert(hasIndex(index));
    lua_insert(L, index);
}

void LuaState::remove(int index)
{
    assert(hasIndex(index));
    lua_remove(L, index);
}

bool LuaState::next(int index)
{
    assert(hasIndex(index));
    return lua_next(L, index);
}

void LuaState::copy(int index)
{
    assert(hasIndex(index));
    lua_pushvalue(L, index);
}

void LuaState::setMetatable(int index)
{
    assert(hasIndex(index));
    lua_setmetatable(L, index);
}

void LuaState::setMetatable(const std::string& name)
{
    assert(hasIndex(-1));
    getGlobalField(name);
    setMetatable();
}

void LuaState::getMetatable(int index)
{
    assert(hasIndex(index));
    lua_getmetatable(L, index);
}

void LuaState::getField(const std::string& key, int index)
{
    assert(hasIndex(index));
    lua_getfield(L, index, key.c_str());
}

void LuaState::setField(const std::string& key, int index)
{
    assert(hasIndex(index));
    lua_setfield(L, index, key.c_str());
}

void LuaState::getEnv(int index)
{
    assert(hasIndex(index));
    lua_getfenv(L, index);
}

void LuaState::setEnv(int index)
{
    assert(hasIndex(index));
    assert(lua_setfenv(L, index) == 1);
}

void LuaState::getTable(int index)
{
    assert(hasIndex(index));
    lua_gettable(L, index);
}

void LuaState::setTable(int index)
{
    assert(hasIndex(index));
    lua_settable(L, index);
}

void LuaState::getGlobalField(const std::string& key)
{
    lua_getglobal(L, key.c_str());
}

void LuaState::setGlobalField(const std::string& key)
{
    assert(hasIndex(-1));
    lua_setglobal(L, key.c_str());
}

void LuaState::rawGet(int index)
{
    assert(hasIndex(index));
    lua_rawget(L, index);
}

void LuaState::rawGeti(int n, int index)
{
    assert(hasIndex(index));
    lua_rawgeti(L, index, n);
}

void LuaState::rawSet(int index)
{
    assert(hasIndex(index));
    lua_rawset(L, index);
}

void LuaState::rawSeti(int n, int index)
{
    assert(hasIndex(index));
    lua_rawseti(L, index, n);
}

void LuaState::newTable()
{
    lua_newtable(L);
}

void* LuaState::newUserdata(int size)
{
    return lua_newuserdata(L, size);
}

void LuaState::pop(int n)
{
    assert(hasIndex(-n));
    lua_pop(L, n);
}

int LuaState::popInteger()
{
    assert(hasIndex(-1));
    int v = toInteger(-1);
    pop();
    return v;
}

double LuaState::popNumber()
{
    assert(hasIndex(-1));
    double v = toNumber(-1);
    pop();
    return v;
}

bool LuaState::popBoolean()
{
    assert(hasIndex(-1));
    bool v = toBoolean(-1);
    pop();
    return v;
}

std::string LuaState::popString()
{
    assert(hasIndex(-1));
    std::string v = toString(-1);
    pop();
    return v;
}

void* LuaState::popUserdata()
{
    assert(hasIndex(-1));
    void* v = toUserdata(-1);
    pop();
    return v;
}

void LuaState::pushNil()
{
    lua_pushnil(L);
}

void LuaState::pushInteger(int v)
{
    lua_pushinteger(L, v);
}

void LuaState::pushNumber(double v)
{
    lua_pushnumber(L, v);
}

void LuaState::pushBoolean(bool v)
{
    lua_pushboolean(L, v);
}

void LuaState::pushString(const std::string& v)
{
    lua_pushstring(L, v.c_str());
}

void LuaState::pushRef(int ref)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
}

void LuaState::pushLightUserdata(void* p)
{
    lua_pushlightuserdata(L, p);
}

bool LuaState::isNil(int index)
{
    assert(hasIndex(index));
    return lua_isnil(L, index);
}

bool LuaState::isBoolean(int index)
{
    assert(hasIndex(index));
    return lua_isboolean(L, index);
}

bool LuaState::isNumber(int index)
{
    assert(hasIndex(index));
    return lua_isnumber(L, index);
}

bool LuaState::isString(int index)
{
    assert(hasIndex(index));
    return lua_isstring(L, index);
}

bool LuaState::isTable(int index)
{
    assert(hasIndex(index));
    return lua_istable(L, index);
}

bool LuaState::isFunction(int index)
{
    assert(hasIndex(index));
    return lua_isfunction(L, index);
}

bool LuaState::isCFunction(int index)
{
    assert(hasIndex(index));
    return lua_iscfunction(L, index);
}

bool LuaState::isUserdata(int index)
{
    assert(hasIndex(index));
    return lua_isuserdata(L, index);
}

bool LuaState::toBoolean(int index)
{
    assert(hasIndex(index));
    return (bool)lua_toboolean(L, index);
}

int LuaState::toInteger(int index)
{
    assert(hasIndex(index));
    return lua_tointeger(L, index);
}

double LuaState::toNumber(int index)
{
    assert(hasIndex(index));
    return lua_tonumber(L, index);
}

std::string LuaState::toString(int index)
{
    assert(hasIndex(index));
    return std::string(isString() ? lua_tostring(L, index) : "");
}

void* LuaState::toUserdata(int index)
{
    assert(hasIndex(index));
    return lua_touserdata(L, index);
}

int LuaState::stackSize()
{
    return lua_gettop(L);
}
