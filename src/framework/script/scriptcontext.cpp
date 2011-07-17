#include <global.h>
#include <script/scriptcontext.h>
#include <script/scriptfunctions.h>
#include <core/resources.h>
#include <lua.hpp>

#include <boost/algorithm/string.hpp>

ScriptContext g_lua;

void ScriptContext::init()
{
    L = luaL_newstate();
    if(!L)
        fatal("FATAL ERROR: could not create lua context");

    // load lua standard libraries
    luaL_openlibs(L);

    // setup custom package loader
    setupPackageLoader();

    registerScriptFunctions();
}

void ScriptContext::terminate()
{
    collectGarbage();
    lua_close(L);
    L = NULL;
}

void ScriptContext::loadAllModules()
{
    std::list<std::string> modules = g_resources.listDirectoryFiles("modules");
    foreach(const std::string& module, modules) {
        std::list<std::string> moduleFiles = g_resources.listDirectoryFiles("modules/" + module);
        foreach(const std::string& moduleFile, moduleFiles) {
            if(boost::ends_with(moduleFile, ".lua"))
                loadFile(std::string("modules/") + module + "/" + moduleFile);
        }
    }
}

bool ScriptContext::loadFile(const std::string& fileName)
{
    std::stringstream fin;
    if(g_resources.loadFile(fileName, fin))
        return loadBuffer(fin.str(), fileName);
    else
        error("ERROR: script file '", fileName, "' doesn't exist");
    return false;
}

bool ScriptContext::loadBuffer(const std::string& text, const std::string& what)
{
    if(loadBufferAsFunction(text, what) && callFunction())
        return true;
    return false;
}

