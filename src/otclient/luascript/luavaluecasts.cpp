/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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

#include "luavaluecasts.h"
#include <framework/luascript/luainterface.h>

void push_luavalue(const Outfit& outfit)
{
    g_lua.newTable();
    g_lua.pushInteger(outfit.getType());
    g_lua.setField("type");
    g_lua.pushInteger(outfit.getAddons());
    g_lua.setField("addons");
    g_lua.pushInteger(outfit.getHead());
    g_lua.setField("head");
    g_lua.pushInteger(outfit.getBody());
    g_lua.setField("body");
    g_lua.pushInteger(outfit.getLegs());
    g_lua.setField("legs");
    g_lua.pushInteger(outfit.getFeet());
    g_lua.setField("feet");
}

bool luavalue_cast(int index, Outfit& outfit)
{
    if(g_lua.isTable(index)) {
        g_lua.getField("type", index);
        outfit.setType(g_lua.popInteger());
        g_lua.getField("addons", index);
        outfit.setAddons(g_lua.popInteger());
        g_lua.getField("head", index);
        outfit.setHead(g_lua.popInteger());
        g_lua.getField("body", index);
        outfit.setBody(g_lua.popInteger());
        g_lua.getField("legs", index);
        outfit.setLegs(g_lua.popInteger());
        g_lua.getField("feet", index);
        outfit.setFeet(g_lua.popInteger());
        return true;
    }
    return false;
}

void push_luavalue(const Position& pos)
{
    g_lua.newTable();
    g_lua.pushInteger(pos.x);
    g_lua.setField("x");
    g_lua.pushInteger(pos.y);
    g_lua.setField("y");
    g_lua.pushInteger(pos.z);
    g_lua.setField("z");
}

bool luavalue_cast(int index, Position& pos)
{
    if(g_lua.isTable(index)) {
        g_lua.getField("x", index);
        pos.x = g_lua.popInteger();
        g_lua.getField("y", index);
        pos.y = g_lua.popInteger();
        g_lua.getField("z", index);
        pos.z = g_lua.popInteger();
        return true;
    }
    return false;
}
