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

#include "thing.h"
#include "spritemanager.h"
#include <framework/graphics/graphics.h>

Thing::Thing(ThingType type) : m_id(0), m_type(type)
{
}

void Thing::internalDraw(int x, int y, int blendframes, int xdiv, int ydiv, int zdiv, int anim, SpriteMask mask)
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

            TexturePtr spriteTex = g_sprites.getSpriteTexture(spriteId, mask);

            Rect drawRect((x - xi*32) - attributes.drawOffset,
                          (y - yi*32) - attributes.drawOffset,
                          32, 32);
            g_graphics.drawTexturedRect(drawRect, spriteTex);
        }
    }
}
