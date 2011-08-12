#include "item.h"
#include "tibiadat.h"
#include "tibiaspr.h"
#include <graphics/graphics.h>

ItemAttributes::ItemAttributes()
{
    group = ITEM_GROUP_NONE;
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
}

ItemAttributes::~ItemAttributes()
{
    if(sprites)
        delete []sprites;
}

Item::Item()
{
    m_type = Thing::TYPE_ITEM;
}

void Item::draw(int x, int y, int z)
{
    ItemAttributes *itemAttributes = g_tibiaDat.getItemAttributes(m_id);

    int cDivX = 0, cDivY = 0, cDivZ = 0, cAnim = 0;

    if(itemAttributes->group == ITEM_GROUP_SPLASH || itemAttributes->group == ITEM_GROUP_FLUID || itemAttributes->stackable) {
        //cDivX = subType % itemAttributes->xdiv;
        //cDivY = subType / itemAttributes->xdiv;
    }
    else if(!itemAttributes->moveable) {
        cDivX = x % itemAttributes->xdiv;
        cDivY = y % itemAttributes->ydiv;
        cDivZ = z % itemAttributes->zdiv;
    }

    x *= 32;
    y *= 32;
    z = (7-z)*32;

    for(int b = 0; b < itemAttributes->blendframes; b++) {
        for(int yi = 0; yi < itemAttributes->height; yi++) {
            for(int xi = 0; xi < itemAttributes->width; xi++) {
                uint16 sprIndex = xi +
                                  yi * itemAttributes->width +
                                  b * itemAttributes->width * itemAttributes->height +
                                  cDivX * itemAttributes->width * itemAttributes->height * itemAttributes->blendframes +
                                  cDivY * itemAttributes->width * itemAttributes->height * itemAttributes->blendframes * itemAttributes->xdiv +
                                  cDivZ * itemAttributes->width * itemAttributes->height * itemAttributes->blendframes * itemAttributes->xdiv * itemAttributes->ydiv +
                                  cAnim * itemAttributes->width * itemAttributes->height * itemAttributes->blendframes * itemAttributes->xdiv * itemAttributes->ydiv * itemAttributes->zdiv;
                uint16 itemId = itemAttributes->sprites[sprIndex];
                if(itemId == 0xFFFF)
                    continue;
                TexturePtr data = g_tibiaSpr.getSprite(itemId);

                // todo verify this to draw in correct pos (screenX, screenY)
                g_graphics.drawTexturedRect(Rect(x - xi*32 - z, y - yi*32 - z, 32, 32), data, Rect(0, 0, 32, 32));

                //g_graphics.drawBoundingRect(Rect(x - xi*32 - z, y - yi*32 - z, 32, 32), Color::green);
                if(x/32 == 7 && y/32 == 5 && z/32+7 == 7)
                    g_graphics.drawBoundingRect(Rect(x - xi*32 - z, y - yi*32 - z, 32, 32), Color::red);
            }
        }
    }
}
