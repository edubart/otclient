/*
* Copyright (c) 2010-2015 OTClient <https://github.com/edubart/otclient>
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
#include "animator.h"

#include <framework/core/declarations.h>
#include <framework/otml/declarations.h>
#include <framework/graphics/texture.h>
#include <framework/graphics/coordsbuffer.h>
#include <framework/luaengine/luaobject.h>
#include <framework/net/server.h>

enum FrameGroupType : uint8 {
    FrameGroupDefault = 0,
    FrameGroupIdle = FrameGroupDefault,
    FrameGroupMoving
};

enum ThingCategory : uint8 {
    ThingCategoryItem = 0,
    ThingCategoryCreature,
    ThingCategoryEffect,
    ThingCategoryMissile,
    ThingInvalidCategory,
    ThingLastCategory = ThingInvalidCategory
};

enum ThingAttr : uint8 {
    ThingAttrGround = 0,
    ThingAttrGroundBorder = 1,
    ThingAttrOnBottom = 2,
    ThingAttrOnTop = 3,
    ThingAttrContainer = 4,
    ThingAttrStackable = 5,
    ThingAttrForceUse = 6,
    ThingAttrMultiUse = 7,
    ThingAttrWritable = 8,
    ThingAttrWritableOnce = 9,
    ThingAttrFluidContainer = 10,
    ThingAttrSplash = 11,
    ThingAttrNotWalkable = 12,
    ThingAttrNotMoveable = 13,
    ThingAttrBlockProjectile = 14,
    ThingAttrNotPathable = 15,
    ThingAttrPickupable = 16,
    ThingAttrHangable = 17,
    ThingAttrHookSouth = 18,
    ThingAttrHookEast = 19,
    ThingAttrRotateable = 20,
    ThingAttrLight = 21,
    ThingAttrDontHide = 22,
    ThingAttrTranslucent = 23,
    ThingAttrDisplacement = 24,
    ThingAttrElevation = 25,
    ThingAttrLyingCorpse = 26,
    ThingAttrAnimateAlways = 27,
    ThingAttrMinimapColor = 28,
    ThingAttrLensHelp = 29,
    ThingAttrFullGround = 30,
    ThingAttrLook = 31,
    ThingAttrCloth = 32,
    ThingAttrMarket = 33,
    ThingAttrUsable = 34,

    // additional
    ThingAttrOpacity = 100,
    ThingAttrNotPreWalkable = 101,

    ThingAttrFloorChange = 252,
    ThingAttrNoMoveAnimation = 253, // 10.10: real value is 16, but we need to do this for backwards compatibility
    ThingAttrChargeable = 254, // deprecated
    ThingLastAttr = 255
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
    uint16 restrictVocation;
    uint16 showAs;
    uint16 tradeAs;
};

struct Light {
    Light() { intensity = 0; color = 215; }
    uint8 intensity;
    uint8 color;
};

class FrameGroup {
public:
    FrameGroup(ThingTypePtr belongsTo = nullptr, int phases = 0, int realSize = 0, int layers = 0, Size size = Size(0, 0), int exactSize = 0, int patternX = 0, int patternY = 0, int patternZ = 0, AnimatorPtr animator = nullptr,
        FrameGroupType type = FrameGroupDefault, std::string customImage = std::string(""), std::vector<TexturePtr> textures = std::vector<TexturePtr>(0),
        std::vector<std::vector<Rect>> texturesFramesRects = std::vector<std::vector<Rect>>(0), std::vector<std::vector<Rect>> texturesFramesOriginRects = std::vector<std::vector<Rect>>(0),
        std::vector<std::vector<Point>> texturesFramesOffsets = std::vector<std::vector<Point>>(0), std::vector<int> spritesIndex = std::vector<int>(0));

    Size getSize() { return m_size; }
    int getWidth() { return m_size.width(); }
    int getHeight() { return m_size.height(); }
    int getExactSize(int layer = 0, int xPattern = 0, int yPattern = 0, int zPattern = 0, int animationPhase = 0);
    int getRealSize() { return m_realSize; }
    int getLayers() { return m_layers; }
    int getNumPatternX() { return m_numPatternX; }
    int getNumPatternY() { return m_numPatternY; }
    int getNumPatternZ() { return m_numPatternZ; }
    int getAnimationPhases() { return m_animationPhases; }
    AnimatorPtr getAnimator() { return m_animator; }
    std::vector<int> getSprites() { return m_spritesIndex; }
    FrameGroupType getType() { return m_type; }
    ThingTypePtr getThing() { return m_belongsTo; }

    const TexturePtr& getTexture(int animationPhase);
    Size getBestTextureDimension(int w, int h, int count);
    uint getSpriteIndex(int w, int h, int l, int x, int y, int z, int a);
    uint getTextureIndex(int l, int x, int y, int z);

    /*    // accessing these variables through functions makes a mess ingame (but why?); guess someone more experienced has to make that happen; for now all these variables are made public (sic! xD)
    std::vector<int> getSpritesIndex() { return m_spritesIndex; }
    std::vector<TexturePtr> getTextures() { return m_textures; }
    std::vector<std::vector<Rect>> getTexturesFramesRects() { return m_texturesFramesRects; }
    std::vector<std::vector<Rect>> getTexturesFramesOriginRects() { return m_texturesFramesOriginRects; }
    std::vector<std::vector<Point>> getTexturesFramesOffsets() { return m_texturesFramesOffsets; }
    */
    Size m_size;
    std::vector<int> m_spritesIndex;
    std::vector<TexturePtr> m_textures;
    std::vector<std::vector<Rect>> m_texturesFramesRects;
    std::vector<std::vector<Rect>> m_texturesFramesOriginRects;
    std::vector<std::vector<Point>> m_texturesFramesOffsets;

