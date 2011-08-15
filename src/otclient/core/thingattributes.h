#ifndef THINGATTRIBUTES_H
#define THINGATTRIBUTES_H

#include "declarations.h"

struct ThingAttributes
{
    ThingAttributes() {
        group = THING_GROUP_NONE;
        blockSolid = false;
        hasHeight = false;
        blockPathFind = false;
        blockProjectile = false;
        alwaysOnTop = false;
        alwaysOnTopOrder = 0;
        stackable = false;
        useable = false;
        moveable = true;
        pickupable = false;
        rotable = false;
        readable = false;
        lookThrough = false;
        speed = 0;
        lightLevel = 0;
        lightColor = 0;
        isVertical = false;
        isHorizontal = false;
        isHangable = false;
        miniMapColor = 0;
        hasMiniMapColor = false;
        subParam07 = 0;
        subParam08 = 0;
        width = 0;
        height = 0;
        blendframes = 0;
        xdiv = 0;
        ydiv = 0;
        zdiv = 0;
        animcount = 0;
        xOffset = 0;
        yOffset = 0;
    }

    bool stackable, alwaysOnTop, useable, readable, moveable, blockSolid, blockProjectile, blockPathFind, pickupable,
         isHangable, isHorizontal, isVertical, rotable, hasHeight, lookThrough, hasMiniMapColor;
    uint8 alwaysOnTopOrder, width, height, blendframes, xdiv, ydiv, zdiv, animcount, xOffset, yOffset;
    uint16 speed, subParam07, subParam08, lightLevel, lightColor, miniMapColor;
    std::vector<int> sprites;
    ThingAttributesGroup group;
};

#endif
