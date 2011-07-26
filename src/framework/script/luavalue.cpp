#include "luavalue.h"
#include "luainterface.h"
#include "luaobject.h"

#include <core/resources.h>

LuaValue::LuaValue(int ref) : ref(ref)
{
}

LuaValue::~LuaValue()
{
    // releases the reference of this value from lua
    g_lua.unref(ref);
}

LuaValueList LuaValue::call(const std::string& funcName, const LuaValueList& args)
{
    LuaValueList rets;

    // if is a function, calls it
    if(isFunction()) {
        g_resources.pushCurrentPath(getPathDirectory(g_lua.getFunctionSource(shared_from_this())));

        try {
            push();
            g_lua.pushValues(args);
            int numRets = g_lua.safeCall(funcName, args.size());
            for(int i=0;i<numRets; ++i)
                rets.push_back(g_lua.popValue());
        } catch(LuaException &e) {
            logError(e.what());
        }

        g_resources.popCurrentPath();
    }
    // also calls a table of functions
    else if(isTable()) {
        LuaValueList functions = getTableValues();
        foreach(const LuaValuePtr& function, functions) {
            if(function->isFunction())
                function->call(funcName, args);
        }
    };

    return rets;
}

int LuaValue::getTableSize()
{
    int size = 0;
    push();
    assert(g_lua.isTable());
    g_lua.pushNil();
    while(g_lua.next()) {
        g_lua.pop();
        size++;
    }
    g_lua.pop();
    return size;
}

LuaValueList LuaValue::getTableValues()
{
    LuaValueList ret;
    push();
    assert(g_lua.isTable());
    g_lua.pushNil();
    while(g_lua.next())
        ret.push_back(g_lua.popValue());
    return ret;
}

LuaValuePtr LuaValue::getField(const std::string& key)
{
    push();
    assert(g_lua.isTable() || g_lua.isUserdata());
    g_lua.getField(key);
    g_lua.remove(-2);
    return g_lua.popValue();
}

void LuaValue::setField(const std::string& key, const LuaValuePtr& value)
{
    push();
    assert(g_lua.isTable() || g_lua.isUserdata());
    g_lua.pushValue(value);
    g_lua.setField(key);
    g_lua.pop();
}

LuaValuePtr LuaValue::getMetatable()
{
    push();
    assert(g_lua.isUserdata() || g_lua.isTable());
    g_lua.getMetatable();
    g_lua.remove(-2);
    return g_lua.popValue();
}

void LuaValue::setMetatable(const LuaValuePtr& mt)
{
    push();
    assert(g_lua.isUserdata() || g_lua.isTable());
    g_lua.pushValue(mt);
    g_lua.setMetatable();
    g_lua.pop();
}

void LuaValue::setTable(int n, const LuaValuePtr& value)
{
    push();
    assert(g_lua.isTable());
    g_lua.pushInteger(n);
    g_lua.pushValue(value);
    g_lua.setTable();
    g_lua.pop();
}

void LuaValue::setEnvironment(LuaValuePtr env)
{
    push();
    g_lua.pushValue(env);
    g_lua.setEnv();
    g_lua.pop();
}

std::string LuaValue::getTypeName()
{
    push();
    std::string typeName = g_lua.typeName();
    g_lua.pop();
    return typeName;
}

bool LuaValue::isBoolean()
{
    push();
    bool ret = g_lua.isBoolean();
    g_lua.pop();
    return ret;
}

bool LuaValue::isString()
{
    push();
    bool ret = g_lua.isString();
    g_lua.pop();
    return ret;
}

bool LuaValue::isNumber()
{
    push();
    bool ret = g_lua.isNumber();
    g_lua.pop();
    return ret;
}

bool LuaValue::isTable()
{
    push();
    bool ret = g_lua.isTable();
    g_lua.pop();
    return ret;
}

bool LuaValue::isFunction()
{
    push();
    bool ret = g_lua.isFunction();
    g_lua.pop();
    return ret;
}

bool LuaValue::isCFunction()
{
    push();
    bool ret = g_lua.isCFunction();
    g_lua.pop();
    return ret;
}

bool LuaValue::isLuaFunction()
{
    push();
    bool ret = g_lua.isLuaFunction();
    g_lua.pop();
    return ret;
}

bool LuaValue::isUserdata()
{
    push();
    bool ret = g_lua.isUserdata();
    g_lua.pop();
    return ret;
}

bool LuaValue::toBoolean()
{
    push();
    return g_lua.popBoolean();
}

int LuaValue::toInteger()
{
    push();
    return g_lua.popInteger();
}

double LuaValue::toNumber()
{
    push();
    return g_lua.popNumber();
}

std::string LuaValue::toString()
{
    push();
    return g_lua.popString();
}

void* LuaValue::toUserdata()
{
    push();
    return g_lua.popUserdata();
}

void LuaValue::push()
{
    g_lua.pushValue(shared_from_this());
}