private:
    int m_animationPhases;
    int m_realSize;
    int m_layers;
    int m_exactSize;
    int m_numPatternX, m_numPatternY, m_numPatternZ;
    AnimatorPtr m_animator;
    FrameGroupType m_type;
    std::string m_customImage;
    ThingTypePtr m_belongsTo;
};

class ThingType : public LuaObject
{
public:
    ThingType();

    void unserialize(uint16 clientId, ThingCategory category, const FileStreamPtr& fin);
    void unserializeOtml(const OTMLNodePtr& node);

    void serialize(const FileStreamPtr& fin);
    void exportImage(std::string fileName);

    void draw(const Point& dest, float scaleFactor, int layer, int xPattern, int yPattern, int zPattern, int animationPhase, LightView *lightView = nullptr, FrameGroupType type = FrameGroupDefault);

    uint16 getId() { return m_id; }
    ThingCategory getCategory() { return m_category; }
    bool isNull() { return m_null; }
    bool hasAttr(ThingAttr attr) { return m_attribs.has(attr); }

    Point getDisplacement() { return m_displacement; }
    int getDisplacementX() { return getDisplacement().x; }
    int getDisplacementY() { return getDisplacement().y; }
    int getElevation() { return m_elevation; }
    float getOpacity() { return m_opacity; }

    int getGroundSpeed() { return m_attribs.get<uint16>(ThingAttrGround); }
    int getMaxTextLength() { return m_attribs.has(ThingAttrWritableOnce) ? m_attribs.get<uint16>(ThingAttrWritableOnce) : m_attribs.get<uint16>(ThingAttrWritable); }
    Light getLight() { return m_attribs.get<Light>(ThingAttrLight); }
    int getMinimapColor() { return m_attribs.get<uint16>(ThingAttrMinimapColor); }
    int getLensHelp() { return m_attribs.get<uint16>(ThingAttrLensHelp); }
    int getClothSlot() { return m_attribs.get<uint16>(ThingAttrCloth); }
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

    // additional
    bool isNotPreWalkable() { return m_attribs.has(ThingAttrNotPreWalkable); }
    const TexturePtr& getTexture(int animationPhase, FrameGroupType type = FrameGroupDefault);

