#include "luaobject.h"
#include "luainterface.h"

LuaObject::LuaObject()
{
    // creates own table
    m_luaTable = g_lua.createTable();
}

LuaObject::~LuaObject()
{

}

LuaValuePtr LuaObject::toLuaValue()
{
    // fills a new userdata with a new LuaObject pointer
    new(g_lua.newUserdata(sizeof(LuaObjectPtr))) LuaObjectPtr(shared_from_this());

    // set the metatable for the userdata
    g_lua.setMetatable(make_string(getLuaTypeName(), "_mt"));

    return g_lua.popValue();
}
