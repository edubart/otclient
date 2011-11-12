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
#include "thingtype.h"
#include <framework/luascript/luaobject.h>

struct Light
{
    uint8 intensity;
    uint8 color;
};

class Thing : public LuaObject
{
public:
    Thing();
    virtual ~Thing() { }

    virtual void draw(const Point& p) = 0;

    void setId(uint32 id);
    void setPosition(const Position& position);

    uint32 getId() const { return m_id; }
    Position getPosition() const { return m_position; }
    int getStackPriority();
    virtual const ThingType& getType() = 0;
    int getAnimationPhases() { return getType().dimensions[ThingType::AnimationPhases]; }

    virtual void onIdChange(int) {}
    virtual void onPositionChange(const Position&) {}

    ThingPtr asThing() { return std::static_pointer_cast<Thing>(shared_from_this()); }
    virtual ItemPtr asItem() { return nullptr; }
    virtual CreaturePtr asCreature() { return nullptr; }
    virtual EffectPtr asEffect() { return nullptr; }
    virtual MissilePtr asMissile() { return nullptr; }
    virtual PlayerPtr asPlayer() { return nullptr; }
    virtual LocalPlayerPtr asLocalPlayer() { return nullptr; }

protected:
    void internalDraw(const Point& p, int layers, Otc::SpriteMask mask = Otc::SpriteNoMask);

    uint32 m_id;
    Position m_position;

    int m_xPattern, m_yPattern, m_zPattern, m_animation;
};

#endif