    // wrappers for FrameGroup functions which originally were functions of ThingType class (might come in handy); do not have to mess with other class' functions
    Size getBestTextureDimension(int w, int h, int count, FrameGroupType type = FrameGroupDefault);
    uint getSpriteIndex(int w, int h, int l, int x, int y, int z, int a, FrameGroupType type = FrameGroupDefault);
    uint getTextureIndex(int l, int x, int y, int z, FrameGroupType type = FrameGroupDefault);
    int getExactSize(int layer = 0, int xPattern = 0, int yPattern = 0, int zPattern = 0, int animationPhase = 0, FrameGroupType type = FrameGroupDefault);
    Size getSize(FrameGroupType type = FrameGroupDefault) {
        int groupId = 0;
        for (uint8 i = 0; i < m_frameGroups.size(); i++) {
            if (m_frameGroups[i]->getType() == type) {
                groupId = i;
            }
        }
        return m_frameGroups[groupId]->getSize();
    }
    int getWidth(FrameGroupType type = FrameGroupDefault) { return getSize(type).width(); }
    int getHeight(FrameGroupType type = FrameGroupDefault) { return getSize(type).height(); }
    int getNumPatternX(FrameGroupType type = FrameGroupDefault) {
        int groupId = 0;
        for (uint8 i = 0; i < m_frameGroups.size(); i++) {
            if (m_frameGroups[i]->getType() == type) {
                groupId = i;
            }
        }
        return m_frameGroups[groupId]->getNumPatternX();
    }
    int getNumPatternY(FrameGroupType type = FrameGroupDefault) {
        int groupId = 0;
        for (uint8 i = 0; i < m_frameGroups.size(); i++) {
            if (m_frameGroups[i]->getType() == type) {
                groupId = i;
            }
        }
        return m_frameGroups[groupId]->getNumPatternY();
    }
    int getNumPatternZ(FrameGroupType type = FrameGroupDefault) {
        int groupId = 0;
        for (uint8 i = 0; i < m_frameGroups.size(); i++) {
            if (m_frameGroups[i]->getType() == type) {
                groupId = i;
            }
        }
        return m_frameGroups[groupId]->getNumPatternZ();
    }
    int getAnimationPhases(FrameGroupType type = FrameGroupDefault) {
        int groupId = 0;
        for (uint8 i = 0; i < m_frameGroups.size(); i++) {
            if (m_frameGroups[i]->getType() == type) {
                groupId = i;
            }
        }
        return m_frameGroups[groupId]->getAnimationPhases();
    }
    AnimatorPtr getAnimator(FrameGroupType type = FrameGroupDefault) {
        int groupId = 0;
        for (uint8 i = 0; i < m_frameGroups.size(); i++) {
            if (m_frameGroups[i]->getType() == type) {
                groupId = i;
            }
        }
        return m_frameGroups[groupId]->getAnimator();
    }
    int getLayers(FrameGroupType type = FrameGroupDefault) {
        int groupId = 0;
        for (uint8 i = 0; i < m_frameGroups.size(); i++) {
            if (m_frameGroups[i]->getType() == type) {
                groupId = i;
            }
        }
        return m_frameGroups[groupId]->getLayers();
    }
    int getRealSize(FrameGroupType type = FrameGroupDefault) {
        int groupId = 0;
        for (uint8 i = 0; i < m_frameGroups.size(); i++) {
            if (m_frameGroups[i]->getType() == type) {
                groupId = i;
            }
        }
        return m_frameGroups[groupId]->getRealSize();
    }
    std::vector<int> getSprites(FrameGroupType type = FrameGroupDefault) {
        int groupId = 0;
        for (uint8 i = 0; i < m_frameGroups.size(); i++) {
            if (m_frameGroups[i]->getType() == type) {
                groupId = i;
            }
        }
        return m_frameGroups[groupId]->getSprites();
    }

    //Make m_frameGroups accessible outside the class
    std::vector<FrameGroup*> getFrameGroups() { return m_frameGroups; }

private:
    ThingCategory m_category;
    uint16 m_id;
    bool m_null;
    stdext::dynamic_storage<uint8> m_attribs;

    Point m_displacement;
    int m_elevation;
    float m_opacity;
    std::string m_customImage;

    std::vector<FrameGroup*> m_frameGroups;
};
#endif
