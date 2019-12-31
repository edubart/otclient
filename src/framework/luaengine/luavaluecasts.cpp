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

#include "luavaluecasts.h"
#include "luainterface.h"
#include <framework/otml/otmlnode.h>

// bool
int push_luavalue(bool b)
{
    g_lua.pushBoolean(b);
    return 1;
}

bool luavalue_cast(int index, bool& b)
{
    b = g_lua.toBoolean(index);
    return true;
}

// int
int push_luavalue(int i)
{
    g_lua.pushInteger(i);
    return 1;
}

bool luavalue_cast(int index, int& i)
{
    i = g_lua.toInteger(index);
    if(i == 0 && !g_lua.isNumber(index) && !g_lua.isNil())
        return false;
    return true;
}

// double
int push_luavalue(double d)
{
    g_lua.pushNumber(d);
    return 1;
}

bool luavalue_cast(int index, double& d)
{
    d = g_lua.toNumber(index);
    if(d == 0 && !g_lua.isNumber(index) && !g_lua.isNil())
        return false;
    return true;
}

// string
int push_luavalue(const char* cstr)
{
    g_lua.pushCString(cstr);
    return 1;
}

int push_luavalue(const std::string& str)
{
    g_lua.pushString(str);
    return 1;
}

bool luavalue_cast(int index, std::string& str)
{
    str = g_lua.toString(index);
    return true;
}

// lua cpp function
int push_luavalue(const LuaCppFunction& func)
{
    g_lua.pushCppFunction(func);
    return 1;
}

// color
int push_luavalue(const Color& color)
{
    g_lua.createTable(0, 4);
    g_lua.pushInteger(color.r());
    g_lua.setField("r");
    g_lua.pushInteger(color.g());
    g_lua.setField("g");
    g_lua.pushInteger(color.b());
    g_lua.setField("b");
    g_lua.pushInteger(color.a());
    g_lua.setField("a");
    return 1;
}

bool luavalue_cast(int index, Color& color)
{
    if(g_lua.isTable(index)) {
        g_lua.getField("r", index);
        color.setRed((int)g_lua.popInteger());
        g_lua.getField("g", index);
        color.setGreen((int)g_lua.popInteger());
        g_lua.getField("b", index);
        color.setBlue((int)g_lua.popInteger());
        g_lua.getField("a", index);
        color.setAlpha((int)g_lua.popInteger());
        return true;
    } else if(g_lua.isString()) {
        return stdext::cast(g_lua.toString(index), color);
    } else if(g_lua.isNil()) {
        color = Color::white;
        return true;
    }
    return false;
}

// rect
int push_luavalue(const Rect& rect)
{
    g_lua.createTable(0, 4);
    g_lua.pushInteger(rect.x());
    g_lua.setField("x");
    g_lua.pushInteger(rect.y());
    g_lua.setField("y");
    g_lua.pushInteger(rect.width());
    g_lua.setField("width");
    g_lua.pushInteger(rect.height());
    g_lua.setField("height");
    return 1;
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
        return stdext::cast(g_lua.toString(index), rect);
    } else if(g_lua.isNil()) {
        rect = Rect();
        return true;
    }
    return false;
}

// point
int push_luavalue(const Point& point)
{
    g_lua.createTable(0, 2);
    g_lua.pushInteger(point.x);
    g_lua.setField("x");
    g_lua.pushInteger(point.y);
    g_lua.setField("y");
    return 1;
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
        return stdext::cast(g_lua.toString(index), point);
    } else if(g_lua.isNil()) {
        point = Point();
        return true;
    }
    return false;
}

// size
int push_luavalue(const Size& size)
{
    g_lua.createTable(0, 2);
    g_lua.pushInteger(size.width());
    g_lua.setField("width");
    g_lua.pushInteger(size.height());
    g_lua.setField("height");
    return 1;
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
        return stdext::cast(g_lua.toString(index), size);
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
        union {
            bool b;
            double d;
            long l;
        };
        std::string value = node->rawValue();
        if(stdext::cast(value, b))
            g_lua.pushBoolean(b);
        else if(stdext::cast(value, l))
            g_lua.pushInteger(l);
        else if(stdext::cast(value, d))
            g_lua.pushNumber(d);
        else
            g_lua.pushString(value);
    } else if(node->hasChildren()) {
        g_lua.newTable();
        bool pushedChild = false;
        int currentIndex = 1;
        for(const OTMLNodePtr& cnode : node->children()) {
            push_otml_subnode_luavalue(cnode);
            if(!g_lua.isNil()) {
                if(cnode->isUnique()) {
                    g_lua.pushString(cnode->tag());
                    g_lua.insert(-2);
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

int push_luavalue(const OTMLNodePtr& node)
{
    if(node) {
        g_lua.newTable();
        int currentIndex = 1;
        for(const OTMLNodePtr& cnode : node->children()) {
            push_otml_subnode_luavalue(cnode);
            if(cnode->isUnique() && !cnode->tag().empty()) {
                g_lua.setField(cnode->tag());
            } else
                g_lua.rawSeti(currentIndex++);
        }
    } else
        g_lua.pushNil();
    return 1;
}

bool luavalue_cast(int index, OTMLNodePtr& node)
{
    node = OTMLNode::create();
    node->setUnique(true);
    if(g_lua.isTable(index)) {
        g_lua.pushNil();
        while(g_lua.next(index < 0 ? index-1 : index)) {
            std::string cnodeName;
            if(g_lua.isString(-2)) {
                g_lua.pushValue(-2);
                cnodeName = g_lua.toString();
                g_lua.pop();
            } else
                assert(g_lua.isNumber());
            if(g_lua.isTable()) {
                OTMLNodePtr cnode;
                if(luavalue_cast(-1, cnode)) {
                    if(cnodeName.empty())
                        cnode->setUnique(false);
                    else
                        cnode->setTag(cnodeName);
                    node->addChild(cnode);
                }
            } else {
                std::string value;
                if(g_lua.isBoolean())
                    value = stdext::unsafe_cast<std::string>(g_lua.toBoolean());
                else
                    value = g_lua.toString();
                if(cnodeName.empty())
                    node->writeIn(value);
                else
                    node->writeAt(cnodeName, value);
            }
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
