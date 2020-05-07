/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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
    m_realSize = 0;
    m_animator = nullptr;
    m_numPatternX = m_numPatternY = m_numPatternZ = 0;
    m_animationPhases = 0;
    m_layers = 0;
    m_elevation = 0;
    m_opacity = 1.0f;
}

void ThingType::serialize(const FileStreamPtr& fin)
{
    for(int i = 0; i < ThingLastAttr; ++i) {
        if(!hasAttr((ThingAttr)i))
            continue;

        int attr = i;
        if(g_game.getClientVersion() >= 780) {
            if(attr == ThingAttrChargeable)
                attr = ThingAttrWritable;
            else if(attr >= ThingAttrWritable)
                attr += 1;
        } else if(g_game.getClientVersion() >= 1000) {
            if(attr == ThingAttrNoMoveAnimation)
                attr = 16;
            else if(attr >= ThingAttrPickupable)
                attr += 1;
        }

        fin->addU8(attr);
        switch(attr) {
            case ThingAttrDisplacement: {
                fin->addU16(m_displacement.x);
                fin->addU16(m_displacement.y);
                break;
            }
            case ThingAttrLight: {
                Light light = m_attribs.get<Light>(attr);
                fin->addU16(light.intensity);
                fin->addU16(light.color);
                break;
            }
            case ThingAttrMarket: {
                MarketData market = m_attribs.get<MarketData>(attr);
                fin->addU16(market.category);
                fin->addU16(market.tradeAs);
                fin->addU16(market.showAs);
                fin->addString(market.name);
                fin->addU16(market.restrictVocation);
                fin->addU16(market.requiredLevel);
                break;
            }
            case ThingAttrUsable:
            case ThingAttrElevation:
            case ThingAttrGround:
            case ThingAttrWritable:
            case ThingAttrWritableOnce:
            case ThingAttrMinimapColor:
            case ThingAttrCloth:
            case ThingAttrLensHelp:
                fin->addU16(m_attribs.get<uint16>(attr));
                break;
            default:
                break;
        };
    }
    fin->addU8(ThingLastAttr);

    fin->addU8(m_size.width());
    fin->addU8(m_size.height());

    if(m_size.width() > 1 || m_size.height() > 1)
        fin->addU8(m_realSize);

    fin->addU8(m_layers);
    fin->addU8(m_numPatternX);
    fin->addU8(m_numPatternY);
    fin->addU8(m_numPatternZ);
    fin->addU8(m_animationPhases);

    if(g_game.getFeature(Otc::GameEnhancedAnimations)) {
        if(m_animationPhases > 1 && m_animator != nullptr)  {
            m_animator->serialize(fin);
        }
    }

    for(int i: m_spritesIndex) {
        if(g_game.getFeature(Otc::GameSpritesU32))
            fin->addU32(i);
        else
            fin->addU16(i);
    }
}

