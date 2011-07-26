#include "luainterface.h"

LuaInterface g_lua;

void LuaInterface::init()
{
    // installs a custom loader that will load scripts correctlyfrom lua require keyword
    auto loaders = getGlobal("package")->getField("loaders");
    loaders->setTable(loaders->getTableSize() + 1, createCppFunction(&LuaInterface::luaScriptLoader));

    // register LuaObject, the class that all other classes will derive from
    registerClass("LuaObject");
    bindClassMemberGetField("LuaObject", "use_count", &LuaObject::getUseCount);

    // register other custom classes and functions
    registerFunctions();
}

void LuaInterface::terminate()
{
    collectGarbage();
}

LuaValuePtr LuaInterface::loadFunction(const std::string& buffer, const std::string& source)
{
    LuaValuePtr func;
    // get the function contained the buffer
    if(boost::starts_with(buffer, "function")) {
        runBuffer(make_string("__func = ", buffer), source);
        func = getGlobal("__func");
        setGlobal("__func", createNil());
    // use the buffer as a function
    } else
        func = loadBuffer(buffer, source);
    return func;
}

LuaValuePtr LuaInterface::evaluateExpression(const std::string& expression, const std::string& source)
{
    // run the expression
    std::string buffer = make_string("__exp = (", expression, ")");
    runBuffer(buffer, source);

    // get the expression result
    LuaValuePtr res = getGlobal("__exp");
    setGlobal("__exp", createNil());
    return res;
}

LuaValuePtr LuaInterface::createEnvironment()
{
    // creates a new environment table and
    // redirect the new environment to the current thread global environment (aka _G)
    // this allows to access global variables from _G in the new environment
    // and prevents new variables on the new environment to be set on the global environment
    auto env = createTable();
    auto mt = createTable();
    mt->setField("__index", getGlobalEnvironment());
    env->setMetatable(mt);
    return env;
}

void LuaInterface::registerClass(const std::string& className, const std::string& baseClass)
{
    // create the class table (that it is also the class methods table)
    auto klass = createTable(className);
    // create the class metatable
    auto klass_mt = createTable(className + "_mt");
    // create the class fieldmethods table
    auto klass_fieldmethods = createTable(className + "_fieldmethods");

    // set metatable metamethods
    klass_mt->setField("__index", &LuaInterface::luaObjectGetEvent);
    klass_mt->setField("__newindex", &LuaInterface::luaObjectSetEvent);
    klass_mt->setField("__eq", &LuaInterface::luaObjectEqualEvent);
    klass_mt->setField("__gc", &LuaInterface::luaObjectCollectEvent);
    klass_mt->setField("methods", klass);
    klass_mt->setField("fieldmethods", klass_fieldmethods);

    // redirect methods and fieldmethods to the base class ones
    if(!className.empty() && className != "LuaObject") {
        /* the following code what create classes hierarchy for lua, by reproducing the following:
         * DerivedClass = { __index = BaseClass }
         * DerivedClass_fieldmethods = { __index = BaseClass_methods }
         */

        // redirect the class methods to the base methods
        auto redirect = createTable();
        auto tmp = getGlobal(baseClass);
        redirect->setField("__index", tmp);
        klass->setMetatable(redirect);

        // redirect the class fieldmethods to the base fieldmethods
        redirect = createTable();
        tmp = getGlobal(baseClass + "_fieldmethods");
        redirect->setField("__index", tmp);
        klass_fieldmethods->setMetatable(redirect);
    }
}

void LuaInterface::registerClassStaticFunction(const std::string& className, const std::string& functionName, const LuaCppFunction& function)
{
    registerClassMemberFunction(className, functionName, function);
}

void LuaInterface::registerClassMemberFunction(const std::string& className, const std::string& functionName, const LuaCppFunction& function)
{
    auto table = getGlobal(className);
    table->setField(functionName, function);
}

