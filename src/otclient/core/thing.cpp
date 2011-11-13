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

Thing::Thing() : m_id(0)
{
    m_xPattern = 0;
    m_yPattern = 0;
    m_zPattern = 0;
    m_animation = 0;
}

void Thing::internalDraw(const Point& p, int layers, Otc::SpriteMask mask)
{
    const ThingType& type = getType();

    for(int yi = 0; yi < type.dimensions[ThingType::Height]; yi++) {
        for(int xi = 0; xi < type.dimensions[ThingType::Width]; xi++) {
            int sprIndex = ((((((m_animation % type.dimensions[ThingType::AnimationPhases])
                            * type.dimensions[ThingType::PatternZ] + m_zPattern)
                            * type.dimensions[ThingType::PatternY] + m_yPattern)
                            * type.dimensions[ThingType::PatternX] + m_xPattern)
                            * type.dimensions[ThingType::Layers] + layers)
                            * type.dimensions[ThingType::Height] + yi)
                            * type.dimensions[ThingType::Width] + xi;

            int spriteId = type.sprites[sprIndex];
            if(!spriteId)
                continue;

            TexturePtr spriteTex = g_sprites.getSpriteTexture(spriteId, mask);

            Rect drawRect((p.x - xi*32) - type.parameters[ThingType::DisplacementX],
                          (p.y - yi*32) - type.parameters[ThingType::DisplacementY],
                          32, 32);
            g_graphics.drawTexturedRect(drawRect, spriteTex);
        }
    }
}

int Thing::getStackPriority()
{
    const ThingType& type = getType();
    if(type.properties[ThingType::IsGround])
        return 0;
    else if(type.properties[ThingType::IsGroundBorder])
        return 1;
    else if(type.properties[ThingType::IsOnBottom])
        return 2;
    else if(type.properties[ThingType::IsOnTop])
        return 3;
    else if(asCreature())
        return 4;
    return 5;
}

