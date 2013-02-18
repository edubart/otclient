/*
 * Copyright (c) 2010-2013 OTClient <https://github.com/edubart/otclient>
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
#include "game.h"
#include "lightview.h"

#include <framework/graphics/graphics.h>
#include <framework/graphics/texture.h>
#include <framework/graphics/image.h>
#include <framework/graphics/texturemanager.h>
#include <framework/core/filestream.h>
#include <framework/otml/otml.h>

ThingType::ThingType()
{
    m_category = ThingInvalidCategory;
    m_id = 0;
    m_null = true;
    m_exactSize = 0;
    m_numPatternX = m_numPatternY = m_numPatternZ = 0;
    m_animationPhases = 0;
    m_layers = 0;
    m_elevation = 0;
    m_opacity = 1.0f;
}

void ThingType::unserialize(uint16 clientId, ThingCategory category, const FileStreamPtr& fin)
{
    m_null = false;
    m_id = clientId;
    m_category = category;

    bool done = false;
    for(int i = 0 ; i < ThingLastAttr;++i) {
        int attr = fin->getU8();
        if(attr == ThingLastAttr) {
            done = true;
            break;
        }

        if(g_game.getFeature(Otc::GameChargeableItems)) {
            if(attr == ThingAttrWritable) {
                m_attribs.set(ThingAttrChargeable, true);
                continue;
            } else if(attr > ThingAttrWritable)
                attr -= 1;
        }

        switch(attr) {
            case ThingAttrDisplacement: {
                m_displacement.x = fin->getU16();
                m_displacement.y = fin->getU16();
                m_attribs.set(attr, true);
                break;
            }
            case ThingAttrLight: {
                Light light;
                light.intensity = fin->getU16();
                light.color = fin->getU16();
                m_attribs.set(attr, light);
                break;
            }
            case ThingAttrMarket: {
                MarketData market;
                market.category = fin->getU16();
                market.tradeAs = fin->getU16();
                market.showAs = fin->getU16();
                market.name = fin->getString();
                market.restrictVocation = fin->getU16();
                market.requiredLevel = fin->getU16();
                m_attribs.set(attr, market);
                break;
            }
            case ThingAttrElevation: {
                m_elevation = fin->getU16();
                m_attribs.set(attr, m_elevation);
                break;
            }
            case ThingAttrGround:
            case ThingAttrWritable:
            case ThingAttrWritableOnce:
            case ThingAttrMinimapColor:
            case ThingAttrCloth:
            case ThingAttrLensHelp:
                m_attribs.set(attr, fin->getU16());
                break;
            default:
                m_attribs.set(attr, true);
                break;
        };
    }

    if(!done)
        stdext::throw_exception("corrupt data");

    uint8 width = fin->getU8();
    uint8 height = fin->getU8();
    m_size = Size(width, height);
    m_exactSize = (width > 1 || height > 1) ? std::min((int)fin->getU8(), std::max(width * 32, height * 32)) : 32;
    m_layers = fin->getU8();
    m_numPatternX = fin->getU8();
    m_numPatternY = fin->getU8();
    m_numPatternZ = fin->getU8();
    m_animationPhases = fin->getU8();

    int totalSprites = m_size.area() * m_layers * m_numPatternX * m_numPatternY * m_numPatternZ * m_animationPhases;

    if(totalSprites == 0)
        stdext::throw_exception("a thing type has no sprites");
    if(totalSprites > 4096)
        stdext::throw_exception("a thing type has more than 4096 sprites");

    m_spritesIndex.resize(totalSprites);
    for(int i = 0; i < totalSprites; i++)
        m_spritesIndex[i] = g_game.getFeature(Otc::GameSpritesU32) ? fin->getU32() : fin->getU16();

    m_textures.resize(m_animationPhases);
    m_texturesFramesRects.resize(m_animationPhases);
    m_texturesFramesOriginRects.resize(m_animationPhases);
    m_texturesFramesOffsets.resize(m_animationPhases);
}

void ThingType::unserializeOtml(const OTMLNodePtr& node)
{
    for(const OTMLNodePtr& node2 : node->children()) {
        if(node2->tag() == "opacity")
            m_opacity = node2->value<float>();
        else if(node2->tag() == "notprewalkable")
            m_attribs.set(ThingAttrNotPreWalkable, node2->value<bool>());
        else if(node2->tag() == "image")
            m_customImage = node2->value();
        else if(node2->tag() == "full-ground") {
            if(node2->value<bool>())
                m_attribs.set(ThingAttrFullGround, true);
            else
                m_attribs.remove(ThingAttrFullGround);
        }
    }
}

void ThingType::draw(const Point& dest, float scaleFactor, int layer, int xPattern, int yPattern, int zPattern, int animationPhase, LightView *lightView)
{
    if(m_null)
        return;

    if(animationPhase >= m_animationPhases)
        return;

    const TexturePtr& texture = getTexture(animationPhase); // texture might not exists, neither its rects.
    if(!texture)
        return;

    uint frameIndex = getTextureIndex(layer, xPattern, yPattern, zPattern);
    if(frameIndex >= m_texturesFramesRects[animationPhase].size())
        return;

    Point textureOffset;
    Rect textureRect;

    if(scaleFactor != 1.0f) {
        textureRect = m_texturesFramesOriginRects[animationPhase][frameIndex];
    } else {
        textureOffset = m_texturesFramesOffsets[animationPhase][frameIndex];
        textureRect = m_texturesFramesRects[animationPhase][frameIndex];
    }

    Rect screenRect(dest + (textureOffset - m_displacement - (m_size.toPoint() - Point(1, 1)) * 32) * scaleFactor,
                    textureRect.size() * scaleFactor);

    bool useOpacity = m_opacity < 1.0f;

    if(useOpacity)
        g_painter->setColor(Color(1.0f,1.0f,1.0f,m_opacity));

    g_painter->drawTexturedRect(screenRect, texture, textureRect);

    if(useOpacity)
        g_painter->setColor(Color::white);

    if(lightView && hasLight()) {
        Light light = getLight();
        if(light.intensity > 0)
            lightView->addLightSource(screenRect.center(), scaleFactor, light);
    }
}

const TexturePtr& ThingType::getTexture(int animationPhase)
{
    TexturePtr& animationPhaseTexture = m_textures[animationPhase];
    if(!animationPhaseTexture) {
        bool useCustomImage = false;
        if(animationPhase == 0 && !m_customImage.empty())
            useCustomImage = true;

        // we don't need layers in common items, they will be pre-drawn
        int textureLayers = 1;
        int numLayers = m_layers;
        if(m_category == ThingCategoryCreature && numLayers >= 2) {
             // 5 layers: outfit base, red mask, green mask, blue mask, yellow mask
            textureLayers = 5;
            numLayers = 5;
        }

        int indexSize = textureLayers * m_numPatternX * m_numPatternY * m_numPatternZ;
        Size textureSize = getBestTextureDimension(m_size.width(), m_size.height(), indexSize);
        ImagePtr fullImage;

        if(useCustomImage)
            fullImage = Image::load(m_customImage);
        else
            fullImage = ImagePtr(new Image(textureSize * Otc::TILE_PIXELS));

        m_texturesFramesRects[animationPhase].resize(indexSize);
        m_texturesFramesOriginRects[animationPhase].resize(indexSize);
        m_texturesFramesOffsets[animationPhase].resize(indexSize);

        for(int z = 0; z < m_numPatternZ; ++z) {
            for(int y = 0; y < m_numPatternY; ++y) {
                for(int x = 0; x < m_numPatternX; ++x) {
                    for(int l = 0; l < numLayers; ++l) {
                        bool spriteMask = (m_category == ThingCategoryCreature && l > 0);
                        int frameIndex = getTextureIndex(l % textureLayers, x, y, z);
                        Point framePos = Point(frameIndex % (textureSize.width() / m_size.width()) * m_size.width(),
                                               frameIndex / (textureSize.width() / m_size.width()) * m_size.height()) * Otc::TILE_PIXELS;

                        if(!useCustomImage) {
                            for(int h = 0; h < m_size.height(); ++h) {
                                for(int w = 0; w < m_size.width(); ++w) {
                                    uint spriteIndex = getSpriteIndex(w, h, spriteMask ? 1 : l, x, y, z, animationPhase);
                                    ImagePtr spriteImage = g_sprites.getSpriteImage(m_spritesIndex[spriteIndex]);
                                    if(spriteImage) {
                                        if(spriteMask) {
                                            static Color maskColors[] = { Color::red, Color::green, Color::blue, Color::yellow };
                                            spriteImage->overwriteMask(maskColors[l - 1]);
                                        }
                                        Point spritePos = Point(m_size.width()  - w - 1,
                                                                m_size.height() - h - 1) * Otc::TILE_PIXELS;

                                        fullImage->blit(framePos + spritePos, spriteImage);
                                    }
                                }
                            }
                        }

                        Rect drawRect(framePos + Point(m_size.width(), m_size.height()) * Otc::TILE_PIXELS - Point(1,1), framePos);
                        for(int x = framePos.x; x < framePos.x + m_size.width() * Otc::TILE_PIXELS; ++x) {
                            for(int y = framePos.y; y < framePos.y + m_size.height() * Otc::TILE_PIXELS; ++y) {
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
                        m_texturesFramesOriginRects[animationPhase][frameIndex] = Rect(framePos, Size(m_size.width(), m_size.height()) * Otc::TILE_PIXELS);
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

Size ThingType::getBestTextureDimension(int w, int h, int count)
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

uint ThingType::getSpriteIndex(int w, int h, int l, int x, int y, int z, int a) {
    uint index =
        ((((((a % m_animationPhases)
        * m_numPatternZ + z)
        * m_numPatternY + y)
        * m_numPatternX + x)
        * m_layers + l)
        * m_size.height() + h)
        * m_size.width() + w;
    assert(index < m_spritesIndex.size());
    return index;
}

uint ThingType::getTextureIndex(int l, int x, int y, int z) {
    return ((l * m_numPatternZ + z)
               * m_numPatternY + y)
               * m_numPatternX + x;
}

int ThingType::getExactSize(int layer, int xPattern, int yPattern, int zPattern, int animationPhase)
{
    if(m_null)
        return 0;

    getTexture(animationPhase); // we must calculate it anyway.
    int frameIndex = getTextureIndex(layer, xPattern, yPattern, zPattern);
    Size size = m_texturesFramesOriginRects[animationPhase][frameIndex].size() - m_texturesFramesOffsets[animationPhase][frameIndex].toSize();
    return std::max(size.width(), size.height());
}
