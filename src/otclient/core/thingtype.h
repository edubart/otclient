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

#ifndef THINGATTRIBUTES_H
#define THINGATTRIBUTES_H

#include "declarations.h"

#include <framework/graphics/declarations.h>

struct ThingType
{
    enum Dimension {
        Width = 0,
        Height,
        ExactSize,
        Layers,
        PatternX,
        PatternY,
        PatternZ,
        AnimationPhases,
        LastDimension
    };

    enum Property {
        IsGround = 0,
        IsGroundBorder,
        IsOnBottom,
        IsOnTop,
        IsContainer,
        IsStackable,
        IsForceUse,
        IsMultiUse,
        IsWritable,
        IsWritableOnce,
        IsFluidContainer,
        IsFluid,
        NotWalkable,
        IsNotMovable,
        BlockProjectile,
        NotPathable,
        IsPickupable,
        IsHangable,
        HookSouth,
        HookEast,
        IsRotateable,
        HasLight,
        DontHide,
        IsTranslucent,
        HasDisplacement,
        HasElevation,
        IsLyingCorpse,
        AnimateAlways,
        MiniMap,
        LensHelp,
        IsFullGround,
        IgnoreLook,
        Cloth,
        Animation,
        LastProperty,
        LastPropertyValue = 255
    };

    enum Parameter {
        GroundSpeed = 0,
        Fluid,
        MaxTextLenght,
        LightLevel,
        LightColor,
        MiniMapColor,
        LensHelpParameter,
        ClothSlot,
        DisplacementX,
        DisplacementY,
        Elevation,
        LastParameter
    };

    enum SpriteMask {
        YellowMask = 0,
        RedMask,
        GreenMask,
        BlueMask,
        LastMask
    };

    ThingType();

    void draw(const Point& dest, float scaleFactor, int w, int h, int xPattern, int yPattern, int zPattern, int layer, int animationPhase);
    void draw(const Point& dest, float scaleFactor, int xPattern, int yPattern, int zPattern, int animationPhase);
    void drawMask(const Point& dest, float scaleFactor, int w, int h, int xPattern, int yPattern, int zPattern, int layer, int animationPhase, SpriteMask mask);

    TexturePtr& getSprite(int w, int h, int l, int x, int y, int z, int a);
    TexturePtr& getSpriteMask(int w, int h, int l, int x, int y, int z, int a, SpriteMask mask);

    bool getProperty(Property property) { return m_properties[property]; }
    int getParameter(Parameter param) { return m_parameters[param]; }
    int getDimension(Dimension dimension) { return m_dimensions[dimension]; }

private:
    uint getSpriteIndex(int w, int h, int l, int x, int y, int z, int a) {
        uint index = ((((((a % m_dimensions[ThingType::AnimationPhases])
            * m_dimensions[ThingType::PatternZ] + z)
            * m_dimensions[ThingType::PatternY] + y)
            * m_dimensions[ThingType::PatternX] + x)
            * m_dimensions[ThingType::Layers] + l)
            * m_dimensions[ThingType::Height] + h)
            * m_dimensions[ThingType::Width] + w;
        assert(index < m_sprites.size());
        return index;
    }

    std::array<int, LastDimension> m_dimensions;
    std::array<int, LastParameter> m_parameters;
    std::array<bool, LastProperty> m_properties;
    std::vector<int> m_spritesIndex;
    std::vector<TexturePtr> m_sprites;
    std::vector<std::array<TexturePtr, LastMask>> m_spritesMask;

    friend class ThingsType;
};

typedef std::vector<ThingType> ThingTypeList;

#endif
