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
#include "thingstype.h"
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

    virtual void startAnimation() { }
    virtual void draw(const Point& dest, float scaleFactor) { }

    virtual void setId(uint32 id);
    virtual void setPosition(const Position& position) { m_position = position; }

    uint32 getId() { return m_id; }
    Position getPosition() { return m_position; }
    int getStackPriority();
    const TilePtr& getCurrentTile();

    void setXPattern(int xPattern) { m_xPattern = xPattern; }
    void setYPattern(int yPattern) { m_yPattern = yPattern; }
    void setZPattern(int zPattern) { m_zPattern = zPattern; }

    ThingPtr asThing() { return std::static_pointer_cast<Thing>(shared_from_this()); }
    virtual ItemPtr asItem() { return nullptr; }
    virtual EffectPtr asEffect() { return nullptr; }
    virtual MissilePtr asMissile() { return nullptr; }
    virtual CreaturePtr asCreature() { return nullptr; }
    virtual NpcPtr asNpc() { return nullptr; }
    virtual MonsterPtr asMonster() { return nullptr; }
    virtual PlayerPtr asPlayer() { return nullptr; }
    virtual LocalPlayerPtr asLocalPlayer() { return nullptr; }
    virtual AnimatedTextPtr asAnimatedText() { return nullptr; }
    virtual StaticTextPtr asStaticText() { return nullptr; }

    // type related
    bool isGround() { return m_type->properties[ThingType::IsGround]; }
    bool isFullGround() { return m_type->properties[ThingType::IsFullGround]; }
    bool isGroundBorder() { return m_type->properties[ThingType::IsGroundBorder]; }
    bool isOnBottom() { return m_type->properties[ThingType::IsOnBottom]; }
    bool isOnTop() { return m_type->properties[ThingType::IsOnTop]; }
    bool isDontHide() { return m_type->properties[ThingType::DontHide]; }
    bool isContainer() { return m_type->properties[ThingType::IsContainer]; }
    bool isForceUse() { return m_type->properties[ThingType::IsForceUse]; }
    bool isMultiUse() { return m_type->properties[ThingType::IsMultiUse]; }
    bool isRotateable() { return m_type->properties[ThingType::IsRotateable]; }
    bool isNotMoveable() { return m_type->properties[ThingType::IsNotMovable]; }
    bool isNotWalkable() { return m_type->properties[ThingType::NotWalkable]; }
    bool isPickupable() { return m_type->properties[ThingType::IsPickupable]; }
    bool isIgnoreLook() { return m_type->properties[ThingType::IgnoreLook]; }
    bool isHangable() { return m_type->properties[ThingType::IsHangable]; }
    bool isHookSouth() { return m_type->properties[ThingType::HookSouth]; }
    bool isHookEast() { return m_type->properties[ThingType::HookEast]; }
    bool isStackable() { return m_type->properties[ThingType::IsStackable]; }
    bool blocksProjectile() { return m_type->properties[ThingType::BlockProjectile]; }
    bool isFluid() { return m_type->properties[ThingType::IsFluid]; }
    bool isFluidContainer() { return m_type->properties[ThingType::IsFluidContainer]; }
    Size getDimension() { return Size(m_type->dimensions[ThingType::Width], m_type->dimensions[ThingType::Height]); }
    int getDimensionWidth() { return m_type->dimensions[ThingType::Width]; }
    int getDimensionHeight() { return m_type->dimensions[ThingType::Height]; }
    Point getDisplacement() { return Point(m_type->parameters[ThingType::DisplacementX], m_type->parameters[ThingType::DisplacementY]); }
    int getNumPatternsX() { return m_type->dimensions[ThingType::PatternX]; }
    int getNumPatternsY() { return m_type->dimensions[ThingType::PatternY]; }
    int getNumPatternsZ() { return m_type->dimensions[ThingType::PatternZ]; }
    int getDisplacementX() { return m_type->parameters[ThingType::DisplacementX]; }
    int getDisplacementY() { return m_type->parameters[ThingType::DisplacementY]; }
    int getLayers() { return m_type->dimensions[ThingType::Layers]; }
    int getAnimationPhases() { return m_type->dimensions[ThingType::AnimationPhases]; }
    int getGroundSpeed() { return m_type->parameters[ThingType::GroundSpeed]; }
    int getElevation() { return m_type->parameters[ThingType::Elevation]; }
    int getSpriteId(int w = 0, int h = 0, int layer = 0, int xPattern = 0, int yPattern = 0, int zPattern = 0, int animation = 0) { return m_type->getSpriteId(w, h, layer, xPattern, yPattern, zPattern, animation); }

protected:
    void internalDraw(const Point& dest, float scaleFactor, int layer);
    void updateType();

    uint32 m_id;
    Position m_position;
    int m_xPattern, m_yPattern, m_zPattern, m_animation;

private:
    ThingType *m_type;
};

#endif
