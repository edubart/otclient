#include "luaobject.h"
#include "luainterface.h"

LuaObject::LuaObject() : m_fieldsTableRef(-1)
{
}

LuaObject::~LuaObject()
{
    releaseLuaFieldsTable();
}

void LuaObject::releaseLuaFieldsTable()
{
    if(m_fieldsTableRef != -1) {
        g_lua.unref(m_fieldsTableRef);
        m_fieldsTableRef = -1;
    }
}

void LuaObject::luaSetField(const std::string& key)
{
    // create fields table on the fly
    if(m_fieldsTableRef == -1) {
        g_lua.newTable(); // create fields table
        m_fieldsTableRef = g_lua.ref(); // save a reference for it
    }

    g_lua.getRef(m_fieldsTableRef); // push the table
    g_lua.insert(-2); // move the value to the top
    g_lua.setField(key); // set the field
    g_lua.pop(); // pop the fields table
}

void LuaObject::luaGetField(const std::string& key)
{
    if(m_fieldsTableRef != -1) {
        g_lua.getRef(m_fieldsTableRef); // push the obj's fields table
        g_lua.getField(key); // push the field value
        g_lua.remove(-2); // remove the table
    } else {
        g_lua.pushNil();
    }
}

int LuaObject::getUseCount()
{
    return shared_from_this().use_count() - 1;
}