bool ScriptContext::loadBufferAsFunction(const std::string& text, const std::string& what)
{
    int ret = luaL_loadbuffer(L, text.c_str(), text.length(), ("@" + what).c_str());
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

void ScriptContext::reportError(const std::string& errorDesc, const char *funcName)
{
    std::stringstream ss;
    ss << "LUA Script ERROR: ";
    if(funcName)
        ss << " in " << funcName << "(): ";
    ss << errorDesc;
    error(ss.str());
}

void ScriptContext::reportErrorWithTraceback(const std::string& errorDesc, const char* funcName)
{
    pushString(errorDesc);
    luaErrorHandler(L);
    reportError(popString(), funcName);
}

void ScriptContext::collectGarbage()
{
    for(int i=0;i<2;i++)
        lua_gc(L, LUA_GCCOLLECT, 0);
}

int ScriptContext::getStackSize()
{
    return lua_gettop(L);
}

void ScriptContext::insert(int index)
{
    if(index != -1)
        lua_insert(L, index);
}

void ScriptContext::swap(int index)
{
    insert(index);
    pushValue(index+1);
    remove(index);
}

void ScriptContext::remove(int index)
{
    lua_remove(L, index);
}

bool ScriptContext::next(int index)
{
    return lua_next(L, index);
}

void ScriptContext::releaseRef(int ref)
{
    luaL_unref(L, LUA_REGISTRYINDEX, ref);
}

void ScriptContext::newTable()
{
    lua_newtable(L);
}

void ScriptContext::setTable(int index)
{
    lua_settable(L, index);
}

void *ScriptContext::newUserdata(int size)
{
    return lua_newuserdata(L, size);
}

void ScriptContext::newMetatable(const std::string& name)
{
    luaL_newmetatable(L, name.c_str());
}

void ScriptContext::setMetatable(const std::string& name, int index)
{
    luaL_getmetatable(L, name.c_str());
    if(isNil())
        reportError(make_string("could not retrive metatable", name));
    else
        lua_setmetatable(L, index < 0 ? index-1 : index);
}

void ScriptContext::rawGet(const std::string& key)
{
    pushString(key);
    lua_rawget(L, -2);
}

void ScriptContext::rawSet(const std::string& key)
{
    pushString(key);
    insert(-2);
    lua_rawset(L, -3);
}

void ScriptContext::getField(const std::string& key)
{
    lua_getfield(L, -1, key.c_str());
}

void ScriptContext::setField(const std::string& key)
{
    lua_setfield(L, -2, key.c_str());
}

void ScriptContext::getScriptObjectField(const ScriptObjectPtr& scriptobject, const std::string& field)
{
    if(scriptobject) {
        pushRef(scriptobject->getScriptTable());
        getField(field);
        remove(-2);
    } else
        pushNil();
}

void ScriptContext::setScriptObjectField(const ScriptObjectPtr& scriptobject, const std::string& field)
{
    if(scriptobject) {
        pushRef(scriptobject->getScriptTable());
        insert(-2);
        setField(field);
    }
    pop();
}

void ScriptContext::rawGetGlobalTableField(const std::string& globalTable, const std::string& key)
{
    getGlobal(globalTable);
    rawGet(key);
    remove(-2);
}

void ScriptContext::rawSetGlobalTableField(const std::string& globalTable, const std::string& key)
{
    getGlobal(globalTable);
    insert(-2);
    rawSet(key);
    pop();
}

void ScriptContext::getGlobal(const std::string& key)
{
    lua_getfield(L, LUA_GLOBALSINDEX, key.c_str());
}

void ScriptContext::setGlobal(const std::string& key)
{
    lua_setfield(L, LUA_GLOBALSINDEX, key.c_str());
}

bool ScriptContext::isNil(int index)
{
    return lua_isnil(L, index);
}

bool ScriptContext::isBoolean(int index)
{
    return lua_isboolean(L, index);
}

bool ScriptContext::isNumber(int index)
{
    return lua_isnumber(L, index);
}

bool ScriptContext::isString(int index)
{
    return lua_isstring(L, index);
}

bool ScriptContext::isTable(int index)
{
    return lua_istable(L, index);
}

bool ScriptContext::isUserdata(int index)
{
    return lua_isuserdata(L, index);
}

bool ScriptContext::isFunction(int index)
{
    return lua_isfunction(L, index);
}

bool ScriptContext::isCFunction(int index)
{
    return lua_iscfunction(L, index);
}

bool ScriptContext::isLuaFunction(int index)
{
    return isFunction(index) && !isCFunction(index);
}

void ScriptContext::pop(int n)
{
    lua_pop(L, n);
}

bool ScriptContext::popBoolean()
{
    bool b = (lua_toboolean(L, -1) != 0);
    pop();
    return b;
}

int ScriptContext::popInteger()
{
    double d = lua_tonumber(L, -1);
    pop();
    return (int)d;
}

std::string ScriptContext::popString()
{
    std::string str;
    if(lua_isstring(L, -1))
        str = lua_tostring(L, -1);
    pop();
    return str;
}

ScriptObjectPtr ScriptContext::popClassInstance()
{
    ScriptObjectPtr object;
    if(isUserdata()) { // instances are store as userdata
        object = *((ScriptObjectPtr *)lua_touserdata(L, -1));
        if(!object)
            reportErrorWithTraceback("attempt to retrive class instance from a object that is already expired");
    } else if(!isNil()) // we accept nil values
        reportErrorWithTraceback("couldn't retrive class instance, the value is not a scriptobject type");
    pop();
    return object;
}

int ScriptContext::popRef()
{
    return luaL_ref(L, LUA_REGISTRYINDEX);
}

void ScriptContext::pushNil()
{
    lua_pushnil(L);
}

void ScriptContext::pushBoolean(bool b)
{
    lua_pushboolean(L, b);
}

void ScriptContext::pushInteger(int32_t i)
{
    lua_pushnumber(L, i);
}

void ScriptContext::pushString(const std::string& str)
{
    lua_pushstring(L, str.c_str());
}

void ScriptContext::pushClassInstance(const ScriptObjectPtr& object)
{
    if(object) {
        new(newUserdata(sizeof(ScriptObjectPtr))) ScriptObjectPtr(object);
        setMetatable(std::string(object->getScriptObjectType()) + "_mt");
    } else
        pushNil();
}

void ScriptContext::pushValue(int index)
{
    lua_pushvalue(L, index);
}

void ScriptContext::pushRef(int ref)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
}

