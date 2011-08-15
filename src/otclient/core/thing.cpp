#include "thing.h"
#include "tibiaspr.h"
#include <framework/graphics/graphics.h>

ThingAttributes::ThingAttributes()
{
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
    sprites = NULL;
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

ThingAttributes::~ThingAttributes()
{
    if(sprites)
        delete []sprites;
}

Thing::Thing()
{
    m_type = TYPE_NONE;
}

void Thing::internalDraw(int x, int y, int blendframes, int xdiv, int ydiv, int zdiv, int anim)
{
    ThingAttributes *thingAttributes = getAttributes();
    if(!thingAttributes)
        return;

    for(int yi = 0; yi < thingAttributes->height; yi++) {
        for(int xi = 0; xi < thingAttributes->width; xi++) {
            uint16 sprIndex = xi +
                    yi * thingAttributes->width +
                    blendframes * thingAttributes->width * thingAttributes->height +
                    xdiv * thingAttributes->width * thingAttributes->height * thingAttributes->blendframes +
                    ydiv * thingAttributes->width * thingAttributes->height * thingAttributes->blendframes * thingAttributes->xdiv +
                    zdiv * thingAttributes->width * thingAttributes->height * thingAttributes->blendframes * thingAttributes->xdiv * thingAttributes->ydiv +
                    anim * thingAttributes->width * thingAttributes->height * thingAttributes->blendframes * thingAttributes->xdiv * thingAttributes->ydiv * thingAttributes->zdiv;
            uint16 itemId = thingAttributes->sprites[sprIndex];
            if(itemId == 0xFFFF)
                continue;
            TexturePtr data = g_tibiaSpr.getSprite(itemId);

            int offsetX = 0, offsetY = 0;
            if(thingAttributes->hasHeight) {
                offsetX = thingAttributes->xOffset;
                offsetY = thingAttributes->xOffset; // << look to xoffset
            }

            g_graphics.drawTexturedRect(Rect((x - xi*32) - offsetX, (y - yi*32) - offsetY, 32, 32), data, Rect(0, 0, 32, 32));
        }
    }
}
