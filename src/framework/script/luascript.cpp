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
#include <script/luafunctions.h>
#include <core/resources.h>

LuaScript g_lua;

LuaScript::LuaScript()
{
    L = luaL_newstate();
    if(!L)
        logFatal("FATAL ERROR: could not create lua context");

    // load lua standard libraries
    luaL_openlibs(L);

    // setup custom package loader
    setupPackageLoader();

    registerLuaFunctions();
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
        flogError("ERROR: script file '%s' doesn't exist", fileName.c_str());
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
    ss << "LUA Script ERROR: ";
    if(funcName)
        ss << " in " << funcName << "(): ";
    ss << errorDesc << std::endl;
    logError(ss.str());
}

void LuaScript::reportErrorWithTraceback(const std::string& errorDesc, const char* funcName)
{
    pushString(errorDesc);
    luaErrorHandler(L);
    reportError(popString(), funcName);
}

void LuaScript::collectGarbage()
{
    for(int i=0;i<2;i++)
        lua_gc(L, LUA_GCCOLLECT, 0);
}

int LuaScript::getStackSize()
{
    return lua_gettop(L);
}

void LuaScript::insert(int index)
{
    lua_insert(L, index);
}

int LuaScript::ref()
{
    return luaL_ref(L, LUA_REGISTRYINDEX);
}

void LuaScript::unref(int ref)
{
    luaL_unref(L, LUA_REGISTRYINDEX, ref);
}

void LuaScript::getRef(int ref)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
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
    pop();
    return (int)d;
}

std::string LuaScript::popString()
{
    std::string str;
    if(lua_isstring(L, -1))
        str = lua_tostring(L, -1);
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

void LuaScript::pushValue(int index)
{
    lua_pushvalue(L, index);
}

void LuaScript::pushClassInstance(const ScriptablePtr& object)
{
    if(object) {
        // create weak_ptr to the scriptable object stored as userdata
        new(lua_newuserdata(L, sizeof(ScriptablePtr))) ScriptablePtr(object);
        // set object metatable
        lua_getfield(L, LUA_REGISTRYINDEX, (std::string(object->getScriptableName()) + "_mt").c_str());
        lua_setmetatable(L, -2);
    } else {
        lua_pushnil(L);
    }
}

ScriptablePtr LuaScript::popClassInstance()
{
    ScriptablePtr object;
    if(lua_isuserdata(L, -1)) { // instances are store as userdata
        object = *((ScriptablePtr *)lua_touserdata(L, -1));
        if(!object)
            reportErrorWithTraceback("attempt to retrive class instance from a object that is already expired");
    } else if(!lua_isnil(L, -1)) // we accept nil values
        reportErrorWithTraceback("couldn't retrive class instance, the value is not a scriptable type");
    lua_pop(L, 1);
    return object;
}

void LuaScript::pushFunction(int functionRef)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, functionRef);
}

int LuaScript::popFunction()
{
    return ref();
}

void LuaScript::releaseFunction(int functionRef)
{
    unref(functionRef);
}

void LuaScript::callFunction(int numArgs)
{
    int size = lua_gettop(L);
    int errorIndex = -(numArgs + 2);
    lua_pushcfunction(L, &LuaScript::luaErrorHandler);
    lua_insert(L, errorIndex);

    int ret = lua_pcall(L, numArgs, 0, errorIndex);
    if(ret != 0) {
        reportError(popString());
    }

    // pop error func
    lua_pop(L, 1);

    if(lua_gettop(L) != size - numArgs - 1)
        reportError("stack size changed!");
}

SimpleCallback LuaScript::createSimpleFuncCallback(int funcRef)
{
    return [this, funcRef]() {
        pushFunction(funcRef);
        callFunction();
    };
}

boost::function<void(ScriptablePtr)> LuaScript::createScriptableSelfFuncCallback(int funcRef)
{
    return [this, funcRef](ScriptablePtr scriptable) {
        pushClassInstance(scriptable);
        setGlobal("self");

        pushFunction(funcRef);
        callFunction();

        pushNil();
        setGlobal("self");
    };
}

