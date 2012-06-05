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
    virtual void draw(const Point& dest, float scaleFactor, bool animate) { }

    virtual void setId(uint32 id) { }
    void setPosition(const Position& position) { m_position = position; }

    virtual uint32 getId() { return 0; }
    Position getPosition() { return m_position; }
    int getStackPriority();
    const TilePtr& getTile();
    ContainerPtr getParentContainer();
    int getStackpos();

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

    virtual bool isItem() { return false; }
    virtual bool isEffect() { return false; }
    virtual bool isMissile() { return false; }
    virtual bool isCreature() { return false; }
    virtual bool isNpc() { return false; }
    virtual bool isMonster() { return false; }
    virtual bool isPlayer() { return false; }
    virtual bool isLocalPlayer() { return false; }
    virtual bool isAnimatedText() { return false; }
    virtual bool isStaticText() { return false; }

    // type related
    bool isGround() { return m_type->getProperty(ThingType::IsGround); }
    bool isFullGround() { return m_type->getProperty(ThingType::IsFullGround); }
    bool isTranslucent() { return m_type->getProperty(ThingType::IsTranslucent); }
    bool isGroundBorder() { return m_type->getProperty(ThingType::IsGroundBorder); }
    bool isOnBottom() { return m_type->getProperty(ThingType::IsOnBottom); }
    bool isOnTop() { return m_type->getProperty(ThingType::IsOnTop); }
    bool isDontHide() { return m_type->getProperty(ThingType::DontHide); }
    bool isContainer() { return m_type->getProperty(ThingType::IsContainer); }
    bool isForceUse() { return m_type->getProperty(ThingType::IsForceUse); }
    bool isMultiUse() { return m_type->getProperty(ThingType::IsMultiUse); }
    bool isRotateable() { return m_type->getProperty(ThingType::IsRotateable); }
    bool isNotMoveable() { return m_type->getProperty(ThingType::IsNotMovable); }
    bool isNotWalkable() { return m_type->getProperty(ThingType::NotWalkable); }
    bool isPickupable() { return m_type->getProperty(ThingType::IsPickupable); }
    bool isNotPathable() { return m_type->getProperty(ThingType::NotPathable); }
    bool isIgnoreLook() { return m_type->getProperty(ThingType::IgnoreLook); }
    bool isHangable() { return m_type->getProperty(ThingType::IsHangable); }
    bool isHookSouth() { return m_type->getProperty(ThingType::HookSouth); }
    bool isHookEast() { return m_type->getProperty(ThingType::HookEast); }
    bool isStackable() { return m_type->getProperty(ThingType::IsStackable); }
    bool isAnimateAlways() { return m_type->getProperty(ThingType::AnimateAlways); }
    bool isLyingCorpse() { return m_type->getProperty(ThingType::IsLyingCorpse); }
    bool blocksProjectile() { return m_type->getProperty(ThingType::BlockProjectile); }
    bool isFluid() { return m_type->getProperty(ThingType::IsFluid); }
    bool isFluidContainer() { return m_type->getProperty(ThingType::IsFluidContainer); }
    Size getDimension() { return Size(m_type->getDimension(ThingType::Width), m_type->getDimension(ThingType::Height)); }
    int getDimensionWidth() { return m_type->getDimension(ThingType::Width); }
    int getDimensionHeight() { return m_type->getDimension(ThingType::Height); }
    int getExactSize() { return m_type->getDimension(ThingType::ExactSize); }
    Point getDisplacement() { return Point(m_type->getParameter(ThingType::DisplacementX), m_type->getParameter(ThingType::DisplacementY)); }
    int getNumPatternsX() { return m_type->getDimension(ThingType::PatternX); }
    int getNumPatternsY() { return m_type->getDimension(ThingType::PatternY); }
    int getNumPatternsZ() { return m_type->getDimension(ThingType::PatternZ); }
    int getDisplacementX() { return m_type->getParameter(ThingType::DisplacementX); }
    int getDisplacementY() { return m_type->getParameter(ThingType::DisplacementY); }
    int getLayers() { return m_type->getDimension(ThingType::Layers); }
    int getAnimationPhases() { return m_type->getDimension(ThingType::AnimationPhases); }
    int getGroundSpeed() { return m_type->getParameter(ThingType::GroundSpeed); }
    int getElevation() { return m_type->getParameter(ThingType::Elevation); }
    int getMinimapColor() { return m_type->getParameter(ThingType::MiniMapColor); }

protected:
    Position m_position;
    ThingType *m_type;
};

#endif
