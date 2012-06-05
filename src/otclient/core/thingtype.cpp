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
#include "thingstype.h"
#include "spritemanager.h"

#include <framework/graphics/graphics.h>
#include <framework/graphics/texture.h>
#include <framework/graphics/image.h>

ThingType::ThingType()
{
    m_category = 0;
    m_dimensions.fill(0);
    m_parameters.fill(0);
    m_properties.fill(false);
}

void ThingType::draw(const Point& dest, float scaleFactor, int layer, int xPattern, int yPattern, int zPattern, int animationPhase)
{
    const TexturePtr& texture = getTexture(animationPhase); // texture might not exists, neither its rects.

    int frameIndex = getTextureIndex(layer, xPattern, yPattern, zPattern);
    Point textureOffset = m_texturesFramesOffsets[animationPhase][frameIndex];
    Rect textureRect = m_texturesFramesRects[animationPhase][frameIndex];

    Point displacement(m_parameters[DisplacementX], m_parameters[DisplacementY]);
    Rect screenRect(dest + (-displacement + textureOffset - Point(m_dimensions[Width] - 1, m_dimensions[Height] - 1) * Otc::TILE_PIXELS) * scaleFactor,
                    textureRect.size() * scaleFactor);

    g_painter->drawTexturedRect(screenRect, texture, textureRect);
}

void ThingType::drawMask(const Point& dest, float scaleFactor, int w, int h, int xPattern, int yPattern, int zPattern, int layer, int animationPhase, ThingType::SpriteMask mask)
{
    int scaledSize = Otc::TILE_PIXELS * scaleFactor;

    Point displacement(m_parameters[DisplacementX], m_parameters[DisplacementY]);

    Rect drawRect(dest - displacement*scaleFactor, Size(scaledSize, scaledSize));
    g_painter->drawTexturedRect(drawRect, getSpriteMask(w, h, layer, xPattern, yPattern, zPattern, animationPhase, mask));
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

TexturePtr& ThingType::getTexture(int animationPhase)
{
    TexturePtr& animationPhaseTexture = m_textures[animationPhase];
    if(!animationPhaseTexture) {

        int textureLayers = m_dimensions[Layers];
        if(m_category != ThingsType::Creature) // we dont need layers in texture. they can be 'rendered' now.
            textureLayers = 1;

        int indexSize = textureLayers * m_dimensions[PatternX] * m_dimensions[PatternY] * m_dimensions[PatternZ];
        Size textureSize = getBestDimension(m_dimensions[Width], m_dimensions[Height], indexSize);
        ImagePtr fullImage = ImagePtr(new Image(textureSize * Otc::TILE_PIXELS));

        m_texturesFramesRects[animationPhase].resize(indexSize);
        m_texturesFramesOffsets[animationPhase].resize(indexSize);

        for(int z = 0; z < m_dimensions[PatternZ]; ++z) {
            for(int y = 0; y < m_dimensions[PatternY]; ++y) {
                for(int x = 0; x < m_dimensions[PatternX]; ++x) {
                    for(int l = 0; l < m_dimensions[Layers]; ++l) {

                        int frameIndex = getTextureIndex(l % textureLayers, x, y, z);
                        Point framePos = Point(frameIndex % (textureSize.width() / m_dimensions[Width]) * m_dimensions[Width],
                                               frameIndex / (textureSize.width() / m_dimensions[Width]) * m_dimensions[Height]) * Otc::TILE_PIXELS;

                        for(int h = 0; h < m_dimensions[Height]; ++h) {
                            for(int w = 0; w < m_dimensions[Width]; ++w) {
                                uint spriteIndex = getSpriteIndex(w, h, l, x, y, z, animationPhase);
                                ImagePtr spriteImage = g_sprites.getSpriteImage(m_spritesIndex[spriteIndex]);
                                if(spriteImage) {
                                    Point spritePos = Point(m_dimensions[Width]  - w - 1,
                                                            m_dimensions[Height] - h - 1) * Otc::TILE_PIXELS;

                                    fullImage->append(framePos + spritePos, spriteImage);
                                }
                            }
                        }

                        Rect drawRect(framePos + Point(m_dimensions[Width], m_dimensions[Height]) * Otc::TILE_PIXELS, framePos);
                        for(int x = framePos.x; x < framePos.x + m_dimensions[Width] * Otc::TILE_PIXELS; ++x) {
                            for(int y = framePos.y; y < framePos.y + m_dimensions[Height] * Otc::TILE_PIXELS; ++y) {
                                uint8 *p = fullImage->getPixel(x,y);
                                if(p[3] != 0x00) {
                                    drawRect.setTop(std::min(y, (int)drawRect.top()));
                                    drawRect.setLeft(std::min(x, (int)drawRect.left()));
                                    drawRect.setBottom(std::max(y, (int)drawRect.bottom()));
                                    drawRect.setRight(std::max(x, (int)drawRect.right()));
                                }
                            }
                        }

                        m_texturesFramesRects[animationPhase][frameIndex] = drawRect;
                        m_texturesFramesOffsets[animationPhase][frameIndex] = drawRect.topLeft() - framePos;
                    }
                }
            }
        }
        animationPhaseTexture = TexturePtr(new Texture(fullImage));
        animationPhaseTexture->setSmooth(true);

        //if(g_graphics.canUseMipmaps())
            //animationPhaseTexture->generateSoftwareMipmaps(fullImage->getPixels());
    }
    return animationPhaseTexture;
}

Size ThingType::getBestDimension(int w, int h, int count)
{
    const int MAX = 16;

    int k = 1;
    while(k < w)
        k<<=1;
    w = k;

    k = 1;
    while(k < h)
        k<<=1;
    h = k;

    int numSprites = w*h*count;
    assert(numSprites <= MAX*MAX);
    assert(w <= MAX);
    assert(h <= MAX);

    Size bestDimension = Size(MAX, MAX);
    for(int i=w;i<=MAX;i<<=1) {
        for(int j=h;j<=MAX;j<<=1) {
            Size candidateDimension = Size(i, j);
            if(candidateDimension.area() < numSprites)
                continue;
            if((candidateDimension.area() < bestDimension.area()) ||
               (candidateDimension.area() == bestDimension.area() && candidateDimension.width() + candidateDimension.height() < bestDimension.width() + bestDimension.height()))
                bestDimension = candidateDimension;
        }
    }

    return bestDimension;
}
