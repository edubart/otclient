/* The MIT License
 *
 * Copyright (c) 2010 OTClient, https://github.com/edubart/otclient
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


#include <prerequisites.h>
#include <script/luascript.h>
#include <core/resources.h>

LuaScript g_lua;

LuaScript::LuaScript()
{
    L = luaL_newstate();
    if(!L)
        logFatal("could not create lua context");

    // load lua standard libraries
    luaL_openlibs(L);

    // setup custom package loader
    setupPackageLoader();

    // register classes and functions
    registerFunctions();
}

LuaScript::~LuaScript()
{
    lua_close(L);
}

void LuaScript::loadAllModules()
{
    std::list<std::string> modules = g_resources.getDirectoryFiles("modules");
    foreach(const std::string& module, modules) {
        std::list<std::string> moduleFiles = g_resources.getDirectoryFiles(std::string("modules/") + module);
        foreach(const std::string& moduleFile, moduleFiles) {
            if(boost::ends_with(moduleFile, ".lua"))
                loadFile(std::string("modules/") + module + "/" + moduleFile);
        }
    }
}

bool LuaScript::loadFile(const std::string& fileName)
{
    if(!g_resources.fileExists(fileName)) {
        logError("script file '%s' doesn't exist", fileName.c_str());
        return false;
    }
    std::string fileContents = g_resources.loadTextFile(fileName);
    return loadBuffer(fileContents, fileName);
}

bool LuaScript::loadBuffer(const std::string& text, const std::string& what)
{
    // load buffer
    int ret = luaL_loadbuffer(L, text.c_str(), text.length(), what.c_str());
    if(ret != 0){
        reportError(popString());
        return false;
    }

    // check if is loaded as a function
    if(lua_isfunction(L, -1) == 0) {
        return false;
    }

    // execute it
    ret = lua_pcall(L, 0, 0, 0);
    if(ret != 0){
        reportError(popString());
        return false;
    }

    return true;
}

int LuaScript::loadBufferAsFunction(const std::string& text, const std::string& what)
{
    int ret = luaL_loadbuffer(L, text.c_str(), text.length(), what.c_str());
    if(ret != 0){
        reportError(popString());
        return LUA_REFNIL;
    }

    // check if is loaded as a function
    if(lua_isfunction(L, -1) == 0) {
        return LUA_REFNIL;
    }

    return popFunction();
}

void LuaScript::reportError(const std::string& errorDesc, const char *funcName)
{
    std::stringstream ss;
    ss << "LUA script error";
    if(funcName)
        ss << " in " << funcName << "()";
    ss << ": " << errorDesc << std::endl;
    logError(ss.str().c_str());
}

int LuaScript::getStackSize()
{
    return lua_gettop(L);
}

void LuaScript::pop(int n)
{
    lua_pop(L, n);
}

bool LuaScript::popBoolean()
{
    bool b = (lua_toboolean(L, -1) != 0);
    pop();
    return b;
}

int32_t LuaScript::popInteger()
{
    double d = lua_tonumber(L, -1);
    pop(1);
    return (int)d;
}

std::string LuaScript::popString()
{
    size_t len;
    const char *cstr = lua_tolstring(L, -1, &len);
    std::string str(cstr, len);
    pop();
    return str;
}

void LuaScript::pushNil()
{
    lua_pushnil(L);
}

void LuaScript::pushBoolean(bool b)
{
    lua_pushboolean(L, b);
}

void LuaScript::pushInteger(int32_t i)
{
    lua_pushnumber(L, i);
}

void LuaScript::pushString(const std::string& str)
{
    lua_pushstring(L, str.c_str());
}

void LuaScript::pushUserdata(void* ptr)
{
    lua_pushlightuserdata(L, ptr);
}

void LuaScript::pushClassInstance(const ScriptablePtr& object)
{
    if(object && object->getScriptableName()) {
        new(lua_newuserdata(L, sizeof(ScriptableWeakPtr))) ScriptableWeakPtr(object);
        lua_getfield(L, LUA_REGISTRYINDEX, (std::string(object->getScriptableName()) + "_mt").c_str());
        lua_setmetatable(L, -2);
    } else {
        lua_pushnil(L);
    }
}

ScriptablePtr LuaScript::popClassInstance()
{
    if(!lua_isuserdata(L, -1)) {
        reportError(format("couldn't pop a class instance, top objet is not a valid type (%s)", luaL_typename(L, -1)));
        lua_pop(L, 1);
        return ScriptablePtr();
    }

    ScriptableWeakPtr *objectRef = (ScriptableWeakPtr *)lua_touserdata(L, -1);
    lua_pop(L, 1);

    ScriptablePtr object = objectRef->lock();
    if(!object)
        reportError(format("attempt to retrive class instance from a object that is already expired"));
    return object;
}

void LuaScript::pushFunction(int functionRef)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, functionRef);
}

int LuaScript::popFunction()
{
    return luaL_ref(L, LUA_REGISTRYINDEX);
}

void LuaScript::releaseFunction(int functionRef)
{
    luaL_unref(L, LUA_REGISTRYINDEX, functionRef);
}

void LuaScript::callFunction(int numArgs)
{
    int size = lua_gettop(L);
    int errorIndex = size - numArgs;
    lua_pushcfunction(L, &LuaScript::luaErrorHandler);
    lua_insert(L, errorIndex);

    int ret = lua_pcall(L, numArgs, 0, errorIndex);
    if(ret != 0) {
        reportError(popString());
    }

    lua_remove(L, errorIndex);

    if(lua_gettop(L) + numArgs + 1 != size)
        reportError("stack size changed!");
}

void LuaScript::setSelf(const ScriptablePtr& scriptable, int envIndex)
{
    lua_getfenv(L, envIndex);
    pushClassInstance(scriptable);
    lua_setfield(L, -2, "self");
    lua_pop(L, 1);
}

void LuaScript::setupPackageLoader()
{
    lua_getfield(L, LUA_GLOBALSINDEX, "package");
    lua_getfield(L, -1, "loaders");
    lua_remove(L, -2);
    lua_pushnil(L);
    int numLoaders = 0;
    while(lua_next(L, -2) != 0) {
        lua_pop(L, 1);
        numLoaders++;
    }
    lua_pushinteger(L, numLoaders + 1);
    lua_pushcfunction(L, &LuaScript::luaPackageLoader);
    lua_rawset(L, -3);
    lua_pop(L, 1);
}

void LuaScript::registerClass(const std::string& klass, const std::string& baseClass)
{
    // klass_mt = {}
    lua_newtable(L); // klass metatable
    lua_pushvalue(L, -1); // another reference to the metatable
    lua_setfield(L, LUA_REGISTRYINDEX, (klass + "_mt").c_str()); // register the metatable at registry index

    // set __gc metamethod, which collects userdata
    lua_pushcfunction(L, &LuaScript::luaCollectClassInstance);
    lua_setfield(L, -2, "__gc");

    // set __eq metamethod
    lua_pushcfunction(L, &LuaScript::luaCompareClassInstances);
    lua_setfield(L, -2, "__eq");

    // klass = {}
    lua_newtable(L); // klass table
    lua_pushvalue(L, -1); // another reference to the table
    lua_setfield(L, LUA_GLOBALSINDEX, klass.c_str()); // register at globals index

    // klass.className = "klass"
    lua_pushstring(L, klass.c_str());
    lua_setfield(L, -2, "className");

    // klass_mt.__index = klass
    lua_setfield(L, -2, "__index");

    // pop the class metatable
    lua_pop(L, 1);

    if(baseClass.length() > 0) {
        // add klass table to the top of the stack
        lua_getfield(L, LUA_GLOBALSINDEX, klass.c_str());

        // redirect = { __index = baseClass }
        lua_newtable(L);
        lua_getfield(L, LUA_GLOBALSINDEX, baseClass.c_str());
        lua_setfield(L, -2, "__index");

        // setmetatable(klass, redirect)
        lua_setmetatable(L, -2);

        // pop the derived class table
        lua_pop(L, 1);
    }
}

void LuaScript::registerMemberFunction(const std::string& klass, const std::string& functionName, LuaCFunction function)
{
    int functionId = m_functions.size();
    m_functions.push_back(function);

    // push the class table
    lua_getfield(L, LUA_GLOBALSINDEX, klass.c_str());
    // push the function id
    lua_pushnumber(L, functionId);
    // store id in the closure
    lua_pushcclosure(L, &LuaScript::luaFunctionCallback, 1);
    // store the function at the class field functionName
    lua_setfield(L, -2, functionName.c_str());
    // pop the table
    lua_pop(L, 1);
}

void LuaScript::registerGlobalFunction(const std::string& functionName, LuaCFunction function)
{
    int functionId = m_functions.size();
    m_functions.push_back(function);

    // push the function id
    lua_pushnumber(L, functionId);
    // store id in the closure
    lua_pushcclosure(L, &LuaScript::luaFunctionCallback, 1);
    // store the function in the global lua table
    lua_setfield(L, LUA_GLOBALSINDEX, functionName.c_str());
}

int LuaScript::luaPackageLoader(lua_State* L)
{
    std::string fileName = lua_tostring(L, -1);
    fileName += ".lua";
    if(g_resources.fileExists(fileName)) {
        std::string fileContents = g_resources.loadTextFile(fileName);
        luaL_loadbuffer(L, fileContents.c_str(), fileContents.length(), fileName.c_str());
    } else {
        lua_pushfstring(L, "\n\tcould not load lua script " LUA_QS, fileName.c_str());
    }
    return 1;
}

int LuaScript::luaCollectClassInstance(lua_State* L)
{
    ScriptableWeakPtr *objectRef = (ScriptableWeakPtr *)lua_touserdata(L, -1);
    objectRef->reset();
    lua_pop(L, 1);
    return 1;
}

int LuaScript::luaCompareClassInstances(lua_State* L)
{
    if(!lua_isuserdata(L, -1) || !lua_isuserdata(L, -2)) {
        lua_pop(L, 2);
        lua_pushboolean(L, 0);
    } else {
        ScriptableWeakPtr *objectRef1 = (ScriptableWeakPtr *)lua_touserdata(L, -1);
        ScriptableWeakPtr *objectRef2 = (ScriptableWeakPtr *)lua_touserdata(L, -2);
        lua_pop(L, 2);
        if(objectRef1->lock() == objectRef2->lock())
            lua_pushboolean(L, 1);
        else
            lua_pushboolean(L, 0);
    }
    return 1;
}

int LuaScript::luaFunctionCallback(lua_State* L)
{
    int id = lua_tonumber(L, lua_upvalueindex(1));
    return (g_lua.*(g_lua.m_functions[id]))();
}

int LuaScript::luaErrorHandler(lua_State *L)
{
    lua_getfield(L, LUA_GLOBALSINDEX, "debug");
    if(!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return 1;
    }
    lua_getfield(L, -1, "traceback");
    if(!lua_isfunction(L, -1)) {
        lua_pop(L, 2);
        return 1;
    }
    lua_pushvalue(L, 1);
    lua_pushinteger(L, 2);
    lua_call(L, 2, 1);
    return 1;
}
