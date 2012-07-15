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
#include "mapview.h"
#include "thingtypedat.h"
#include <framework/luascript/luaobject.h>

struct Light
{
    uint8 intensity;
    uint8 color;
};

// @bindclass
class Thing : public LuaObject
{
public:
    Thing();
    virtual ~Thing() { }

    virtual void startAnimation() { }
    virtual void draw(const Point& dest, float scaleFactor, bool animate) { }
    virtual void drawLight(const Point& dest, float scaleFactor, bool animate, MapView* mapview) { }

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

    // type shortcuts
    ThingTypeDatPtr getType() { return m_datType; }
    Size getSize() { return m_datType->getSize(); }
    int getWidth() { return m_datType->getWidth(); }
    int getHeight() { return m_datType->getHeight(); }
    Point getDisplacement() { return m_datType->getDisplacement(); }
    int getDisplacementX() { return m_datType->getDisplacementX(); }
    int getDisplacementY() { return m_datType->getDisplacementY(); }
    int getExactSize() { return m_datType->getExactSize(); }
    int getLayers() { return m_datType->getLayers(); }
    int getNumPatternX() { return m_datType->getNumPatternX(); }
    int getNumPatternY() { return m_datType->getNumPatternY(); }
    int getNumPatternZ() { return m_datType->getNumPatternZ(); }
    int getAnimationPhases() { return m_datType->getAnimationPhases(); }
    int getGroundSpeed() { return m_datType->getGroundSpeed(); }
    int getMaxTextLength() { return m_datType->getMaxTextLength(); }
    int getLightLevel() { return m_datType->getLightLevel(); }
    int getLightColor() { return m_datType->getLightColor(); }
    int getMinimapColor() { return m_datType->getMinimapColor(); }
    int getLensHelp() { return m_datType->getLensHelp(); }
    int getClothSlot() { return m_datType->getClothSlot(); }
    int getElevation() { return m_datType->getElevation(); }
    bool isGround() { return m_datType->isGround(); }
    bool isGroundBorder() { return m_datType->isGroundBorder(); }
    bool isOnBottom() { return m_datType->isOnBottom(); }
    bool isOnTop() { return m_datType->isOnTop(); }
    bool isContainer() { return m_datType->isContainer(); }
    bool isStackable() { return m_datType->isStackable(); }
    bool isForceUse() { return m_datType->isForceUse(); }
    bool isMultiUse() { return m_datType->isMultiUse(); }
    bool isWritable() { return m_datType->isWritable(); }
    bool isWritableOnce() { return m_datType->isWritableOnce(); }
    bool isFluidContainer() { return m_datType->isFluidContainer(); }
    bool isFluid() { return m_datType->isFluid(); }
    bool isNotWalkable() { return m_datType->isNotWalkable(); }
    bool isNotMoveable() { return m_datType->isNotMoveable(); }
    bool blockProjectile() { return m_datType->blockProjectile(); }
    bool isNotPathable() { return m_datType->isNotPathable(); }
    bool isPickupable() { return m_datType->isPickupable(); }
    bool isHangable() { return m_datType->isHangable(); }
    bool isHookSouth() { return m_datType->isHookSouth(); }
    bool isHookEast() { return m_datType->isHookEast(); }
    bool isRotateable() { return m_datType->isRotateable(); }
    bool hasLight() { return m_datType->hasLight(); }
    bool isDontHide() { return m_datType->isDontHide(); }
    bool isTranslucent() { return m_datType->isTranslucent(); }
    bool hasDisplacement() { return m_datType->hasDisplacement(); }
    bool hasElevation() { return m_datType->hasElevation(); }
    bool isLyingCorpse() { return m_datType->isLyingCorpse(); }
    bool isAnimateAlways() { return m_datType->isAnimateAlways(); }
    bool hasMiniMapColor() { return m_datType->hasMiniMapColor(); }
    bool hasLensHelp() { return m_datType->hasLensHelp(); }
    bool isFullGround() { return m_datType->isFullGround(); }
    bool isIgnoreLook() { return m_datType->isIgnoreLook(); }
    bool isCloth() { return m_datType->isCloth(); }

protected:
    Position m_position;
    ThingTypeDatPtr m_datType;
};

#endif

