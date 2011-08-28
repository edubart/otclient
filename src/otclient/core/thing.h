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

#ifndef THING_H
#define THING_H

#include "declarations.h"
#include "thingattributes.h"
#include <framework/luascript/luaobject.h>

struct Light
{
    uint8 intensity;
    uint8 color;
};

class Thing : public LuaObject
{
public:
    Thing(ThingType type);
    virtual ~Thing() { }

    virtual void draw(int x, int y) = 0;

    void setId(uint32 id) { m_id = id; }
    void setPosition(const Position& position) { m_position = position; }

    uint32 getId() const { return m_id; }
    ThingType getType() const { return m_type; }
    Position getPosition() const { return m_position; }
    virtual const ThingAttributes& getAttributes() = 0;

    ThingPtr asThing() { return std::static_pointer_cast<Thing>(shared_from_this()); }
    virtual ItemPtr asItem() { return nullptr; }
    virtual CreaturePtr asCreature() { return nullptr; }
    virtual EffectPtr asEffect() { return nullptr; }
    virtual PlayerPtr asPlayer() { return nullptr; }
    virtual LocalPlayerPtr asLocalPlayer() { return nullptr; }

protected:
    void internalDraw(int x, int y, int blendframes, int xdiv, int ydiv, int zdiv, int anim, SpriteMask mask = SpriteMaskNone);

    uint32 m_id;
    ThingType m_type;
    Position m_position;
};

#endif
