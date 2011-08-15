#include "thing.h"
#include "spritemanager.h"
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

Thing::Thing()
{
    m_type = TYPE_NONE;
}

void Thing::internalDraw(int x, int y, int blendframes, int xdiv, int ydiv, int zdiv, int anim)
{
    const ThingAttributes& attributes = getAttributes();

    for(int yi = 0; yi < attributes.height; yi++) {
        for(int xi = 0; xi < attributes.width; xi++) {
            int sprIndex = xi +
                    yi * attributes.width +
                    blendframes * attributes.width * attributes.height +
                    xdiv * attributes.width * attributes.height * attributes.blendframes +
                    ydiv * attributes.width * attributes.height * attributes.blendframes * attributes.xdiv +
                    zdiv * attributes.width * attributes.height * attributes.blendframes * attributes.xdiv * attributes.ydiv +
                    anim * attributes.width * attributes.height * attributes.blendframes * attributes.xdiv * attributes.ydiv * attributes.zdiv;

            int spriteId = attributes.sprites[sprIndex];
            if(!spriteId)
                continue;

            TexturePtr spriteTex = g_sprites.getSpriteTexture(spriteId);
            if(spriteTex->isEmpty())
                continue;

            int offsetX = 0, offsetY = 0;
            if(attributes.hasHeight) {
                offsetX = attributes.xOffset;
                offsetY = attributes.xOffset; // << look to xoffset
            }

            g_graphics.drawTexturedRect(Rect((x - xi*32) - offsetX, (y - yi*32) - offsetY, 32, 32), spriteTex, Rect(0, 0, 32, 32));
        }
    }
}
