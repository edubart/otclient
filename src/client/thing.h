/*
 * Copyright (c) 2010-2022 OTClient <https://github.com/edubart/otclient>
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

#pragma once

#include "declarations.h"
#include "thingtype.h"
#include "thingtypemanager.h"
#include <framework/luaengine/luaobject.h>

struct Highlight
{
    int fadeLevel;
    Color rgbColor = Color::alpha;
    ThingPtr thing;
    ScheduledEventPtr listeningEvent;

    bool enabled{ false },
        update{ false },
        invertedColorSelection{ false };
};

// @bindclass
#pragma pack(push,1) // disable memory alignment
class Thing : public LuaObject
{
public:
    ~Thing() override = default;
    virtual void draw(const Point& /*dest*/, float /*scaleFactor*/, bool /*animate*/, const Highlight& /*highLight*/, TextureType /*textureType*/ = TextureType::NONE, Color /* color */ = Color::white, LightView* /*lightView*/ = nullptr) {}
    virtual void setId(uint32_t /*id*/) {}

    void setPosition(const Position& position);

    virtual uint32_t getId() { return 0; }
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
    Size getSize() { return getThingType()->getSize(); }
    int getWidth() { return getThingType()->getWidth(); }
    int getHeight() { return getThingType()->getHeight(); }
    int getRealSize() { return getThingType()->getRealSize(); }
    virtual Point getDisplacement() { return getThingType()->getDisplacement(); }
    virtual int getDisplacementX() { return getThingType()->getDisplacementX(); }
    virtual int getDisplacementY() { return getThingType()->getDisplacementY(); }

    virtual int getExactSize() { return getThingType()->getExactSize(0, 0, 0, 0, 0); }
    virtual int getExactSize(int layer, int xPattern, int yPattern, int zPattern, int animationPhase) { return getThingType()->getExactSize(layer, xPattern, yPattern, zPattern, animationPhase); }
    int getLayers() { return getThingType()->getLayers(); }
    int getNumPatternX() { return getThingType()->getNumPatternX(); }
    int getNumPatternY() { return getThingType()->getNumPatternY(); }
    int getNumPatternZ() { return getThingType()->getNumPatternZ(); }
    int getAnimationPhases() { return getThingType()->getAnimationPhases(); }
    bool hasAnimationPhases() { return getThingType()->getAnimationPhases() > 1; }
    AnimatorPtr getAnimator() { return getThingType()->getAnimator(); }
    AnimatorPtr getIdleAnimator() { return getThingType()->getIdleAnimator(); }
    int getGroundSpeed() { return getThingType()->getGroundSpeed(); }
    int getMaxTextLength() { return getThingType()->getMaxTextLength(); }
    virtual Light getLight() { return getThingType()->getLight(); }
    virtual bool hasLight() { return getThingType()->hasLight(); }
    int getMinimapColor() { return getThingType()->getMinimapColor(); }
    int getLensHelp() { return getThingType()->getLensHelp(); }
    int getClothSlot() { return getThingType()->getClothSlot(); }
    int getElevation() { return getThingType()->getElevation(); }
    bool isGround() { return getThingType()->isGround(); }
    bool isGroundBorder() { return getThingType()->isGroundBorder(); }
    bool isTopGround() { return getThingType()->isTopGround(); }
    bool isTopGroundBorder() { return getThingType()->isTopGroundBorder(); }
    bool isSingleGround() { return getThingType()->isSingleGround(); }
    bool isSingleGroundBorder() { return getThingType()->isSingleGroundBorder(); }
    bool isOnBottom() { return getThingType()->isOnBottom(); }
    bool isOnTop() { return getThingType()->isOnTop(); }
    bool isCommon() { return !isGround() && !isGroundBorder() && !isOnTop() && !isCreature() && !isOnBottom(); }
    virtual bool isContainer() { return getThingType()->isContainer(); }
    bool isStackable() { return getThingType()->isStackable(); }
    bool isForceUse() { return getThingType()->isForceUse(); }
    bool isMultiUse() { return getThingType()->isMultiUse(); }
    bool isWritable() { return getThingType()->isWritable(); }
    bool isChargeable() { return getThingType()->isChargeable(); }
    bool isWritableOnce() { return getThingType()->isWritableOnce(); }
    bool isFluidContainer() { return getThingType()->isFluidContainer(); }
    bool isSplash() { return getThingType()->isSplash(); }
    bool isNotWalkable() { return getThingType()->isNotWalkable(); }
    bool isNotMoveable() { return getThingType()->isNotMoveable(); }
    bool isMoveable() { return !getThingType()->isNotMoveable(); }
    bool blockProjectile() { return getThingType()->blockProjectile(); }
    bool isNotPathable() { return getThingType()->isNotPathable(); }
    bool isPickupable() { return getThingType()->isPickupable(); }
    bool isHangable() { return getThingType()->isHangable(); }
    bool isHookSouth() { return getThingType()->isHookSouth(); }
    bool isHookEast() { return getThingType()->isHookEast(); }
    bool isRotateable() { return getThingType()->isRotateable(); }
    bool isDontHide() { return getThingType()->isDontHide(); }
    bool isTranslucent() { return getThingType()->isTranslucent(); }
    bool hasDisplacement() { return getThingType()->hasDisplacement(); }
    bool hasElevation() { return getThingType()->hasElevation(); }
    bool isLyingCorpse() { return getThingType()->isLyingCorpse(); }
    bool isAnimateAlways() { return getThingType()->isAnimateAlways(); }
    bool hasMiniMapColor() { return getThingType()->hasMiniMapColor(); }
    bool hasLensHelp() { return getThingType()->hasLensHelp(); }
    bool isFullGround() { return getThingType()->isFullGround(); }
    bool isIgnoreLook() { return getThingType()->isIgnoreLook(); }
    bool isCloth() { return getThingType()->isCloth(); }
    bool isMarketable() { return getThingType()->isMarketable(); }
    bool isUsable() { return getThingType()->isUsable(); }
    bool isWrapable() { return getThingType()->isWrapable(); }
    bool isUnwrapable() { return getThingType()->isUnwrapable(); }
    bool isTopEffect() { return getThingType()->isTopEffect(); }
    bool hasAction() { return getThingType()->hasAction(); }
    bool isOpaque() { return getThingType()->isOpaque(); }
    bool isSingleDimension() { return getThingType()->isSingleDimension(); }
    bool isTall(const bool useRealSize = false) { return getThingType()->isTall(useRealSize); }
    uint16_t getClassification() { return getThingType()->getClassification(); }

    void canDraw(bool canDraw) { m_canDraw = canDraw; }
    bool canDraw()  const { return m_canDraw; }

    MarketData getMarketData() { return getThingType()->getMarketData(); }

    virtual void onPositionChange(const Position& /*newPos*/, const Position& /*oldPos*/) {}
    virtual void onAppear() {}
    virtual void onDisappear() {}

protected:
    Position m_position;
    uint16_t m_datId{ 0 };
    ThingTypePtr m_thingType;

private:
    bool m_canDraw{ true };
};
#pragma pack(pop)
