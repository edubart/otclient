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
#include <lua.hpp>

LuaScript g_lua;

void LuaScript::init()
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

void LuaScript::terminate()
{
    collectGarbage();
    lua_close(L);
    L = NULL;
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
        pop();
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

bool LuaScript::loadBufferAsFunction(const std::string& text, const std::string& what)
{
    int ret = luaL_loadbuffer(L, text.c_str(), text.length(), what.c_str());
    if(ret != 0){
        reportError(popString());
        return false;
    }

    if(!isFunction()) {
        pop();
        return false;
    }

    return true;
}

void LuaScript::reportError(const std::string& errorDesc, const char *funcName)
{
    std::stringstream ss;
    ss << "LUA Script ERROR: ";
    if(funcName)
        ss << " in " << funcName << "(): ";
    ss << errorDesc;
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

void LuaScript::remove(int index)
{
    lua_remove(L, index);
}

bool LuaScript::next(int index)
{
    return lua_next(L, index);
}

void LuaScript::releaseRef(int ref)
{
    luaL_unref(L, LUA_REGISTRYINDEX, ref);
}

void LuaScript::newTable()
{
    lua_newtable(L);
}

void LuaScript::setTable(int index)
{
    lua_settable(L, index);
}

void *LuaScript::newUserdata(int size)
{
    return lua_newuserdata(L, size);
}

void LuaScript::newMetatable(const std::string& name)
{
    luaL_newmetatable(L, name.c_str());
}

void LuaScript::setMetatable(const std::string& name, int index)
{
    luaL_getmetatable(L, name.c_str());
    if(isNil())
        reportError(f("could not retrive metatable %d", name));
    else
        lua_setmetatable(L, index < 0 ? index-1 : index);
}

void LuaScript::rawGet(const std::string& key)
{
    pushString(key);
    lua_rawget(L, -2);
}

void LuaScript::rawSet(const std::string& key)
{
    pushString(key);
    insert(-2);
    lua_rawset(L, -3);
}

void LuaScript::getField(const std::string& key)
{
    lua_getfield(L, -1, key.c_str());
}

void LuaScript::setField(const std::string& key)
{
    lua_setfield(L, -2, key.c_str());
}

void LuaScript::getScriptableField(const ScriptablePtr& scriptable, const std::string& field)
{
    if(scriptable) {
        pushRef(scriptable->getLuaTableRef());
        getField(field);
        remove(-2);
    } else
        pushNil();
}

void LuaScript::setScriptableField(const ScriptablePtr& scriptable, const std::string& field)
{
    if(scriptable) {
        pushRef(scriptable->getLuaTableRef());
        insert(-2);
        setField(field);
    }
    pop();
}

void LuaScript::rawGetGlobalTableField(const std::string& globalTable, const std::string& key)
{
    getGlobal(globalTable);
    rawGet(key);
    remove(-2);
}

void LuaScript::rawSetGlobalTableField(const std::string& globalTable, const std::string& key)
{
    getGlobal(globalTable);
    insert(-2);
    rawSet(key);
    pop();
}

void LuaScript::getGlobal(const std::string& key)
{
    lua_getfield(L, LUA_GLOBALSINDEX, key.c_str());
}

void LuaScript::setGlobal(const std::string& key)
{
    lua_setfield(L, LUA_GLOBALSINDEX, key.c_str());
}

bool LuaScript::isNil(int index)
{
    return lua_isnil(L, index);
}

bool LuaScript::isBoolean(int index)
{
    return lua_isboolean(L, index);
}

bool LuaScript::isNumber(int index)
{
    return lua_isnumber(L, index);
}

bool LuaScript::isString(int index)
{
    return lua_isstring(L, index);
}

bool LuaScript::isTable(int index)
{
    return lua_istable(L, index);
}

bool LuaScript::isUserdata(int index)
{
    return lua_isuserdata(L, index);
}

bool LuaScript::isFunction(int index)
{
    return lua_isfunction(L, index);
}

bool LuaScript::isCFunction(int index)
{
    return lua_iscfunction(L, index);
}

bool LuaScript::isLuaFunction(int index)
{
    return isFunction(index) && !isCFunction(index);
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

int LuaScript::popInteger()
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

ScriptablePtr LuaScript::popClassInstance()
{
    ScriptablePtr object;
    if(isUserdata()) { // instances are store as userdata
        object = *((ScriptablePtr *)lua_touserdata(L, -1));
        if(!object)
            reportErrorWithTraceback("attempt to retrive class instance from a object that is already expired");
    } else if(!isNil()) // we accept nil values
        reportErrorWithTraceback("couldn't retrive class instance, the value is not a scriptable type");
    pop();
    return object;
}

int LuaScript::popRef()
{
    return luaL_ref(L, LUA_REGISTRYINDEX);
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

void LuaScript::pushClassInstance(const ScriptablePtr& object)
{
    if(object) {
        new(newUserdata(sizeof(ScriptablePtr))) ScriptablePtr(object);
        setMetatable(std::string(object->getScriptableName()) + "_mt");
    } else
        pushNil();
}

void LuaScript::pushValue(int index)
{
    lua_pushvalue(L, index);
}

void LuaScript::pushRef(int ref)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
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

void LuaScript::callModuleField(const std::string& module, const std::string& field)
{
    g_lua.rawGetGlobalTableField(module, field);
    g_lua.callFunction();
}

void LuaScript::setupPackageLoader()
{
    lua_getfield(L, LUA_GLOBALSINDEX, "package"); // package
    lua_getfield(L, -1, "loaders"); // package.loaders, package
    lua_remove(L, -2); // package.loaders
    lua_pushnil(L); // nil, package.loaders
    int numLoaders = 0;
    while(lua_next(L, -2) != 0) {
        // value, key, package.loaders
        lua_pop(L, 1); // key, package.loaders
        numLoaders++;
    }
    lua_pushinteger(L, numLoaders + 1); // at, package.loaders
    lua_pushcfunction(L, &LuaScript::luaPackageLoader); // luaPackageLoader, at, package.loaders
    lua_rawset(L, -3); // package.loaders
    lua_pop(L, 1); // (empty)
}

void LuaScript::registerClass(const std::string& klass, const std::string& baseClass)
{
    // mt = {}
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

    // klass = { }
    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setfield(L, LUA_GLOBALSINDEX, klass.c_str());

    // mt.methods = klass
    lua_setfield(L, -2, "methods");

    // klass_fieldmethods = { }
    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setfield(L, LUA_GLOBALSINDEX, (klass + "_fieldmethods").c_str());

    // mt.fieldmethods = klass_fieldmethods
    lua_setfield(L, -2, "fieldmethods");

    if(baseClass.length()) {
        // get klass
        lua_getfield(L, LUA_GLOBALSINDEX, klass.c_str());

        // redirect = { __index = baseClass }
        lua_newtable(L);
        lua_getfield(L, LUA_GLOBALSINDEX, baseClass.c_str());
        lua_setfield(L, -2, "__index");

        // setmetatable(klass, redirect)
        lua_setmetatable(L, -2);
        lua_pop(L, 1);

        // get klass_fieldmethods
        lua_getfield(L, LUA_GLOBALSINDEX, (klass + "_fieldmethods").c_str());

        // redirect = { __index = baseClass_fieldmethods }
        lua_newtable(L);
        lua_getfield(L, LUA_GLOBALSINDEX, (baseClass + "_fieldmethods").c_str());
        lua_setfield(L, -2, "__index");

        // setmetatable(klass_fieldmethods, redirect)
        lua_setmetatable(L, -2);
    }

    lua_pop(L, 1);

    m_currentClass = klass;
}

void LuaScript::registerMemberField(const std::string& field, LuaScript::LuaCFunction getFunction, LuaScript::LuaCFunction setFunction)
{
    if(getFunction) {
        int functionId = m_functions.size();
        m_functions.push_back(getFunction);

        // push the class table
        lua_getfield(L, LUA_GLOBALSINDEX, (m_currentClass + "_fieldmethods").c_str());
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
        lua_getfield(L, LUA_GLOBALSINDEX, (m_currentClass + "_fieldmethods").c_str());
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

void LuaScript::registerMemberFunction(const std::string& functionName, LuaCFunction function)
{
    int functionId = m_functions.size();
    m_functions.push_back(function);

    // push the class table
    lua_getfield(L, LUA_GLOBALSINDEX, m_currentClass.c_str());
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
    registerClass(module);
    lua_getfield(L, LUA_GLOBALSINDEX, module.c_str());
    lua_getfield(L, LUA_REGISTRYINDEX, (module + "_mt").c_str());
    lua_setmetatable(L, -2);
    lua_pop(L, -1);
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
    // stack: key, obj
    std::string key = lua_tostring(L, -1); // key, obj
    lua_pop(L, 1); // obj
    lua_getmetatable(L, -1); // mt, obj
    lua_getfield(L, -1, "fieldmethods"); // fieldmethods, mt, obj
    lua_getfield(L, -1, ("get_" + key).c_str()); // get_method, fieldmethods, mt, obj
    lua_remove(L, -2); // get_method, mt, obj
    if(!lua_isnil(L, -1)) {
        lua_remove(L, -2); // get_method, obj
        lua_insert(L, -2); // obj, get_method
        lua_pushcfunction(L, &LuaScript::luaErrorHandler); // errorfunc, obj,get_method
        lua_insert(L, -3); // obj, get_method, errorfunc
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
        lua_getfield(L, -1, "methods"); // methods, mt, obj
        lua_getfield(L, -1, key.c_str()); // method, methods, mt, obj
        lua_remove(L, -2); // method, mt, obj
        if(!lua_isnil(L, -1)) {
            lua_insert(L, -3); // obj, mt, method
            lua_pop(L, 2); // method
            return 1;
        }
        lua_pop(L, 1); // mt, obj
    }
    lua_pop(L, 1); // obj

    // stack: obj
    if(g_lua.isTable()) {
        g_lua.rawGet(key);
        g_lua.remove(-2);
    } else if(g_lua.isUserdata()) {
        g_lua.getScriptableField(g_lua.popClassInstance(), key);
    } else {
        g_lua.pop();
        g_lua.pushNil();
        g_lua.reportErrorWithTraceback("unknown type in newindex metamethod");
    }

    return 1;
}

int LuaScript::luaNewIndexMetaMethod(lua_State* L)
{
    // stack: value, key, obj
    lua_insert(L, -2); // key, value, obj
    std::string key = lua_tostring(L, -1);
    lua_pop(L, 1); //  value, obj
    lua_getmetatable(L, -2); // mt, value, obj
    lua_getfield(L, -1, "fieldmethods"); // fieldmethods, mt, value, obj
    lua_getfield(L, -1, ("set_" + key).c_str()); // set_method, fieldmethods, mt, value, obj
    lua_remove(L, -2); //  set_method, mt, value, obj
    if(!lua_isnil(L, -1)) {
        lua_remove(L, -2); // set_method, value, obj
        lua_insert(L, -3); // value, obj, set_method
        lua_pushcfunction(L, &LuaScript::luaErrorHandler); // errorfunc, value, obj, set_method
        lua_insert(L, -4); // value, obj, set_method, errorfunc
        int ret = lua_pcall(L, 2, 0, -4); // errorfunc
        if(ret != 0)
            g_lua.reportError(g_lua.popString()); // errofunc
        lua_pop(L, 1); // (empty)
        return 1;
    }
    lua_pop(L, 2); // mt, value, obj

    // stack: value, obj
    g_lua.insert(-2);
    if(g_lua.isTable()) {
        g_lua.insert(-2);
        g_lua.rawSet(key);
        g_lua.pop();
    } else if(g_lua.isUserdata()) {
        g_lua.setScriptableField(g_lua.popClassInstance(), key);
    } else {
        g_lua.pop(2);
        g_lua.reportErrorWithTraceback("unknown type in index metamethod");
    }

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
    if(lua_isuserdata(L, -1)) {
        ScriptablePtr *objectRef = (ScriptablePtr *)lua_touserdata(L, -1);
        objectRef->reset();
    }
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
