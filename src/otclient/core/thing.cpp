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
    for(int yi = 0; yi < m_type->dimensions[ThingType::Height]; yi++) {
        for(int xi = 0; xi < m_type->dimensions[ThingType::Width]; xi++) {
            int sprIndex = ((((((m_animation % m_type->dimensions[ThingType::AnimationPhases])
                            * m_type->dimensions[ThingType::PatternZ] + m_zPattern)
                            * m_type->dimensions[ThingType::PatternY] + m_yPattern)
                            * m_type->dimensions[ThingType::PatternX] + m_xPattern)
                            * m_type->dimensions[ThingType::Layers] + layers)
                            * m_type->dimensions[ThingType::Height] + yi)
                            * m_type->dimensions[ThingType::Width] + xi;

            int spriteId = m_type->sprites[sprIndex];
            if(!spriteId)
                continue;

            TexturePtr spriteTex = g_sprites.getSpriteTexture(spriteId, mask);

            Rect drawRect((p.x - xi*32) - m_type->parameters[ThingType::DisplacementX],
                          (p.y - yi*32) - m_type->parameters[ThingType::DisplacementY],
                          32, 32);
            g_graphics.drawTexturedRect(drawRect, spriteTex);
        }
    }
}

void Thing::setId(uint32 id)
{
    m_id = id;
    m_type = getType();
}

int Thing::getStackPriority()
{
    if(m_type->properties[ThingType::IsGround])
        return 0;
    else if(m_type->properties[ThingType::IsGroundBorder])
        return 1;
    else if(m_type->properties[ThingType::IsOnBottom])
        return 2;
    else if(m_type->properties[ThingType::IsOnTop])
        return 3;
    else if(asCreature())
        return 4;
    return 5;
}