void LuaInterface::registerClassMemberField(const std::string& className, const std::string& field, const LuaCppFunction& getFunction, const LuaCppFunction& setFunction)
{
    auto fieldmethods = getGlobal(className + "_fieldmethods");
    auto methods = getGlobal(className);
    auto capitalized = field;
    capitalized[0] = std::toupper(field[0]);

    if(getFunction) {
        auto func = createCppFunction(getFunction);
        fieldmethods->setField(make_string("get_", field), func);
        methods->setField(make_string("get", capitalized), func);
    }

    if(setFunction) {
        auto func = createCppFunction(setFunction);
        fieldmethods->setField(make_string("set_", field), func);
        methods->setField(make_string("set", capitalized), func);
    }
}

void LuaInterface::registerGlobalFunction(const std::string& functionName, const LuaCppFunction& function)
{
    setGlobal(functionName, createCppFunction(function));
}

int LuaInterface::luaScriptLoader(LuaState* lua)
{
    // load the script as a function
    auto fileName = make_string(lua->popValue()->toString(), + ".lua");
    auto scriptMain = lua->loadScript(fileName);
    lua->pushValue(scriptMain);
    return 1;
}

int LuaInterface::luaObjectGetEvent(LuaState* lua)
{
    // metamethod that will retrive fields values (that include functions) from the object

    auto key = lua->popValue()->toString();
    auto objRef = lua->popValue();
    auto obj = safe_luavalue_cast<LuaObjectPtr>(objRef);

    // if a get method for this key exists, calls it
    auto get_method = objRef->getMetatable()->getField("fieldmethods")->getField("get_" + key);
    if(!get_method->isNil()) {
        LuaValueList rets = get_method->call(make_string(obj->getLuaTypeName(), " obj.", key), objRef);
        assert(rets.size() == 1);
        lua->pushValues(rets);
        return 1;
    }

    // if the field for this key exists, returns it
    auto field = obj->getField(key);
    if(!field->isNil()) {
        lua->pushValue(field);
        return 1;
    }

    // if a method for this key exists, returns it
    auto method = objRef->getMetatable()->getField("methods")->getField(key);
    lua->pushValue(method);
    return 1;
}

int LuaInterface::luaObjectSetEvent(LuaState* lua)
{
    // this metamethod is called when setting a field of the object by using the keyword '='

    auto value = lua->popValue();
    auto key = lua->popValue()->toString();
    auto objRef = lua->popValue();
    auto obj = safe_luavalue_cast<LuaObjectPtr>(objRef);

    // check if a set method for this field exists and call it
    auto set_method = objRef->getMetatable()->getField("fieldmethods")->getField("set_" + key);
    if(!set_method->isNil()) {
        set_method->call(make_string(obj->getLuaTypeName(), " obj.", key, "="), objRef, value);
        return 0;
    }

    // no set method exists, then sets the object table
    obj->setField(key, value);
    return 0;
}

int LuaInterface::luaObjectEqualEvent(LuaState* lua)
{
    // metamethod that will check equality of objects

    auto objRef2 = lua->popValue();
    auto objRef1 = lua->popValue();
    bool ret = false;

    // check if obj1 == obj2
    if(objRef1->isUserdata() && objRef2->isUserdata()) {
        LuaObjectPtr* objPtr1 = static_cast<LuaObjectPtr*>(objRef1->toUserdata());
        LuaObjectPtr* objPtr2 = static_cast<LuaObjectPtr*>(objRef2->toUserdata());
        assert(objPtr1 && objPtr2);
        if(*objPtr1 == *objPtr2)
            ret = true;
    }

    lua->pushBoolean(ret);
    return 1;
}

int LuaInterface::luaObjectCollectEvent(LuaState* lua)
{
    /* this metamethod is called every two lua garbage collections
     * for any LuaObject that have no references left in lua environment
     * anymore, thus this creates the possibility of holding an object
     * existence by lua
     */

    // get object pointer
    auto objRef = lua->popValue();
    auto objPtr = static_cast<LuaObjectPtr*>(objRef->toUserdata());
    assert(objPtr);

    // reset pointer to decrease object use count
    objPtr->reset();
    return 0;
}

