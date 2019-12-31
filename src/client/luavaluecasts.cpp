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
#include <framework/luaengine/luainterface.h>

int push_luavalue(const Outfit& outfit)
{
    g_lua.createTable(0, 8);
    g_lua.pushInteger(outfit.getId());
    g_lua.setField("type");
    g_lua.pushInteger(outfit.getAuxId());
    g_lua.setField("auxType");
    if(g_game.getFeature(Otc::GamePlayerAddons)) {
        g_lua.pushInteger(outfit.getAddons());
        g_lua.setField("addons");
    }
    g_lua.pushInteger(outfit.getHead());
    g_lua.setField("head");
    g_lua.pushInteger(outfit.getBody());
    g_lua.setField("body");
    g_lua.pushInteger(outfit.getLegs());
    g_lua.setField("legs");
    g_lua.pushInteger(outfit.getFeet());
    g_lua.setField("feet");
    if(g_game.getFeature(Otc::GamePlayerMounts)) {
        g_lua.pushInteger(outfit.getMount());
        g_lua.setField("mount");
    }
    return 1;
}

bool luavalue_cast(int index, Outfit& outfit)
{
    if(g_lua.isTable(index)) {
        g_lua.getField("type", index);
        outfit.setId(g_lua.popInteger());
        g_lua.getField("auxType", index);
        outfit.setAuxId(g_lua.popInteger());
        if(g_game.getFeature(Otc::GamePlayerAddons)) {
            g_lua.getField("addons", index);
            outfit.setAddons(g_lua.popInteger());
        }
        g_lua.getField("head", index);
        outfit.setHead(g_lua.popInteger());
        g_lua.getField("body", index);
        outfit.setBody(g_lua.popInteger());
        g_lua.getField("legs", index);
        outfit.setLegs(g_lua.popInteger());
        g_lua.getField("feet", index);
        outfit.setFeet(g_lua.popInteger());
        if(g_game.getFeature(Otc::GamePlayerMounts)) {
            g_lua.getField("mount", index);
            outfit.setMount(g_lua.popInteger());
        }
        return true;
    }
    return false;
}

int push_luavalue(const Position& pos)
{
    if(pos.isValid()) {
        g_lua.createTable(0, 3);
        g_lua.pushInteger(pos.x);
        g_lua.setField("x");
        g_lua.pushInteger(pos.y);
        g_lua.setField("y");
        g_lua.pushInteger(pos.z);
        g_lua.setField("z");
    } else
        g_lua.pushNil();
    return 1;
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

int push_luavalue(const MarketData& data)
{
    g_lua.createTable(0, 6);
    g_lua.pushInteger(data.category);
    g_lua.setField("category");
    g_lua.pushString(data.name);
    g_lua.setField("name");
    g_lua.pushInteger(data.requiredLevel);
    g_lua.setField("requiredLevel");
    g_lua.pushInteger(data.restrictVocation);
    g_lua.setField("restrictVocation");
    g_lua.pushInteger(data.showAs);
    g_lua.setField("showAs");
    g_lua.pushInteger(data.tradeAs);
    g_lua.setField("tradeAs");
    return 1;
}

bool luavalue_cast(int index, MarketData& data)
{
    if(g_lua.isTable(index)) {
        g_lua.getField("category", index);
        data.category = g_lua.popInteger();
        g_lua.getField("name", index);
        data.name = g_lua.popString();
        g_lua.getField("requiredLevel", index);
        data.requiredLevel = g_lua.popInteger();
        g_lua.getField("restrictVocation", index);
        data.restrictVocation = g_lua.popInteger();
        g_lua.getField("showAs", index);
        data.showAs = g_lua.popInteger();
        g_lua.getField("tradeAs", index);
        data.tradeAs = g_lua.popInteger();
        return true;
    }
    return false;
}

int push_luavalue(const Light& light)
{
    g_lua.createTable(0, 2);
    g_lua.pushInteger(light.color);
    g_lua.setField("color");
    g_lua.pushInteger(light.intensity);
    g_lua.setField("intensity");
    return 1;
}

bool luavalue_cast(int index, Light& light)
{
    if(g_lua.isTable(index)) {
        g_lua.getField("color", index);
        light.color = g_lua.popInteger();
        g_lua.getField("intensity", index);
        light.intensity = g_lua.popInteger();
        return true;
    }
    return false;
}

int push_luavalue(const UnjustifiedPoints& unjustifiedPoints)
{
    g_lua.createTable(0, 7);
    g_lua.pushInteger(unjustifiedPoints.killsDay);
    g_lua.setField("killsDay");
    g_lua.pushInteger(unjustifiedPoints.killsDayRemaining);
    g_lua.setField("killsDayRemaining");
    g_lua.pushInteger(unjustifiedPoints.killsWeek);
    g_lua.setField("killsWeek");
    g_lua.pushInteger(unjustifiedPoints.killsWeekRemaining);
    g_lua.setField("killsWeekRemaining");
    g_lua.pushInteger(unjustifiedPoints.killsMonth);
    g_lua.setField("killsMonth");
    g_lua.pushInteger(unjustifiedPoints.killsMonthRemaining);
    g_lua.setField("killsMonthRemaining");
    g_lua.pushInteger(unjustifiedPoints.skullTime);
    g_lua.setField("skullTime");
    return 1;
}

bool luavalue_cast(int index, UnjustifiedPoints& unjustifiedPoints)
{
    if(g_lua.isTable(index)) {
        g_lua.getField("killsDay", index);
        unjustifiedPoints.killsDay = g_lua.popInteger();
        g_lua.getField("killsDayRemaining", index);
        unjustifiedPoints.killsDayRemaining = g_lua.popInteger();
        g_lua.getField("killsWeek", index);
        unjustifiedPoints.killsWeek = g_lua.popInteger();
        g_lua.getField("killsWeekRemaining", index);
        unjustifiedPoints.killsWeekRemaining = g_lua.popInteger();
        g_lua.getField("killsMonth", index);
        unjustifiedPoints.killsMonth = g_lua.popInteger();
        g_lua.getField("killsMonthRemaining", index);
        unjustifiedPoints.killsMonthRemaining = g_lua.popInteger();
        g_lua.getField("skullTime", index);
        unjustifiedPoints.skullTime = g_lua.popInteger();
        return true;
    }
    return false;
}
