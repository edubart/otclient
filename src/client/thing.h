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

#ifndef THING_H
#define THING_H

#include "declarations.h"
#include "thingtype.h"
#include "thingtypemanager.h"
#include <framework/luaengine/luaobject.h>

// @bindclass
#pragma pack(push,1) // disable memory alignment
class Thing : public LuaObject
{
public:
    Thing();
    virtual ~Thing() { }

    virtual void draw(const Point& /*dest*/, float /*scaleFactor*/, bool /*animate*/, LightView* /*lightView*/ = nullptr) { }

    virtual void setId(uint32 /*id*/) { }
    void setPosition(const Position& position);

    virtual uint32 getId() { return 0; }
    Position getPosition() { return m_position; }
    int getStackPriority();
    const TilePtr& getTile();
    ContainerPtr getParentContainer();
    int getStackPos();

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
    virtual const ThingTypePtr& getThingType();
    virtual ThingType *rawGetThingType();
    Size getSize() { return rawGetThingType()->getSize(); }
    int getWidth() { return rawGetThingType()->getWidth(); }
    int getHeight() { return rawGetThingType()->getHeight(); }
    virtual Point getDisplacement() { return rawGetThingType()->getDisplacement(); }
    virtual int getDisplacementX() { return rawGetThingType()->getDisplacementX(); }
    virtual int getDisplacementY() { return rawGetThingType()->getDisplacementY(); }
    virtual int getExactSize(int layer, int xPattern, int yPattern, int zPattern, int animationPhase) { return rawGetThingType()->getExactSize(layer, xPattern, yPattern, zPattern, animationPhase); }
    int getLayers() { return rawGetThingType()->getLayers(); }
    int getNumPatternX() { return rawGetThingType()->getNumPatternX(); }
    int getNumPatternY() { return rawGetThingType()->getNumPatternY(); }
    int getNumPatternZ() { return rawGetThingType()->getNumPatternZ(); }
    int getAnimationPhases() { return rawGetThingType()->getAnimationPhases(); }
    AnimatorPtr getAnimator() { return rawGetThingType()->getAnimator(); }
    int getGroundSpeed() { return rawGetThingType()->getGroundSpeed(); }
    int getMaxTextLength() { return rawGetThingType()->getMaxTextLength(); }
    Light getLight() { return rawGetThingType()->getLight(); }
    int getMinimapColor() { return rawGetThingType()->getMinimapColor(); }
    int getLensHelp() { return rawGetThingType()->getLensHelp(); }
    int getClothSlot() { return rawGetThingType()->getClothSlot(); }
    int getElevation() { return rawGetThingType()->getElevation(); }
    bool isGround() { return rawGetThingType()->isGround(); }
    bool isGroundBorder() { return rawGetThingType()->isGroundBorder(); }
    bool isOnBottom() { return rawGetThingType()->isOnBottom(); }
    bool isOnTop() { return rawGetThingType()->isOnTop(); }
    bool isContainer() { return rawGetThingType()->isContainer(); }
    bool isStackable() { return rawGetThingType()->isStackable(); }
    bool isForceUse() { return rawGetThingType()->isForceUse(); }
    bool isMultiUse() { return rawGetThingType()->isMultiUse(); }
    bool isWritable() { return rawGetThingType()->isWritable(); }
    bool isChargeable() { return rawGetThingType()->isChargeable(); }
    bool isWritableOnce() { return rawGetThingType()->isWritableOnce(); }
    bool isFluidContainer() { return rawGetThingType()->isFluidContainer(); }
    bool isSplash() { return rawGetThingType()->isSplash(); }
    bool isNotWalkable() { return rawGetThingType()->isNotWalkable(); }
    bool isNotMoveable() { return rawGetThingType()->isNotMoveable(); }
    bool blockProjectile() { return rawGetThingType()->blockProjectile(); }
    bool isNotPathable() { return rawGetThingType()->isNotPathable(); }
    bool isPickupable() { return rawGetThingType()->isPickupable(); }
    bool isHangable() { return rawGetThingType()->isHangable(); }
    bool isHookSouth() { return rawGetThingType()->isHookSouth(); }
    bool isHookEast() { return rawGetThingType()->isHookEast(); }
    bool isRotateable() { return rawGetThingType()->isRotateable(); }
    bool hasLight() { return rawGetThingType()->hasLight(); }
    bool isDontHide() { return rawGetThingType()->isDontHide(); }
    bool isTranslucent() { return rawGetThingType()->isTranslucent(); }
    bool hasDisplacement() { return rawGetThingType()->hasDisplacement(); }
    bool hasElevation() { return rawGetThingType()->hasElevation(); }
    bool isLyingCorpse() { return rawGetThingType()->isLyingCorpse(); }
    bool isAnimateAlways() { return rawGetThingType()->isAnimateAlways(); }
    bool hasMiniMapColor() { return rawGetThingType()->hasMiniMapColor(); }
    bool hasLensHelp() { return rawGetThingType()->hasLensHelp(); }
    bool isFullGround() { return rawGetThingType()->isFullGround(); }
    bool isIgnoreLook() { return rawGetThingType()->isIgnoreLook(); }
    bool isCloth() { return rawGetThingType()->isCloth(); }
    bool isMarketable() { return rawGetThingType()->isMarketable(); }
    bool isUsable() { return rawGetThingType()->isUsable(); }
    bool isWrapable() { return rawGetThingType()->isWrapable(); }
    bool isUnwrapable() { return rawGetThingType()->isUnwrapable(); }
    bool isTopEffect() { return rawGetThingType()->isTopEffect(); }
    MarketData getMarketData() { return rawGetThingType()->getMarketData(); }

    virtual void onPositionChange(const Position& /*newPos*/, const Position& /*oldPos*/) { }
    virtual void onAppear() { }
    virtual void onDisappear() { }

protected:
    Position m_position;
    uint16 m_datId;
};
#pragma pack(pop)

#endif