std::string ScriptContext::getFunctionSourcePath(bool functionIsOnStack, int level)
{
    std::string path;

    lua_Debug ar;
    memset(&ar, 0, sizeof(ar));
    if(functionIsOnStack)
        lua_getinfo(L, ">Sn", &ar);
    else {
        if(lua_getstack(L, level-1, &ar) == 1)
            lua_getinfo(L, "Sn", &ar);
    }

    // c function, we must get information of a level above
    if(strcmp("C", ar.what) == 0) {
        memset(&ar, 0, sizeof(ar));
        if(lua_getstack(L, level, &ar) == 1) {
            lua_getinfo(L, "f", &ar);
            return getFunctionSourcePath(true, level+1);
        }

    } else {
        if(ar.source) {
            std::string source = ar.source;
            std::size_t pos = source.find_last_of('/');
            if(source[0] == '@' && pos != std::string::npos)
                path = source.substr(1, pos - 1);
        } else {
            error("no source");
        }
    }
    return path;
}

bool ScriptContext::callFunction(int numArgs, int numRets)
{
    pushValue(-numArgs - 1);
    g_resources.pushCurrentPath(getFunctionSourcePath(true));

    int size = getStackSize();
    int errorIndex = -numArgs - 2;
    lua_pushcfunction(L, &ScriptContext::luaErrorHandler);
    insert(errorIndex);

    int ret = lua_pcall(L, numArgs, numRets, errorIndex);
    if(ret != 0) {
        reportError(popString());
        pop(); // pop error func
        return false;
    }

    remove(-numRets - 1); // pop error func

    if(getStackSize() != size - numArgs - 1 + numRets) {
        reportError("stack size changed!");
        return false;
    }

    g_resources.popCurrentPath();
    return true;
}

void ScriptContext::callModuleField(const std::string& module, const std::string& field)
{
    g_lua.rawGetGlobalTableField(module, field);
    g_lua.callFunction();
}

void ScriptContext::setupPackageLoader()
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
    lua_pushcfunction(L, &ScriptContext::luaPackageLoader); // luaPackageLoader, at, package.loaders
    lua_rawset(L, -3); // package.loaders
    lua_pop(L, 1); // (empty)
}

