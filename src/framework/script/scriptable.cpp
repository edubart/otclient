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

void Scriptable::associateLuaRef(const std::string& refName, int refId)
{
    // check if there is already a ref with this name
    if(m_luaRefs.find(refName) != m_luaRefs.end())
        g_lua.unref(m_luaRefs[refName]);
    m_luaRefs[refName] = refId;
}

int Scriptable::getLuaRef(const std::string& refName)
{
    if(m_luaRefs.find(refName) != m_luaRefs.end())
        return m_luaRefs[refName];
    return -1;
}

void Scriptable::clearLuaRefs()
{
    foreach(auto pair, m_luaRefs) {
        g_lua.unref(pair.second);
    }
    m_luaRefs.clear();
}