void LuaScript::setGlobal(const char *varName)
{
    lua_setfield(L, LUA_GLOBALSINDEX, varName);
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
    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setfield(L, LUA_REGISTRYINDEX, (klass + "_mt").c_str());

    // set __index metamethod
    lua_pushcfunction(L, &LuaScript::luaIndexMetaMethod);
    lua_setfield(L, -2, "__index");

    // set __newindex metamethod
    lua_pushcfunction(L, &LuaScript::luaNewIndexMetaMethod);
    lua_setfield(L, -2, "__newindex");

    // set __eq metamethod
    lua_pushcfunction(L, &LuaScript::luaEqualMetaMethod);
    lua_setfield(L, -2, "__eq");

    // set __gc metamethod, which collects userdata
    lua_pushcfunction(L, &LuaScript::luaGarbageCollectMetaMethod);
    lua_setfield(L, -2, "__gc");

    // klass_mt.methods = { }
    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setfield(L, LUA_GLOBALSINDEX, klass.c_str());
    lua_setfield(L, -2, "methods");

    // klass_mt.fieldmethods = { }
    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setfield(L, LUA_GLOBALSINDEX, (klass + "_fieldmethods").c_str());
    lua_setfield(L, -2, "fieldmethods");

    if(baseClass.length()) {
        // klass_mt.base = baseClass_mt
        lua_getfield(L, LUA_REGISTRYINDEX, (baseClass + "_mt").c_str());
        lua_setfield(L, -2, "base");
    }

    lua_pop(L, 1);
}

