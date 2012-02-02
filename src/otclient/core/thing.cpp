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

#include "thing.h"
#include "spritemanager.h"
#include "thingstype.h"
#include <framework/graphics/graphics.h>
#include "map.h"

Thing::Thing()
{
    m_type = g_thingsType.getEmptyThingType();
}

int Thing::getStackPriority()
{
    if(isGround())
        return 0;
    else if(isGroundBorder())
        return 1;
    else if(isOnBottom())
        return 2;
    else if(isOnTop())
        return 3;
    else if(asCreature())
        return 4;
    else // common items
        return 5;
}

const TilePtr& Thing::getTile()
{
    return g_map.getTile(m_position);
}

void Thing::internalDraw(const Point& dest, float scaleFactor, int xPattern, int yPattern, int zPattern, int layer, int animationPhase)
{
    int scaledSize = Otc::TILE_PIXELS * scaleFactor;

    for(int h = 0; h < getDimensionHeight(); h++) {
        for(int w = 0; w < getDimensionWidth(); w++) {
            int spriteId = getSpriteId(w, h, layer, xPattern, yPattern, zPattern, animationPhase);
            if(!spriteId)
                continue;

            TexturePtr spriteTex = g_sprites.getSpriteTexture(spriteId);
            Rect drawRect(dest.x - (w*Otc::TILE_PIXELS - getDisplacementX())*scaleFactor,
                          dest.y - (h*Otc::TILE_PIXELS - getDisplacementY())*scaleFactor,
                          scaledSize, scaledSize);

            g_painter.drawTexturedRect(drawRect, spriteTex);
        }
    }
}

