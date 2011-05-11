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
#include <script/scriptable.h>
#include <script/luascript.h>
#include <core/dispatcher.h>

int Scriptable::getLuaTableRef()
{
    if(m_luaTableRef == -1) {
        g_lua.newTable();
        m_luaTableRef = g_lua.popRef();
    }
    return m_luaTableRef;
}

void Scriptable::releaseLuaTableRef()
{
    if(m_luaTableRef != -1) {
        g_lua.releaseRef(m_luaTableRef);
        m_luaTableRef = -1;
    }
}

void Scriptable::callLuaTableField(const std::string& field)
{
    // set self
    g_lua.pushClassInstance(shared_from_this());
    g_lua.setGlobal("self");

    // push field
    g_lua.getScriptableField(shared_from_this(), field);

    // call it if its a function
    if(g_lua.isFunction()) {
        g_lua.callFunction();
    // if its an array call each element
    } else if(g_lua.isTable()) {
        g_lua.pushNil();
        while(g_lua.next()) {
            // call it if its a function
            if(g_lua.isFunction())
                g_lua.callFunction();
            g_lua.pop();
        }
    } else if(!g_lua.isNil()) {
        g_lua.reportError(fmt("field '%s' for '%s' is not a valid function or array of functions", field %  getScriptableName()));
    }

    // release self
    g_lua.pushNil();
    g_lua.setGlobal("self");
}

