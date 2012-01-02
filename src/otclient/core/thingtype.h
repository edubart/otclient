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

struct ThingType
{
    ThingType() {
        // fill default values
        dimensions.fill(0);
        properties.fill(false);
        parameters.fill(0);
    }

    enum Dimensions {
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
    std::array<int, LastDimension> dimensions;
    std::vector<int> sprites;

    enum Properties {
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
        NotMovable,
        BlockProjectile,
        NotPathable,
        Pickupable,
        IsHangable,
        HookSouth,
        HookEast,
        IsRotable,
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
    std::array<bool, LastProperty> properties;

    enum Parameters {
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
    std::array<int, LastParameter> parameters;

    int getSpriteId(int w, int h, int l, int x, int y, int z, int a)
    {
        int sprIndex = ((((((a % dimensions[ThingType::AnimationPhases])
                        * dimensions[ThingType::PatternZ] + z)
                        * dimensions[ThingType::PatternY] + y)
                        * dimensions[ThingType::PatternX] + x)
                        * dimensions[ThingType::Layers] + l)
                        * dimensions[ThingType::Height] + h)
                        * dimensions[ThingType::Width] + w;

        if(sprIndex >= 0 && sprIndex < (int)sprites.size())
            return sprites[sprIndex];

        return 0;
    }
};

typedef std::vector<ThingType> ThingTypeList;

#endif
