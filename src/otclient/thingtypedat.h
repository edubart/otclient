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

#ifndef THINGTYPEDAT_H
#define THINGTYPEDAT_H

#include "declarations.h"
#include <framework/core/declarations.h>
#include <framework/graphics/declarations.h>
#include <framework/graphics/coordsbuffer.h>
#include <framework/luaengine/luaobject.h>
#include <framework/net/server.h>
#include <framework/util/attribstorage.h>

enum DatCategory {
    DatItemCategory = 0,
    DatCreatureCategory,
    DatEffectCategory,
    DatMissileCategory,
    DatLastCategory,
    DatInvalidCategory = DatLastCategory
};

enum DatSpriteMask {
    DatRedMask = 1,
    DatGreenMask,
    DatBlueMask,
    DatYellowMask,
    DatLastMask
};

enum DatAttrib {
    DatAttribGround = 0,
    DatAttribGroundBorder,
    DatAttribOnBottom,
    DatAttribOnTop,
    DatAttribContainer,
    DatAttribStackable,
    DatAttribForceUse,
    DatAttribMultiUse,
    //DatAttribRune
    DatAttribWritable,
    DatAttribWritableOnce,
    DatAttribFluidContainer,
    DatAttribSplash,
    DatAttribNotWalkable,
    DatAttribNotMoveable,
    DatAttribBlockProjectile,
    DatAttribNotPathable,
    DatAttribPickupable,
    DatAttribHangable,
    DatAttribHookSouth,
    DatAttribHookEast,
    DatAttribRotateable,
    DatAttribLight,
    DatAttribDontHide,
    DatAttribTranslucent,
    DatAttribDisplacement,
    DatAttribElevation,
    DatAttribLyingCorpse,
    DatAttribAnimateAlways,
    DatAttribMiniMapColor,
    DatAttribLensHelp,
    DatAttribFullGround,
    DatAttribIgnoreLook,
    DatAttribCloth,
    DatAttribMarket,
    DatLastAttrib = 255,

    // legacy attribs
    DatAttribChargeable = 254
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

class ThingTypeDat : public LuaObject
{
public:
    ThingTypeDat();

    void unserialize(uint16 clientId, DatCategory category, const FileStreamPtr& fin);

    void draw(const Point& dest, float scaleFactor, int layer, int xPattern, int yPattern, int zPattern, int animationPhase);

    uint16 getId() { return m_id; }
    DatCategory getCategory() { return m_category; }
    bool isNull() { return m_null; }

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

    int getGroundSpeed() { return m_attribs.get<uint16>(DatAttribGround); }
    int getMaxTextLength() { return m_attribs.has(DatAttribWritableOnce) ? m_attribs.get<uint16>(DatAttribWritableOnce) : m_attribs.get<uint16>(DatAttribWritable); }
    Light getLight() { return m_attribs.get<Light>(DatAttribLight); }
    int getMinimapColor() { return m_attribs.get<uint16>(DatAttribMiniMapColor); }
    int getLensHelp() { return m_attribs.get<uint16>(DatAttribLensHelp); }
    int getClothSlot() { return m_attribs.get<uint16>(DatAttribCloth); }
    int getElevation() { return m_attribs.get<uint16>(DatAttribElevation); }
    MarketData getMarketData() { return m_attribs.get<MarketData>(DatAttribMarket); }
    bool isGround() { return m_attribs.has(DatAttribGround); }
    bool isGroundBorder() { return m_attribs.has(DatAttribGroundBorder); }
    bool isOnBottom() { return m_attribs.has(DatAttribOnBottom); }
    bool isOnTop() { return m_attribs.has(DatAttribOnTop); }
    bool isContainer() { return m_attribs.has(DatAttribContainer); }
    bool isStackable() { return m_attribs.has(DatAttribStackable); }
    bool isForceUse() { return m_attribs.has(DatAttribForceUse); }
    bool isMultiUse() { return m_attribs.has(DatAttribMultiUse); }
    bool isWritable() { return m_attribs.has(DatAttribWritable); }
    bool isChargeable() { return m_attribs.has(DatAttribChargeable); }
    bool isWritableOnce() { return m_attribs.has(DatAttribWritableOnce); }
    bool isFluidContainer() { return m_attribs.has(DatAttribFluidContainer); }
    bool isSplash() { return m_attribs.has(DatAttribSplash); }
    bool isNotWalkable() { return m_attribs.has(DatAttribNotWalkable); }
    bool isNotMoveable() { return m_attribs.has(DatAttribNotMoveable); }
    bool blockProjectile() { return m_attribs.has(DatAttribBlockProjectile); }
    bool isNotPathable() { return m_attribs.has(DatAttribNotPathable); }
    bool isPickupable() { return m_attribs.has(DatAttribPickupable); }
    bool isHangable() { return m_attribs.has(DatAttribHangable); }
    bool isHookSouth() { return m_attribs.has(DatAttribHookSouth); }
    bool isHookEast() { return m_attribs.has(DatAttribHookEast); }
    bool isRotateable() { return m_attribs.has(DatAttribRotateable); }
    bool hasLight() { return m_attribs.has(DatAttribLight); }
    bool isDontHide() { return m_attribs.has(DatAttribDontHide); }
    bool isTranslucent() { return m_attribs.has(DatAttribTranslucent); }
    bool hasDisplacement() { return m_attribs.has(DatAttribDisplacement); }
    bool hasElevation() { return m_attribs.has(DatAttribElevation); }
    bool isLyingCorpse() { return m_attribs.has(DatAttribLyingCorpse); }
    bool isAnimateAlways() { return m_attribs.has(DatAttribAnimateAlways); }
    bool hasMiniMapColor() { return m_attribs.has(DatAttribMiniMapColor); }
    bool hasLensHelp() { return m_attribs.has(DatAttribLensHelp); }
    bool isFullGround() { return m_attribs.has(DatAttribFullGround); }
    bool isIgnoreLook() { return m_attribs.has(DatAttribIgnoreLook); }
    bool isCloth() { return m_attribs.has(DatAttribCloth); }
    bool isMarketable() { return m_attribs.has(DatAttribMarket); }

private:
    const TexturePtr& getTexture(int animationPhase);
    Size getBestTextureDimension(int w, int h, int count);
    uint getSpriteIndex(int w, int h, int l, int x, int y, int z, int a);
    uint getTextureIndex(int l, int x, int y, int z);

    DatCategory m_category;
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