void ScriptContext::registerClass(const std::string& klass, const std::string& baseClass)
{
    // mt = {}
    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setfield(L, LUA_REGISTRYINDEX, (klass + "_mt").c_str());

    // set __index metamethod
    lua_pushcfunction(L, &ScriptContext::luaIndexMetaMethod);
    lua_setfield(L, -2, "__index");

    // set __newindex metamethod
    lua_pushcfunction(L, &ScriptContext::luaNewIndexMetaMethod);
    lua_setfield(L, -2, "__newindex");

    // set __eq metamethod
    lua_pushcfunction(L, &ScriptContext::luaEqualMetaMethod);
    lua_setfield(L, -2, "__eq");

    // set __gc metamethod, which collects userdata
    lua_pushcfunction(L, &ScriptContext::luaGarbageCollectMetaMethod);
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

void ScriptContext::registerMemberField(const std::string& field, ScriptContext::LuaCFunction getFunction, ScriptContext::LuaCFunction setFunction)
{
    if(getFunction) {
        int functionId = m_functions.size();
        m_functions.push_back(getFunction);

        // push the class table
        lua_getfield(L, LUA_GLOBALSINDEX, (m_currentClass + "_fieldmethods").c_str());
        // push the function id
        lua_pushnumber(L, functionId);
        // store id in the closure
        lua_pushcclosure(L, &ScriptContext::luaFunctionCallback, 1);
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
        lua_pushcclosure(L, &ScriptContext::luaFunctionCallback, 1);
        // store the function at the class field functionName
        lua_setfield(L, -2, ("set_" + field).c_str());
        // pop the table
        lua_pop(L, 1);
    }
}

void ScriptContext::registerMemberFunction(const std::string& functionName, LuaCFunction function)
{
    int functionId = m_functions.size();
    m_functions.push_back(function);

    // push the class table
    lua_getfield(L, LUA_GLOBALSINDEX, m_currentClass.c_str());
    // push the function id
    lua_pushnumber(L, functionId);
    // store id in the closure
    lua_pushcclosure(L, &ScriptContext::luaFunctionCallback, 1);
    // store the function at the class field functionName
    lua_setfield(L, -2, functionName.c_str());
    // pop the table
    lua_pop(L, 1);
}

void ScriptContext::registerGlobalFunction(const std::string& functionName, LuaCFunction function)
{
    int functionId = m_functions.size();
    m_functions.push_back(function);

    // push the function id
    lua_pushnumber(L, functionId);
    // store id in the closure
    lua_pushcclosure(L, &ScriptContext::luaFunctionCallback, 1);
    // store the function in the global lua table
    lua_setfield(L, LUA_GLOBALSINDEX, functionName.c_str());
}

void ScriptContext::registerModule(const std::string& module)
{
    registerClass(module);
    lua_getfield(L, LUA_GLOBALSINDEX, module.c_str());
    lua_getfield(L, LUA_REGISTRYINDEX, (module + "_mt").c_str());
    lua_setmetatable(L, -2);
    lua_pop(L, -1);
}

int ScriptContext::luaPackageLoader(lua_State* L)
{
    std::string fileName = lua_tostring(L, -1);
    fileName += ".lua";
    if(g_resources.fileExists(fileName)) {
        std::stringstream fin;
        if(g_resources.loadFile(fileName, fin))
            luaL_loadbuffer(L, fin.str().c_str(), fin.str().length(), fileName.c_str());
    } else {
        lua_pushfstring(L, "\n\tcould not load lua script " LUA_QS, fileName.c_str());
    }
    return 1;
}

int ScriptContext::luaIndexMetaMethod(lua_State* L)
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
        if(!g_lua.callFunction(1, 1))
            lua_pushnil(L);
        return 1;
    } else {
        lua_pop(L, 1); // mt, obj
        lua_getfield(L, -1, "methods"); // methods, mt, obj
        lua_getfield(L, -1, key.c_str()); // method, methods, mt, obj
        lua_remove(L, -2); // method, mt, obj
        if(!lua_isnil(L, -1)) {
            lua_insert(L, -3); // mt, obj, method
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
        g_lua.getScriptObjectField(g_lua.popClassInstance(), key);
    } else {
        g_lua.pop();
        g_lua.pushNil();
        g_lua.reportErrorWithTraceback("unknown type in newindex metamethod");
    }

    return 1;
}

int ScriptContext::luaNewIndexMetaMethod(lua_State* L)
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
        g_lua.callFunction(2);
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
        g_lua.setScriptObjectField(g_lua.popClassInstance(), key);
    } else {
        g_lua.pop(2);
        g_lua.reportErrorWithTraceback("unknown type in index metamethod");
    }

    return 1;
}

int ScriptContext::luaEqualMetaMethod(lua_State* L)
{
    if(!lua_isuserdata(L, -1) || !lua_isuserdata(L, -2)) {
        lua_pop(L, 2);
        lua_pushboolean(L, 0);
    } else {
        ScriptObjectPtr *objectRef1 = (ScriptObjectPtr *)lua_touserdata(L, -1);
        ScriptObjectPtr *objectRef2 = (ScriptObjectPtr *)lua_touserdata(L, -2);
        lua_pop(L, 2);
        if(objectRef1 == objectRef2)
            lua_pushboolean(L, 1);
        else
            lua_pushboolean(L, 0);
    }
    return 1;
}

int ScriptContext::luaGarbageCollectMetaMethod(lua_State* L)
{
    if(lua_isuserdata(L, -1)) {
        ScriptObjectPtr *objectRef = (ScriptObjectPtr *)lua_touserdata(L, -1);
        objectRef->reset();
    }
    lua_pop(L, 1);
    return 1;
}

int ScriptContext::luaFunctionCallback(lua_State* L)
{
    // look for function id
    int id = lua_tonumber(L, lua_upvalueindex(1));

    g_resources.pushCurrentPath(g_lua.getFunctionSourcePath(false));

    // call the function
    int ret = (*(g_lua.m_functions[id]))();

    g_resources.popCurrentPath();
    return ret;
}

int ScriptContext::luaErrorHandler(lua_State *L)
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
