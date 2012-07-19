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
#include "thingtypedat.h"
#include "thingtypemanager.h"
#include <framework/luaengine/luaobject.h>

// @bindclass
#pragma pack(push,1) // disable memory alignment
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

    // type shortcuts
    virtual const ThingTypeDatPtr& getDatType();
    virtual ThingTypeDat *rawGetDatType();
    Size getSize() { return rawGetDatType()->getSize(); }
    int getWidth() { return rawGetDatType()->getWidth(); }
    int getHeight() { return rawGetDatType()->getHeight(); }
    Point getDisplacement() { return rawGetDatType()->getDisplacement(); }
    int getDisplacementX() { return rawGetDatType()->getDisplacementX(); }
    int getDisplacementY() { return rawGetDatType()->getDisplacementY(); }
    int getExactSize() { return rawGetDatType()->getExactSize(); }
    int getLayers() { return rawGetDatType()->getLayers(); }
    int getNumPatternX() { return rawGetDatType()->getNumPatternX(); }
    int getNumPatternY() { return rawGetDatType()->getNumPatternY(); }
    int getNumPatternZ() { return rawGetDatType()->getNumPatternZ(); }
    int getAnimationPhases() { return rawGetDatType()->getAnimationPhases(); }
    int getGroundSpeed() { return rawGetDatType()->getGroundSpeed(); }
    int getMaxTextLength() { return rawGetDatType()->getMaxTextLength(); }
    Light getLight() { return rawGetDatType()->getLight(); }
    int getMinimapColor() { return rawGetDatType()->getMinimapColor(); }
    int getLensHelp() { return rawGetDatType()->getLensHelp(); }
    int getClothSlot() { return rawGetDatType()->getClothSlot(); }
    int getElevation() { return rawGetDatType()->getElevation(); }
    bool isGround() { return rawGetDatType()->isGround(); }
    bool isGroundBorder() { return rawGetDatType()->isGroundBorder(); }
    bool isOnBottom() { return rawGetDatType()->isOnBottom(); }
    bool isOnTop() { return rawGetDatType()->isOnTop(); }
    bool isContainer() { return rawGetDatType()->isContainer(); }
    bool isStackable() { return rawGetDatType()->isStackable(); }
    bool isForceUse() { return rawGetDatType()->isForceUse(); }
    bool isMultiUse() { return rawGetDatType()->isMultiUse(); }
    bool isWritable() { return rawGetDatType()->isWritable(); }
    bool isChargeable() { return rawGetDatType()->isChargeable(); }
    bool isWritableOnce() { return rawGetDatType()->isWritableOnce(); }
    bool isFluidContainer() { return rawGetDatType()->isFluidContainer(); }
    bool isSplash() { return rawGetDatType()->isSplash(); }
    bool isNotWalkable() { return rawGetDatType()->isNotWalkable(); }
    bool isNotMoveable() { return rawGetDatType()->isNotMoveable(); }
    bool blockProjectile() { return rawGetDatType()->blockProjectile(); }
    bool isNotPathable() { return rawGetDatType()->isNotPathable(); }
    bool isPickupable() { return rawGetDatType()->isPickupable(); }
    bool isHangable() { return rawGetDatType()->isHangable(); }
    bool isHookSouth() { return rawGetDatType()->isHookSouth(); }
    bool isHookEast() { return rawGetDatType()->isHookEast(); }
    bool isRotateable() { return rawGetDatType()->isRotateable(); }
    bool hasLight() { return rawGetDatType()->hasLight(); }
    bool isDontHide() { return rawGetDatType()->isDontHide(); }
    bool isTranslucent() { return rawGetDatType()->isTranslucent(); }
    bool hasDisplacement() { return rawGetDatType()->hasDisplacement(); }
    bool hasElevation() { return rawGetDatType()->hasElevation(); }
    bool isLyingCorpse() { return rawGetDatType()->isLyingCorpse(); }
    bool isAnimateAlways() { return rawGetDatType()->isAnimateAlways(); }
    bool hasMiniMapColor() { return rawGetDatType()->hasMiniMapColor(); }
    bool hasLensHelp() { return rawGetDatType()->hasLensHelp(); }
    bool isFullGround() { return rawGetDatType()->isFullGround(); }
    bool isIgnoreLook() { return rawGetDatType()->isIgnoreLook(); }
    bool isCloth() { return rawGetDatType()->isCloth(); }
    bool isMarketable() { return rawGetDatType()->isMarketable(); }
    MarketData getMarketData() { return rawGetDatType()->getMarketData(); }


protected:
    Position m_position;
    uint16 m_datId;
};
#pragma pack(pop)

#endif

