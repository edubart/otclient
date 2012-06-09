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
    Point textureOffset;
    Rect textureRect;

    if(scaleFactor != 1.0f) {
        textureRect = m_texturesFramesOriginRects[animationPhase][frameIndex];
    } else {
        textureOffset = m_texturesFramesOffsets[animationPhase][frameIndex];
        textureRect = m_texturesFramesRects[animationPhase][frameIndex];
    }

    Point displacement(m_parameters[DisplacementX], m_parameters[DisplacementY]);
    Rect screenRect(dest + (-displacement + textureOffset - Point(m_dimensions[Width] - 1, m_dimensions[Height] - 1) * Otc::TILE_PIXELS) * scaleFactor,
                    textureRect.size() * scaleFactor);

    g_painter->drawTexturedRect(screenRect, texture, textureRect);
}

TexturePtr& ThingType::getTexture(int animationPhase)
{
    TexturePtr& animationPhaseTexture = m_textures[animationPhase];
    if(!animationPhaseTexture) {
        // we don't need layers in common items, they will be pre-drawn
        int textureLayers = 1;
        int numLayers = m_dimensions[Layers];
        if(m_category == ThingsType::Creature && m_dimensions[Layers] >= 2) {
             // 5 layers: outfit base, red mask, green mask, blue mask, yellow mask
            textureLayers = 5;
            numLayers = 5;
        }

        int indexSize = textureLayers * m_dimensions[PatternX] * m_dimensions[PatternY] * m_dimensions[PatternZ];
        Size textureSize = getBestDimension(m_dimensions[Width], m_dimensions[Height], indexSize);
        ImagePtr fullImage = ImagePtr(new Image(textureSize * Otc::TILE_PIXELS));

        m_texturesFramesRects[animationPhase].resize(indexSize);
        m_texturesFramesOriginRects[animationPhase].resize(indexSize);
        m_texturesFramesOffsets[animationPhase].resize(indexSize);

        for(int z = 0; z < m_dimensions[PatternZ]; ++z) {
            for(int y = 0; y < m_dimensions[PatternY]; ++y) {
                for(int x = 0; x < m_dimensions[PatternX]; ++x) {
                    for(int l = 0; l < numLayers; ++l) {
                        bool spriteMask = (m_category == ThingsType::Creature && l > 0);
                        int frameIndex = getTextureIndex(l % textureLayers, x, y, z);
                        Point framePos = Point(frameIndex % (textureSize.width() / m_dimensions[Width]) * m_dimensions[Width],
                                               frameIndex / (textureSize.width() / m_dimensions[Width]) * m_dimensions[Height]) * Otc::TILE_PIXELS;

                        for(int h = 0; h < m_dimensions[Height]; ++h) {
                            for(int w = 0; w < m_dimensions[Width]; ++w) {
                                uint spriteIndex = getSpriteIndex(w, h, spriteMask ? 1 : l, x, y, z, animationPhase);
                                ImagePtr spriteImage = g_sprites.getSpriteImage(m_spritesIndex[spriteIndex]);
                                if(spriteImage) {
                                    if(spriteMask) {
                                        static Color maskColors[] = { Color::red, Color::green, Color::blue, Color::yellow };
                                        spriteImage->overwriteMask(maskColors[l - 1]);
                                    }
                                    Point spritePos = Point(m_dimensions[Width]  - w - 1,
                                                            m_dimensions[Height] - h - 1) * Otc::TILE_PIXELS;

                                    fullImage->blit(framePos + spritePos, spriteImage);
                                }
                            }
                        }

                        Rect drawRect(framePos + Point(m_dimensions[Width], m_dimensions[Height]) * Otc::TILE_PIXELS - Point(1,1), framePos);
                        for(int x = framePos.x; x < framePos.x + m_dimensions[Width] * Otc::TILE_PIXELS; ++x) {
                            for(int y = framePos.y; y < framePos.y + m_dimensions[Height] * Otc::TILE_PIXELS; ++y) {
                                uint8 *p = fullImage->getPixel(x,y);
                                if(p[3] != 0x00) {
                                    drawRect.setTop   (std::min(y, (int)drawRect.top()));
                                    drawRect.setLeft  (std::min(x, (int)drawRect.left()));
                                    drawRect.setBottom(std::max(y, (int)drawRect.bottom()));
                                    drawRect.setRight (std::max(x, (int)drawRect.right()));
                                }
                            }
                        }

                        m_texturesFramesRects[animationPhase][frameIndex] = drawRect;
                        m_texturesFramesOriginRects[animationPhase][frameIndex] = Rect(framePos, Size(m_dimensions[Width], m_dimensions[Height]) * Otc::TILE_PIXELS);
                        m_texturesFramesOffsets[animationPhase][frameIndex] = drawRect.topLeft() - framePos;
                    }
                }
            }
        }
        animationPhaseTexture = TexturePtr(new Texture(fullImage, true));
        animationPhaseTexture->setSmooth(true);
    }
    return animationPhaseTexture;
}

Size ThingType::getBestDimension(int w, int h, int count)
{
    const int MAX = 32;

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
