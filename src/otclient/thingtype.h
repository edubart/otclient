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

#ifndef THINGTYPE_H
#define THINGTYPE_H

#include "declarations.h"
#include <framework/core/declarations.h>
#include <framework/graphics/declarations.h>
#include <framework/graphics/coordsbuffer.h>
#include <framework/luaengine/luaobject.h>
#include <framework/net/server.h>
#include <framework/util/attribstorage.h>

enum ThingCategory : uint8 {
    ThingCategoryItem = 0,
    ThingCategoryCreature,
    ThingCategoryEffect,
    ThingCategoryMissile,
    ThingInvalidCategory,
    ThingLastCategory = ThingInvalidCategory
};

enum ThingAttr : uint8 {
    ThingAttrGround           = 0,
    ThingAttrGroundBorder     = 1,
    ThingAttrOnBottom         = 2,
    ThingAttrOnTop            = 3,
    ThingAttrContainer        = 4,
    ThingAttrStackable        = 5,
    ThingAttrForceUse         = 6,
    ThingAttrMultiUse         = 7,
    ThingAttrWritable         = 8,
    ThingAttrWritableOnce     = 9,
    ThingAttrFluidContainer   = 10,
    ThingAttrSplash           = 11,
    ThingAttrNotWalkable      = 12,
    ThingAttrNotMoveable      = 13,
    ThingAttrBlockProjectile  = 14,
    ThingAttrNotPathable      = 15,
    ThingAttrPickupable       = 16,
    ThingAttrHangable         = 17,
    ThingAttrHookSouth        = 18,
    ThingAttrHookEast         = 19,
    ThingAttrRotateable       = 20,
    ThingAttrLight            = 21,
    ThingAttrDontHide         = 22,
    ThingAttrTranslucent      = 23,
    ThingAttrDisplacement     = 24,
    ThingAttrElevation        = 25,
    ThingAttrLyingCorpse      = 26,
    ThingAttrAnimateAlways    = 27,
    ThingAttrMinimapColor     = 28,
    ThingAttrLensHelp         = 29,
    ThingAttrFullGround       = 30,
    ThingAttrLook             = 31,
    ThingAttrCloth            = 32,
    ThingAttrMarket           = 33,
    ThingAttrChargeable       = 254, // deprecated
    ThingLastAttr             = 255,
};

enum SpriteMask {
    SpriteMaskRed = 1,
    SpriteMaskGreen,
    SpriteMaskBlue,
    SpriteMaskYellow
};

struct MarketData {
    std::string name;
    int category;
    uint16 requiredLevel;
    uint16 restrictProfession;
    uint16 showAs;
    uint16 tradeAs;
};

struct Light {
    uint8 intensity;
    uint8 color;
};

class ThingType : public LuaObject
{
public:
    ThingType();

    void unserialize(uint16 clientId, ThingCategory category, const FileStreamPtr& fin);

    void draw(const Point& dest, float scaleFactor, int layer, int xPattern, int yPattern, int zPattern, int animationPhase);

    uint16 getId() { return m_id; }
    ThingCategory getCategory() { return m_category; }
    bool isNull() { return m_null; }
    bool hasAttr(ThingAttr attr) { return m_attribs.has(attr); }

    Size getSize() { return m_size; }
    int getWidth() { return m_size.width(); }
    int getHeight() { return m_size.height(); }
    int getExactSize() { return m_exactSize; }
    int getLayers() { return m_layers; }
    int getNumPatternX() { return m_numPatternX; }
    int getNumPatternY() { return m_numPatternY; }
    int getNumPatternZ() { return m_numPatternZ; }
    int getAnimationPhases() { return m_animationPhases; }
    Point getDisplacement() { return m_displacement; }
    int getDisplacementX() { return getDisplacement().x; }
    int getDisplacementY() { return getDisplacement().y; }

