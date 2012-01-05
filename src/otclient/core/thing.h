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

    virtual void start() {}

    virtual void draw(const Point& p, const Rect&) = 0;

    void setId(int id);
    virtual void setPos(const Position& position) { m_position = position; }

    int getId() const { return m_id; }
    Position getPos() const { return m_position; }
    int getStackPriority();
    virtual ThingType *getType();
    int getAnimationPhases() { return m_type->dimensions[ThingType::AnimationPhases]; }

    void setXPattern(int xPattern) { m_xPattern = xPattern; }
    void setYPattern(int yPattern) { m_yPattern = yPattern; }
    void setZPattern(int zPattern) { m_zPattern = zPattern; }

    ThingPtr asThing() { return std::static_pointer_cast<Thing>(shared_from_this()); }
    virtual ItemPtr asItem() { return nullptr; }
    virtual CreaturePtr asCreature() { return nullptr; }
    virtual EffectPtr asEffect() { return nullptr; }
    virtual MissilePtr asMissile() { return nullptr; }
    virtual PlayerPtr asPlayer() { return nullptr; }
    virtual LocalPlayerPtr asLocalPlayer() { return nullptr; }
    virtual AnimatedTextPtr asAnimatedText() { return nullptr; }
    virtual StaticTextPtr asStaticText() { return nullptr; }

    bool isGround();
    bool isGroundBorder();
    bool isOnBottom();
    bool isOnTop();
    bool isContainer();
    bool isForceUse();
    bool isMultiUse();
    bool isRotateable();
    bool isNotMoveable();
    bool isPickupable();
    bool ignoreLook();

protected:
    void internalDraw(const Point& p, int layer);

    uint32 m_id;
    Position m_position;
    ThingType *m_type;

    int m_xPattern, m_yPattern, m_zPattern, m_animation;
};

#endif
