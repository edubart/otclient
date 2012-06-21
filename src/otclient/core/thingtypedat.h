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
#include <framework/luascript/luaobject.h>
#include <framework/net/server.h>

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
    DatAttribIsGround = 0,
    DatAttribIsGroundBorder,
    DatAttribIsOnBottom,
    DatAttribIsOnTop,
    DatAttribIsContainer,
    DatAttribIsStackable,
    DatAttribIsForceUse,
    DatAttribIsMultiUse,
    DatAttribIsWritable,
    DatAttribIsWritableOnce,
    DatAttribIsFluidContainer,
    DatAttribIsFluid,
    DatAttribIsNotWalkable,
    DatAttribIsNotMoveable,
    DatAttribBlockProjectile,
    DatAttribIsNotPathable,
    DatAttribIsPickupable,
    DatAttribIsHangable,
    DatAttribHookSouth,
    DatAttribHookEast,
    DatAttribIsRotateable,
    DatAttribHasLight,
    DatAttribDontHide,
    DatAttribIsTranslucent,
    DatAttribHasDisplacement,
    DatAttribHasElevation,
    DatAttribIsLyingCorpse,
    DatAttribAnimateAlways,
    DatAttribMiniMapColor,
    DatAttribLensHelp,
    DatAttribIsFullGround,
    DatAttribIgnoreLook,
    DatAttribCloth,
    DatAttribMarket,
    DatLastAttrib = 255
};

enum DatDimension {
    DatWidth = 0,
    DatHeight,
    DatExactSize,
    DatLayers,
    DatPatternX,
    DatPatternY,
    DatPatternZ,
    DatAnimationPhases,
    DatLastDimension
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
    Point getDisplacement() { return m_displacement; }
    int getDisplacementX() { return m_displacement.x; }
    int getDisplacementY() { return m_displacement.y; }
    int getExactSize() { return m_exactSize; }
    int getLayers() { return m_layers; }
    int getNumPatternX() { return m_numPatternX; }
    int getNumPatternY() { return m_numPatternY; }
    int getNumPatternZ() { return m_numPatternZ; }
    int getAnimationPhases() { return m_animationPhases; }
    int getGroundSpeed() { return m_groundSpeed; }
    int getMaxTextLength() { return m_maxTextLenght; }
    int getLightLevel() { return m_lightLevel; }
    int getLightColor() { return m_lightColor; }
    int getMinimapColor() { return m_miniMapColor; }
    int getLensHelp() { return m_lensHelp; }
    int getClothSlot() { return m_clothSlot; }
    int getElevation() { return m_elevation; }
    bool isGround() { return m_isGround; }
    bool isGroundBorder() { return m_isGroundBorder; }
    bool isOnBottom() { return m_isOnBottom; }
    bool isOnTop() { return m_isOnTop; }
    bool isContainer() { return m_isContainer; }
    bool isStackable() { return m_isStackable; }
    bool isForceUse() { return m_isForceUse; }
    bool isMultiUse() { return m_isMultiUse; }
    bool isWritable() { return m_isWritable; }
    bool isWritableOnce() { return m_isWritableOnce; }
    bool isFluidContainer() { return m_isFluidContainer; }
    bool isFluid() { return m_isFluid; }
    bool isNotWalkable() { return m_isNotWalkable; }
    bool isNotMoveable() { return m_isNotMoveable; }
    bool blockProjectile() { return m_blockProjectile; }
    bool isNotPathable() { return m_isNotPathable; }
    bool isPickupable() { return m_isPickupable; }
    bool isHangable() { return m_isHangable; }
    bool isHookSouth() { return m_isHookSouth; }
    bool isHookEast() { return m_isHookEast; }
    bool isRotateable() { return m_isRotateable; }
    bool hasLight() { return m_hasLight; }
    bool isDontHide() { return m_isDontHide; }
    bool isTranslucent() { return m_isTranslucent; }
    bool hasDisplacement() { return m_hasDisplacement; }
    bool hasElevation() { return m_hasElevation; }
    bool isLyingCorpse() { return m_isLyingCorpse; }
    bool isAnimateAlways() { return m_isAnimateAlways; }
    bool hasMiniMapColor() { return m_hasMiniMapColor; }
    bool hasLensHelp() { return m_hasLensHelp; }
    bool isFullGround() { return m_isFullGround; }
    bool isIgnoreLook() { return m_isIgnoreLook; }
    bool isCloth() { return m_isCloth; }

private:
    const TexturePtr& getTexture(int animationPhase);
    Size getBestTextureDimension(int w, int h, int count);
    uint getSpriteIndex(int w, int h, int l, int x, int y, int z, int a);
    uint getTextureIndex(int l, int x, int y, int z);

    DatCategory m_category;
    uint16 m_id;
    Boolean<true> m_null;

    std::vector<int> m_spritesIndex;
    std::vector<TexturePtr> m_textures;
    std::vector<std::vector<Rect>> m_texturesFramesRects;
    std::vector<std::vector<Rect>> m_texturesFramesOriginRects;
    std::vector<std::vector<Point>> m_texturesFramesOffsets;

    // dat stuff
    Size m_size;
    Point m_displacement;
    int m_exactSize;
    int m_layers;
    int m_numPatternX;
    int m_numPatternY;
    int m_numPatternZ;
    int m_animationPhases;
    int m_groundSpeed;
    int m_maxTextLenght;
    int m_lightLevel;
    int m_lightColor;
    int m_miniMapColor;
    int m_lensHelp;
    int m_clothSlot;
    int m_elevation;
    Boolean<false> m_isGround;
    Boolean<false> m_isGroundBorder;
    Boolean<false> m_isOnBottom;
    Boolean<false> m_isOnTop;
    Boolean<false> m_isContainer;
    Boolean<false> m_isStackable;
    Boolean<false> m_isForceUse;
    Boolean<false> m_isMultiUse;
    Boolean<false> m_isWritable;
    Boolean<false> m_isWritableOnce;
    Boolean<false> m_isFluidContainer;
    Boolean<false> m_isFluid;
    Boolean<false> m_isNotWalkable;
    Boolean<false> m_isNotMoveable;
    Boolean<false> m_blockProjectile;
    Boolean<false> m_isNotPathable;
    Boolean<false> m_isPickupable;
    Boolean<false> m_isHangable;
    Boolean<false> m_isHookSouth;
    Boolean<false> m_isHookEast;
    Boolean<false> m_isRotateable;
    Boolean<false> m_hasLight;
    Boolean<false> m_isDontHide;
    Boolean<false> m_isTranslucent;
    Boolean<false> m_hasDisplacement;
    Boolean<false> m_hasElevation;
    Boolean<false> m_isLyingCorpse;
    Boolean<false> m_isAnimateAlways;
    Boolean<false> m_hasMiniMapColor;
    Boolean<false> m_hasLensHelp;
    Boolean<false> m_isFullGround;
    Boolean<false> m_isIgnoreLook;
    Boolean<false> m_isCloth;
};

#endif