    int getGroundSpeed() { return m_attribs.get<uint16>(ThingAttrGround); }
    int getMaxTextLength() { return m_attribs.has(ThingAttrWritableOnce) ? m_attribs.get<uint16>(ThingAttrWritableOnce) : m_attribs.get<uint16>(ThingAttrWritable); }
    Light getLight() { return m_attribs.get<Light>(ThingAttrLight); }
    int getMinimapColor() { return m_attribs.get<uint16>(ThingAttrMinimapColor); }
    int getLensHelp() { return m_attribs.get<uint16>(ThingAttrLensHelp); }
    int getClothSlot() { return m_attribs.get<uint16>(ThingAttrCloth); }
    int getElevation() { return m_attribs.get<uint16>(ThingAttrElevation); }
    MarketData getMarketData() { return m_attribs.get<MarketData>(ThingAttrMarket); }
    bool isGround() { return m_attribs.has(ThingAttrGround); }
    bool isGroundBorder() { return m_attribs.has(ThingAttrGroundBorder); }
    bool isOnBottom() { return m_attribs.has(ThingAttrOnBottom); }
    bool isOnTop() { return m_attribs.has(ThingAttrOnTop); }
    bool isContainer() { return m_attribs.has(ThingAttrContainer); }
    bool isStackable() { return m_attribs.has(ThingAttrStackable); }
    bool isForceUse() { return m_attribs.has(ThingAttrForceUse); }
    bool isMultiUse() { return m_attribs.has(ThingAttrMultiUse); }
    bool isWritable() { return m_attribs.has(ThingAttrWritable); }
    bool isChargeable() { return m_attribs.has(ThingAttrChargeable); }
    bool isWritableOnce() { return m_attribs.has(ThingAttrWritableOnce); }
    bool isFluidContainer() { return m_attribs.has(ThingAttrFluidContainer); }
    bool isSplash() { return m_attribs.has(ThingAttrSplash); }
    bool isNotWalkable() { return m_attribs.has(ThingAttrNotWalkable); }
    bool isNotMoveable() { return m_attribs.has(ThingAttrNotMoveable); }
    bool blockProjectile() { return m_attribs.has(ThingAttrBlockProjectile); }
    bool isNotPathable() { return m_attribs.has(ThingAttrNotPathable); }
    bool isPickupable() { return m_attribs.has(ThingAttrPickupable); }
    bool isHangable() { return m_attribs.has(ThingAttrHangable); }
    bool isHookSouth() { return m_attribs.has(ThingAttrHookSouth); }
    bool isHookEast() { return m_attribs.has(ThingAttrHookEast); }
    bool isRotateable() { return m_attribs.has(ThingAttrRotateable); }
    bool hasLight() { return m_attribs.has(ThingAttrLight); }
    bool isDontHide() { return m_attribs.has(ThingAttrDontHide); }
    bool isTranslucent() { return m_attribs.has(ThingAttrTranslucent); }
    bool hasDisplacement() { return m_attribs.has(ThingAttrDisplacement); }
    bool hasElevation() { return m_attribs.has(ThingAttrElevation); }
    bool isLyingCorpse() { return m_attribs.has(ThingAttrLyingCorpse); }
    bool isAnimateAlways() { return m_attribs.has(ThingAttrAnimateAlways); }
    bool hasMiniMapColor() { return m_attribs.has(ThingAttrMinimapColor); }
    bool hasLensHelp() { return m_attribs.has(ThingAttrLensHelp); }
    bool isFullGround() { return m_attribs.has(ThingAttrFullGround); }
    bool isIgnoreLook() { return m_attribs.has(ThingAttrLook); }
    bool isCloth() { return m_attribs.has(ThingAttrCloth); }
    bool isMarketable() { return m_attribs.has(ThingAttrMarket); }

private:
    const TexturePtr& getTexture(int animationPhase);
    Size getBestTextureDimension(int w, int h, int count);
    uint getSpriteIndex(int w, int h, int l, int x, int y, int z, int a);
    uint getTextureIndex(int l, int x, int y, int z);

    ThingCategory m_category;
    uint16 m_id;
    bool m_null;
    AttribStorage m_attribs;

    Size m_size;
    Point m_displacement;
    int m_exactSize;
    int m_numPatternX, m_numPatternY, m_numPatternZ;
    int m_animationPhases;
    int m_layers;

    std::vector<int> m_spritesIndex;
    std::vector<TexturePtr> m_textures;
    std::vector<std::vector<Rect>> m_texturesFramesRects;
    std::vector<std::vector<Rect>> m_texturesFramesOriginRects;
    std::vector<std::vector<Point>> m_texturesFramesOffsets;
};

#endif
