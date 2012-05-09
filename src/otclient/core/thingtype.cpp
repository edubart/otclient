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

#include "thingtype.h"
#include "spritemanager.h"

#include <framework/graphics/graphics.h>
#include <framework/graphics/texture.h>
#include <framework/graphics/image.h>

ThingType::ThingType()
{
    m_dimensions.fill(0);
    m_parameters.fill(0);
    m_properties.fill(false);
}

void ThingType::draw(const Point& dest, float scaleFactor, int w, int h, int xPattern, int yPattern, int zPattern, int layer, int animationPhase)
{
    int scaledSize = Otc::TILE_PIXELS * scaleFactor;

    Point displacement(m_parameters[DisplacementX], m_parameters[DisplacementY]);

    Rect drawRect(dest - displacement*scaleFactor, Size(scaledSize, scaledSize));
    g_painter->setColor(Color::white);
    g_painter->drawTexturedRect(drawRect, getSprite(w, h, layer, xPattern, yPattern, zPattern, animationPhase));
}

void ThingType::draw(const Point& dest, float scaleFactor, int xPattern, int yPattern, int zPattern, int animationPhase)
{
    for(int l = 0; l < m_dimensions[Layers]; ++l)
        for(int w = 0; w < m_dimensions[Width]; ++w)
            for(int h = 0; h < m_dimensions[Height]; ++h)
                draw(dest - Point(w,h)*Otc::TILE_PIXELS*scaleFactor, scaleFactor, w, h, xPattern, yPattern, zPattern, l, animationPhase);
}

void ThingType::drawMask(const Point& dest, float scaleFactor, int w, int h, int xPattern, int yPattern, int zPattern, int layer, int animationPhase, ThingType::SpriteMask mask)
{
    int scaledSize = Otc::TILE_PIXELS * scaleFactor;

    Point displacement(m_parameters[DisplacementX], m_parameters[DisplacementY]);

    Rect drawRect(dest - displacement*scaleFactor, Size(scaledSize, scaledSize));
    g_painter->drawTexturedRect(drawRect, getSpriteMask(w, h, layer, xPattern, yPattern, zPattern, animationPhase, mask));
}

TexturePtr& ThingType::getSprite(int w, int h, int l, int x, int y, int z, int a)
{
    uint index = getSpriteIndex(w,h,l,x,y,z,a);
    TexturePtr& spriteTexture = m_sprites[index];
    if(!spriteTexture) {
        ImagePtr spriteImage = g_sprites.getSpriteImage(m_spritesIndex[index]);
        if(!spriteImage)
            spriteTexture = g_graphics.getEmptyTexture();
        else {
            spriteTexture = TexturePtr(new Texture(spriteImage));
            spriteTexture->setSmooth(true);

            if(g_graphics.canUseMipmaps())
                spriteTexture->generateSoftwareMipmaps(spriteImage->getPixels());
        }
    }

    return spriteTexture;
}

TexturePtr& ThingType::getSpriteMask(int w, int h, int l, int x, int y, int z, int a, ThingType::SpriteMask mask)
{
    if(m_spritesMask.size() == 0)
        m_spritesMask.resize(m_spritesIndex.size());

    uint index = getSpriteIndex(w,h,l,x,y,z,a);
    TexturePtr& maskTexture = m_spritesMask[index][mask];
    if(!maskTexture) {
        ImagePtr maskImage = g_sprites.getSpriteImage(m_spritesIndex[index]);
        if(!maskImage)
            maskTexture = g_graphics.getEmptyTexture();
        else {
            static Color maskColors[LastMask] = { Color::yellow, Color::red, Color::green, Color::blue };
            maskImage->overwriteMask(maskColors[mask]);

            maskTexture = TexturePtr(new Texture(maskImage));
            maskTexture->setSmooth(true);

            if(g_graphics.canUseMipmaps())
                maskTexture->generateSoftwareMipmaps(maskImage->getPixels());
        }
    }

    return maskTexture;
}
