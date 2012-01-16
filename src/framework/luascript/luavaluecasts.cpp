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
#include "luainterface.h"
#include <framework/otml/otmlnode.h>

// bool
void push_luavalue(bool b)
{
    g_lua.pushBoolean(b);
}

bool luavalue_cast(int index, bool& b)
{
    b = g_lua.toBoolean(index);
    return true;
}

// int
void push_luavalue(int i)
{
    g_lua.pushInteger(i);
}

bool luavalue_cast(int index, int& i)
{
    i = g_lua.toInteger(index);
    if(i == 0 && !g_lua.isNumber(index) && !g_lua.isNil())
        return false;
    return true;
}

// double
void push_luavalue(double d)
{
    g_lua.pushNumber(d);
}

bool luavalue_cast(int index, double& d)
{
    d = g_lua.toNumber(index);
    if(d == 0 && !g_lua.isNumber(index) && !g_lua.isNil())
        return false;
    return true;
}

// string
void push_luavalue(const char* cstr)
{
    g_lua.pushCString(cstr);
}

void push_luavalue(const std::string& str)
{
    g_lua.pushString(str);
}

bool luavalue_cast(int index, std::string& str)
{
    str = g_lua.toString(index);
    return true;
}

// lua cpp function
void push_luavalue(const LuaCppFunction& func)
{
    g_lua.pushCppFunction(func);
}

// color
void push_luavalue(const Color& color)
{
    g_lua.newTable();
    g_lua.pushInteger(color.r());
    g_lua.setField("r");
    g_lua.pushInteger(color.g());
    g_lua.setField("g");
    g_lua.pushInteger(color.b());
    g_lua.setField("b");
    g_lua.pushInteger(color.a());
    g_lua.setField("a");
}

bool luavalue_cast(int index, Color& color)
{
    if(g_lua.isTable(index)) {
        g_lua.getField("r", index);
        color.setRed(g_lua.popInteger());
        g_lua.getField("g", index);
        color.setGreen(g_lua.popInteger());
        g_lua.getField("b", index);
        color.setBlue(g_lua.popInteger());
        g_lua.getField("a", index);
        color.setAlpha(g_lua.popInteger());
        return true;
    } else if(g_lua.isString()) {
        return Fw::cast(g_lua.toString(index), color);
    } else if(g_lua.isNil()) {
        color = Fw::white;
        return true;
    }
    return false;
}

// rect
void push_luavalue(const Rect& rect)
{
    g_lua.newTable();
    g_lua.pushInteger(rect.x());
    g_lua.setField("x");
    g_lua.pushInteger(rect.y());
    g_lua.setField("y");
    g_lua.pushInteger(rect.width());
    g_lua.setField("width");
    g_lua.pushInteger(rect.height());
    g_lua.setField("height");
}

bool luavalue_cast(int index, Rect& rect)
{
    if(g_lua.isTable(index)) {
        g_lua.getField("x", index);
        rect.setX(g_lua.popInteger());
        g_lua.getField("y", index);
        rect.setY(g_lua.popInteger());
        g_lua.getField("width", index);
        rect.setWidth(g_lua.popInteger());
        g_lua.getField("height", index);
        rect.setHeight(g_lua.popInteger());
        return true;
    } else if(g_lua.isString()) {
        return Fw::cast(g_lua.toString(index), rect);
    } else if(g_lua.isNil()) {
        rect = Rect();
        return true;
    }
    return false;
}

// point
void push_luavalue(const Point& point)
{
    g_lua.newTable();
    g_lua.pushInteger(point.x);
    g_lua.setField("x");
    g_lua.pushInteger(point.y);
    g_lua.setField("y");
}

bool luavalue_cast(int index, Point& point)
{
    if(g_lua.isTable(index)) {
        g_lua.getField("x", index);
        point.x = g_lua.popInteger();
        g_lua.getField("y", index);
        point.y = g_lua.popInteger();
        return true;
    } else if(g_lua.isString()) {
        return Fw::cast(g_lua.toString(index), point);
    } else if(g_lua.isNil()) {
        point = Point();
        return true;
    }
    return false;
}

// size
void push_luavalue(const Size& size)
{
    g_lua.newTable();
    g_lua.pushInteger(size.width());
    g_lua.setField("width");
    g_lua.pushInteger(size.height());
    g_lua.setField("height");
}

bool luavalue_cast(int index, Size& size)
{
    if(g_lua.isTable(index)) {
        g_lua.getField("width", index);
        size.setWidth(g_lua.popInteger());
        g_lua.getField("height", index);
        size.setHeight(g_lua.popInteger());
        return true;
    } else if(g_lua.isString()) {
        return Fw::cast(g_lua.toString(index), size);
    } else if(g_lua.isNil()) {
        size = Size();
        return true;
    }
    return false;
}

// otml nodes
void push_otml_subnode_luavalue(const OTMLNodePtr& node)
{
    if(node->hasValue()) {
        g_lua.pushString(node->value());
    } else if(node->hasChildren()) {
        g_lua.newTable();
        bool pushedChild = false;
        int currentIndex = 1;
        for(const OTMLNodePtr& cnode : node->children()) {
            push_otml_subnode_luavalue(cnode);
            if(!g_lua.isNil()) {
                if(cnode->isUnique()) {
                    g_lua.pushString(cnode->tag());
                    g_lua.rawSet();
                } else
                    g_lua.rawSeti(currentIndex++);
                pushedChild = true;
            } else
                g_lua.pop();
        }
        if(!pushedChild) {
            g_lua.pop();
            g_lua.pushNil();
        }
    } else
        g_lua.pushNil();
}

void push_luavalue(const OTMLNodePtr& node)
{
    g_lua.newTable();
    for(const OTMLNodePtr& cnode : node->children()) {
        if(cnode->isUnique()) {
            push_otml_subnode_luavalue(cnode);
            if(!g_lua.isNil()) {
                g_lua.setField(cnode->tag());
            } else
                g_lua.pop();
        }
    }
}

bool luavalue_cast(int index, OTMLNodePtr& node)
{
    node = OTMLNode::create();
    node->setUnique(true);
    if(g_lua.isTable(index)) {
        g_lua.pushNil();
        while(g_lua.next(index < 0 ? index-1 : index)) {
            std::string cnodeName = g_lua.toString(-2);
            if(g_lua.isTable()) {
                OTMLNodePtr cnode;
                if(luavalue_cast(-1, node)) {
                    cnode->setTag(cnodeName);
                    node->addChild(cnode);
                }
            } else
                node->writeAt(cnodeName, g_lua.toString());
            g_lua.pop();
        }
        return true;
    }
    return false;
}

// object ptr
bool luavalue_cast(int index, LuaObjectPtr& obj) {
    if(g_lua.isUserdata(index)) {
        obj = g_lua.toObject(index);
        return true;
    } else if(g_lua.isNil(index)) {
        obj = nullptr;
        return true;
    }
    return false;
}
