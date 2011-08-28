/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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

#ifndef TILE_H
#define TILE_H

#include "declarations.h"
#include <framework/luascript/luaobject.h>

class Tile : public LuaObject
{
public:
    Tile();

    void draw(int x, int y, int step);

    void addThing(ThingPtr thing, int stackpos);
    ThingPtr getThing(unsigned int stackpos);
    void removeThing(unsigned int stackpos);
    void removeThingByPtr(ThingPtr thing);

    void clean();

    bool hasGround() { return (!!m_ground); }

    int getStackSize(int stop);

    const ThingList& getCreatures() { return m_creatures; }
    int getDrawNextOffset() { return m_drawNextOffset; }

private:
    ThingPtr m_ground;
    ThingList m_itemsBottom;
    ThingList m_creatures;
    ThingList m_itemsTop;
    ThingList m_effects;

    int m_drawNextOffset;
};

#endif
