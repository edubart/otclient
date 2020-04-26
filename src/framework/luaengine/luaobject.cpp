/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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

#include "luaobject.h"
#include "luainterface.h"

#include <typeinfo>
#include <framework/core/application.h>

LuaObject::LuaObject() :
    m_fieldsTableRef(-1)
{
}

LuaObject::~LuaObject()
{
#ifndef NDEBUG
    assert(!g_app.isTerminated());
#endif
    releaseLuaFieldsTable();
}

bool LuaObject::hasLuaField(const std::string& field)
{
    bool ret = false;
    if(m_fieldsTableRef != -1) {
        g_lua.getRef(m_fieldsTableRef);
        g_lua.getField(field); // push the field value
        ret = !g_lua.isNil();
        g_lua.pop(2);
    }
    return ret;
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

void LuaObject::luaGetMetatable()
{
    static std::unordered_map<const std::type_info*, int> metatableMap;
    const std::type_info& tinfo = typeid(*this);
    auto it = metatableMap.find(&tinfo);

    int metatableRef;
    if(it == metatableMap.end()) {
        g_lua.getGlobal(getClassName() + "_mt");
        metatableRef = g_lua.ref();
        metatableMap[&tinfo] = metatableRef;
    } else
        metatableRef = it->second;

    g_lua.getRef(metatableRef);
}

void LuaObject::luaGetFieldsTable()
{
    if(m_fieldsTableRef != -1)
        g_lua.getRef(m_fieldsTableRef);
    else
        g_lua.pushNil();
}

int LuaObject::getUseCount()
{
    return ref_count();
}

std::string LuaObject::getClassName()
{
    // TODO: this could be cached for more performance
#ifdef _MSC_VER
    return stdext::demangle_name(typeid(*this).name()) + 6;
#else
    return stdext::demangle_name(typeid(*this).name());
#endif
}
