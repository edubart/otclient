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
    if(i == 0 && !g_lua.isNumber(index))
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
    if(d == 0 && !g_lua.isNumber(index))
        return false;
    return true;
}

// size_t
void push_luavalue(std::size_t s)
{
    push_luavalue((double)s);
}

bool luavalue_cast(int index, std::size_t& s)
{
    double d;
    bool ret = luavalue_cast(index, d);
    s = d;
    return ret;
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
    if(str.empty() && !g_lua.isString(index))
        return false;
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
        return fw::cast(g_lua.toString(index), color);
    } else if(g_lua.isNil()) {
        color = Color::white;
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
    } else if(g_lua.isString()) {
        return fw::cast(g_lua.toString(index), rect);
    } else if(g_lua.isNil()) {
        rect = Rect();
        return true;
    }
    return true;
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
        return fw::cast(g_lua.toString(index), point);
    } else if(g_lua.isNil()) {
        point = Point();
        return true;
    }
    return true;
}

// otml nodes
void push_luavalue(const OTMLNodePtr& node)
{
    if(node->hasValue()) {
        g_lua.pushString(node->value());
    } else if(node->hasChildren()) {
        g_lua.newTable();
        bool pushedChild = false;
        for(const OTMLNodePtr& cnode : node->children()) {
            if(cnode->isUnique()) {
                push_luavalue(cnode);
                if(!g_lua.isNil()) {
                    g_lua.setField(cnode->tag());
                    pushedChild = true;
                } else
                    g_lua.pop();
            }
        }
        if(!pushedChild) {
            g_lua.pop();
            g_lua.pushNil();
        }
    } else
        g_lua.pushNil();
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