void LuaScript::registerMemberField(const std::string& klass, const std::string& field, LuaScript::LuaCFunction getFunction, LuaScript::LuaCFunction setFunction)
{
    if(getFunction) {
        int functionId = m_functions.size();
        m_functions.push_back(getFunction);

        // push the class table
        lua_getfield(L, LUA_GLOBALSINDEX, (klass + "_fieldmethods").c_str());
        // push the function id
        lua_pushnumber(L, functionId);
        // store id in the closure
        lua_pushcclosure(L, &LuaScript::luaFunctionCallback, 1);
        // store the function at the class field functionName
        lua_setfield(L, -2, ("get_" + field).c_str());
        // pop the table
        lua_pop(L, 1);
    }

    if(setFunction) {
        int functionId = m_functions.size();
        m_functions.push_back(setFunction);

        // push the class table
        lua_getfield(L, LUA_GLOBALSINDEX, (klass + "_fieldmethods").c_str());
        // push the function id
        lua_pushnumber(L, functionId);
        // store id in the closure
        lua_pushcclosure(L, &LuaScript::luaFunctionCallback, 1);
        // store the function at the class field functionName
        lua_setfield(L, -2, ("set_" + field).c_str());
        // pop the table
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

void LuaScript::registerModule(const std::string& module)
{
    // module = {}
    lua_newtable(L); // module table
    lua_setfield(L, LUA_GLOBALSINDEX, module.c_str()); // register at globals index
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

int LuaScript::luaIndexMetaMethod(lua_State* L)
{
    std::string key = lua_tostring(L, -1); // key, obj
    lua_pop(L, 1); // obj
    lua_getmetatable(L, -1); // mt, obj
    while(!lua_isnil(L, -1)) {
        lua_getfield(L, -1, "fieldmethods"); // mt.fieldmethods, mt, obj
        lua_getfield(L, -1, ("get_" + key).c_str()); // mt.fieldmethods[get_key], mt.fieldmethods, mt, obj
        lua_remove(L, -2); // mt.fieldmethods[get_key], mt, obj
        if(!lua_isnil(L, -1)) {
            lua_remove(L, -2); // mt.fieldmethods[get_key], obj
            lua_insert(L, -2); // obj, mt.fieldmethods[get_key]
            lua_pushcfunction(L, &LuaScript::luaErrorHandler); // errorfunc, obj, mt.fieldmethods[get_key]
            lua_insert(L, -3); // obj, mt.fieldmethods[get_key], errorfunc
            int ret = lua_pcall(L, 1, 1, -3); // ret, errorfunc
            if(ret != 0) {
                g_lua.reportError(g_lua.popString()); // errofunc
                lua_pop(L, 1); // (empty)
                lua_pushnil(L); // nil
            } else {
                lua_remove(L, -2); // ret
            }
            return 1;
        } else {
            lua_pop(L, 1); // mt, obj
            lua_getfield(L, -1, "methods"); // mt.methods, mt, obj
            lua_getfield(L, -1, key.c_str()); // mt.methods[key], mt.methods, mt, obj
            lua_remove(L, -2); // mt.methods[key], mt, obj
            if(!lua_isnil(L, -1)) {
                lua_insert(L, -3);
                lua_pop(L, 2);
                return 1;
            }
            lua_pop(L, 1);
        }
        lua_getfield(L, -1, "base"); // mt.base, mt, obj
        lua_remove(L, -2); // mt.base, obj
    }
    lua_pop(L, 1);

    ScriptablePtr scriptable = g_lua.popClassInstance();
    int refId = scriptable->getLuaRef(key);
    if(refId != -1)
        g_lua.getRef(refId);
    else
        g_lua.pushNil();

    return 1;
}

int LuaScript::luaNewIndexMetaMethod(lua_State* L)
{
    // stack: value, key, obj
    lua_insert(L, -2);
    std::string key = lua_tostring(L, -1);
    lua_pop(L, 1);
    lua_getmetatable(L, -2);
    // stack: mt, value, obj
    while(!lua_isnil(L, -1)) {
        lua_getfield(L, -1, "fieldmethods");
        lua_getfield(L, -1, ("set_" + key).c_str());
        lua_remove(L, -2); // stack: set method, mt, value, obj
        if(!lua_isnil(L, -1)) {
            lua_remove(L, -2); // mt.fieldmethods[get_key], value, obj
            lua_insert(L, -3); // value, obj, mt.fieldmethods[get_key]
            lua_pushcfunction(L, &LuaScript::luaErrorHandler); // errorfunc, value, obj, mt.fieldmethods[get_key]
            lua_insert(L, -4); // value, obj, mt.fieldmethods[get_key], errorfunc
            int ret = lua_pcall(L, 2, 0, -4); // errorfunc
            if(ret != 0)
                g_lua.reportError(g_lua.popString()); // errofunc
            lua_pop(L, 1);
            return 1;
        }
        lua_pop(L, 1); // mt, value, obj
        lua_getfield(L, -1, "base"); // mt.base, mt, value, obj
        lua_remove(L, -2); // mt.base, value, obj
    }

    // stack:
    g_lua.pop(); // value, obj
    g_lua.insert(-2); // obj, value
    ScriptablePtr scriptable = g_lua.popClassInstance();
    g_lua.pushValue(); // value, value
    int refId = g_lua.ref();
    scriptable->associateLuaRef(key, refId);
    g_lua.pop();

    return 1;
}

int LuaScript::luaEqualMetaMethod(lua_State* L)
{
    if(!lua_isuserdata(L, -1) || !lua_isuserdata(L, -2)) {
        lua_pop(L, 2);
        lua_pushboolean(L, 0);
    } else {
        ScriptablePtr *objectRef1 = (ScriptablePtr *)lua_touserdata(L, -1);
        ScriptablePtr *objectRef2 = (ScriptablePtr *)lua_touserdata(L, -2);
        lua_pop(L, 2);
        if(objectRef1 == objectRef2)
            lua_pushboolean(L, 1);
        else
            lua_pushboolean(L, 0);
    }
    return 1;
}

int LuaScript::luaGarbageCollectMetaMethod(lua_State* L)
{
    ScriptablePtr *objectRef = (ScriptablePtr *)lua_touserdata(L, -1);
    objectRef->reset();
    lua_pop(L, 1);
    return 1;
}

int LuaScript::luaFunctionCallback(lua_State* L)
{
    // look for function id
    int id = lua_tonumber(L, lua_upvalueindex(1));
    // call the function
    return (*(g_lua.m_functions[id]))();
}

int LuaScript::luaErrorHandler(lua_State *L)
{
    // push debug
    lua_getfield(L, LUA_GLOBALSINDEX, "debug");
    if(!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return 1;
    }
    // push debug.traceback
    lua_getfield(L, -1, "traceback");
    if(!lua_isfunction(L, -1)) {
        lua_pop(L, 2);
        return 1;
    }
    // remove debug
    lua_remove(L, -2);
    // push additional error message
    lua_pushvalue(L, -2);
    // push level
    lua_pushinteger(L, 2);
    // call debug.traceback
    lua_call(L, 2, 1);
    // remove additional error message
    lua_remove(L, -2);
    // return, the complete error message is on the stack
    return 1;
}