void ThingType::unserialize(uint16 clientId, ThingCategory category, const FileStreamPtr& fin)
{
    m_null = false;
    m_id = clientId;
    m_category = category;

    int count = 0, attr = -1;
    bool done = false;
    for(int i = 0 ; i < ThingLastAttr;++i) {
        count++;
        attr = fin->getU8();
        if(attr == ThingLastAttr) {
            done = true;
            break;
        }

        if(g_game.getClientVersion() >= 1000) {
            /* In 10.10+ all attributes from 16 and up were
             * incremented by 1 to make space for 16 as
             * "No Movement Animation" flag.
             */
            if(attr == 16)
                attr = ThingAttrNoMoveAnimation;
            else if(attr > 16)
                attr -= 1;
        } else if(g_game.getClientVersion() >= 860) {
            /* Default attribute values follow
             * the format of 8.6-9.86.
             * Therefore no changes here.
             */
        } else if(g_game.getClientVersion() >= 780) {
            /* In 7.80-8.54 all attributes from 8 and higher were
             * incremented by 1 to make space for 8 as
             * "Item Charges" flag.
             */
            if(attr == 8) {
                m_attribs.set(ThingAttrChargeable, true);
                continue;
            } else if(attr > 8)
                attr -= 1;
        } else if(g_game.getClientVersion() >= 755) {
            /* In 7.55-7.72 attributes 23 is "Floor Change". */
            if(attr == 23)
                attr = ThingAttrFloorChange;
        } else if(g_game.getClientVersion() >= 740) {
            /* In 7.4-7.5 attribute "Ground Border" did not exist
             * attributes 1-15 have to be adjusted.
             * Several other changes in the format.
             */
            if(attr > 0 && attr <= 15)
                attr += 1;
            else if(attr == 16)
                attr = ThingAttrLight;
            else if(attr == 17)
                attr = ThingAttrFloorChange;
            else if(attr == 18)
                attr = ThingAttrFullGround;
            else if(attr == 19)
                attr = ThingAttrElevation;
            else if(attr == 20)
                attr = ThingAttrDisplacement;
            else if(attr == 22)
                attr = ThingAttrMinimapColor;
            else if(attr == 23)
                attr = ThingAttrRotateable;
            else if(attr == 24)
                attr = ThingAttrLyingCorpse;
            else if(attr == 25)
                attr = ThingAttrHangable;
            else if(attr == 26)
                attr = ThingAttrHookSouth;
            else if(attr == 27)
                attr = ThingAttrHookEast;
            else if(attr == 28)
                attr = ThingAttrAnimateAlways;

            /* "Multi Use" and "Force Use" are swapped */
            if(attr == ThingAttrMultiUse)
                attr = ThingAttrForceUse;
            else if(attr == ThingAttrForceUse)
                attr = ThingAttrMultiUse;
        }

        switch(attr) {
            case ThingAttrDisplacement: {
                if(g_game.getClientVersion() >= 755) {
                    m_displacement.x = fin->getU16();
                    m_displacement.y = fin->getU16();
                } else {
                    m_displacement.x = 8;
                    m_displacement.y = 8;
                }
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
            case ThingAttrUsable:
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
        stdext::throw_exception(stdext::format("corrupt data (id: %d, category: %d, count: %d, lastAttr: %d)",
            m_id, m_category, count, attr));

    bool hasFrameGroups = (category == ThingCategoryCreature && g_game.getFeature(Otc::GameIdleAnimations));
    uint8 groupCount = hasFrameGroups ? fin->getU8() : 1;

    m_animationPhases = 0;
    int totalSpritesCount = 0;

    for(int i = 0; i < groupCount; ++i) {
        uint8 frameGroupType = FrameGroupDefault;
        if(hasFrameGroups)
            frameGroupType = fin->getU8();

        uint8 width = fin->getU8();
        uint8 height = fin->getU8();
        m_size = Size(width, height);
        if(width > 1 || height > 1) {
            m_realSize = fin->getU8();
            m_exactSize = std::min<int>(m_realSize, std::max<int>(width * 32, height * 32));
        }
        else
            m_exactSize = 32;

        m_layers = fin->getU8();
        m_numPatternX = fin->getU8();
        m_numPatternY = fin->getU8();
        if(g_game.getClientVersion() >= 755)
            m_numPatternZ = fin->getU8();
        else
            m_numPatternZ = 1;
        
        int groupAnimationsPhases = fin->getU8();
        m_animationPhases += groupAnimationsPhases;

        if(groupAnimationsPhases > 1 && g_game.getFeature(Otc::GameEnhancedAnimations)) {
            m_animator = AnimatorPtr(new Animator);
            m_animator->unserialize(groupAnimationsPhases, fin);
        }

        int totalSprites = m_size.area() * m_layers * m_numPatternX * m_numPatternY * m_numPatternZ * groupAnimationsPhases;

        if((totalSpritesCount+totalSprites) > 4096)
            stdext::throw_exception("a thing type has more than 4096 sprites");

        m_spritesIndex.resize((totalSpritesCount+totalSprites));
        for(int j = totalSpritesCount; j < (totalSpritesCount+totalSprites); j++)
            m_spritesIndex[j] = g_game.getFeature(Otc::GameSpritesU32) ? fin->getU32() : fin->getU16();

        totalSpritesCount += totalSprites;
    }

    m_textures.resize(m_animationPhases);
    m_texturesFramesRects.resize(m_animationPhases);
    m_texturesFramesOriginRects.resize(m_animationPhases);
    m_texturesFramesOffsets.resize(m_animationPhases);
}

void ThingType::exportImage(std::string fileName)
{
    if(m_null)
        stdext::throw_exception("cannot export null thingtype");

    if(m_spritesIndex.empty())
        stdext::throw_exception("cannot export thingtype without sprites");

    ImagePtr image(new Image(Size(32 * m_size.width() * m_layers * m_numPatternX, 32 * m_size.height() * m_animationPhases * m_numPatternY * m_numPatternZ)));
    for(int z = 0; z < m_numPatternZ; ++z) {
        for(int y = 0; y < m_numPatternY; ++y) {
            for(int x = 0; x < m_numPatternX; ++x) {
                for(int l = 0; l < m_layers; ++l) {
                    for(int a = 0; a < m_animationPhases; ++a) {
                        for(int w = 0; w < m_size.width(); ++w) {
                            for(int h = 0; h < m_size.height(); ++h) {
                                image->blit(Point(32 * (m_size.width() - w - 1 + m_size.width() * x + m_size.width() * m_numPatternX * l),
                                                  32 * (m_size.height() - h - 1 + m_size.height() * y + m_size.height() * m_numPatternY * a + m_size.height() * m_numPatternY * m_animationPhases * z)),
                                            g_sprites.getSpriteImage(m_spritesIndex[getSpriteIndex(w, h, l, x, y, z, a)]));
                            }
                        }
                    }
                }
            }
        }
    }

    image->savePNG(fileName);
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
                        for(int fx = framePos.x; fx < framePos.x + m_size.width() * Otc::TILE_PIXELS; ++fx) {
                            for(int fy = framePos.y; fy < framePos.y + m_size.height() * Otc::TILE_PIXELS; ++fy) {
                                uint8 *p = fullImage->getPixel(fx,fy);
                                if(p[3] != 0x00) {
                                    drawRect.setTop   (std::min<int>(fy, (int)drawRect.top()));
                                    drawRect.setLeft  (std::min<int>(fx, (int)drawRect.left()));
                                    drawRect.setBottom(std::max<int>(fy, (int)drawRect.bottom()));
                                    drawRect.setRight (std::max<int>(fx, (int)drawRect.right()));
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
    return std::max<int>(size.width(), size.height());
}

void ThingType::setPathable(bool var)
{
    if(var == true)
        m_attribs.remove(ThingAttrNotPathable);
    else
        m_attribs.set(ThingAttrNotPathable, true);
}
