/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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

struct ThingAttributes
{
    ThingAttributes() {
        layers = 0;
        width = height = 0;
        exactSize = 0;
        xPattern = yPattern = zPattern = 0;
        animationPhases = 0;
        xDisplacment = yDisplacment = 0;
        elevation = 0;

        isGround = false;
        isGroundClip = false;
        isOnBottom = false;
        isOnTop = false;
        isContainer = false;
        isStackable = false;
        isForceUse = false;
        isMultiUse = false;
        isWritable = false;
        isWritableOnce = false;
        isFluidContainer = false;
        isSplash = false;
        isNotWalkable = false;
        isNotMoveable = false;
        isNotProjectable = false;
        isNotPathable = false;
        isPickupable = false;
        isHangable = false;
        isHookSouth = false;
        isHookEast = false;
        isRotable = false;
        isDontHide = false;
        isTranslucent = false;
        isLyingCorpse = false;
        isAnimatedAlways = false;
        isLensHelp = false;
        isFullGround = false;
        isIgnoreLook = false;
        isClothe = false;
        hasLight = false;
        hasMiniMapColor = false;

        groundSpeed = 0;
        fluidParam = 0;
        maxTextLength = 0;
        lightLevel = lightColor = 0;
        miniMapColor = 0;
        lensHelpParam = 0;
    }

    uint8 layers;
    uint8 width, height;
    uint8 exactSize;
    uint8 xPattern, yPattern, zPattern;
    uint8 animationPhases;
    uint16 xDisplacment, yDisplacment;
    uint16 elevation;
    std::vector<int> sprites;

    bool isGround;
    bool isGroundClip;
    bool isOnBottom;
    bool isOnTop;
    bool isContainer;
    bool isStackable;
    bool isForceUse;
    bool isMultiUse;
    bool isWritable;
    bool isWritableOnce;
    bool isFluidContainer;
    bool isSplash;
    bool isNotWalkable;
    bool isNotMoveable;
    bool isNotProjectable;
    bool isNotPathable;
    bool isPickupable;
    bool isHangable;
    bool isHookSouth;
    bool isHookEast;
    bool isRotable;
    bool isDontHide;
    bool isTranslucent;
    bool isLyingCorpse;
    bool isAnimatedAlways;
    bool isLensHelp;
    bool isFullGround;
    bool isIgnoreLook;
    bool isClothe;
    bool hasLight;
    bool hasMiniMapColor;

    uint16 groundSpeed;
    uint8 fluidParam;
    uint16 maxTextLength;
    uint16 lightLevel, lightColor;
    uint16 miniMapColor;
    uint16 lensHelpParam;
};

typedef std::vector<ThingAttributes> ThingAttributesList;

#endif
