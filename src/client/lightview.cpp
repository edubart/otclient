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

#include "lightview.h"
#include <framework/graphics/framebuffer.h>
#include <framework/graphics/framebuffermanager.h>
#include <framework/graphics/image.h>
#include <framework/graphics/painter.h>
#include "mapview.h"
#include "map.h"

enum {
    MAX_LIGHT_INTENSITY = 8,
    MAX_AMBIENT_LIGHT_INTENSITY = 255
};

#define DEBUG_BUBBLE 0

LightView::LightView(const MapViewPtr& mapView, const uint8 version)
{
    m_mapView = mapView;
    m_version = version;

    m_lightbuffer = g_framebuffers.createFrameBuffer();
    m_lightTexture = generateLightBubble();
    m_blendEquation = Painter::BlendEquation_Add;

    reset();
}

TexturePtr LightView::generateLightBubble()
{
    int intensityVariant;
    float centerFactor;

    if(m_version == 1) {
        centerFactor = .1f;
        intensityVariant = 0xB4;
    } else {
        centerFactor = .0f;
        intensityVariant = 0xff;
    }

    const int bubbleRadius = 256,
        centerRadius = bubbleRadius * centerFactor,
        bubbleDiameter = bubbleRadius * 2;

    ImagePtr lightImage = ImagePtr(new Image(Size(bubbleDiameter, bubbleDiameter)));

    for(int x = 0; x < bubbleDiameter; ++x) {
        for(int y = 0; y < bubbleDiameter; ++y) {
            const float radius = std::sqrt((bubbleRadius - x) * (bubbleRadius - x) + (bubbleRadius - y) * (bubbleRadius - y));
            float intensity = stdext::clamp<float>((bubbleRadius - radius) / static_cast<float>(bubbleRadius - centerRadius), .0f, 1.0f);
            // light intensity varies inversely with the square of the distance
            intensity *= intensity;
            if(m_version > 1) intensity = std::min<float>(intensity, 0.4);
            const uint8_t colorByte = intensity * intensityVariant;

            uint8_t pixel[4] = { colorByte, colorByte, colorByte, 0xff };
            lightImage->setPixel(x, y, pixel);
        }
    }

    TexturePtr tex = TexturePtr(new Texture(lightImage, true));
    tex->setSmooth(true);
    return tex;
}

void LightView::reset()
{
    m_lightMap.clear();
}

void LightView::setGlobalLight(const Light& light)
{
    m_globalLight = light;
}

void LightView::addLightSource(const Position& pos, const Point& center, float scaleFactor, const Light& light, const ThingPtr& thing)
{
    if(m_version == 1) {
        Point walkOffset = Point();
        if(thing && thing->isCreature()) {
            const CreaturePtr& c = thing->static_self_cast<Creature>();
            walkOffset = c->getWalkOffset();
        }

        addLightSourceV1(center + (walkOffset + Point(16, 16)) * scaleFactor, scaleFactor, light);
        return;
    }

    if(m_version == 2) {
        addLightSourceV2(pos, center, scaleFactor, light, thing);
        return;
    }
}

void LightView::addLightSourceV1(const Point& center, float scaleFactor, const Light& light)
{
    const int intensity = light.intensity,
        radius = (intensity * Otc::TILE_PIXELS * scaleFactor) * 1.25;

    Color color = Color::from8bit(light.color);

    const float brightnessLevel = intensity > 1 ? .7 : .2,
        brightness = brightnessLevel + (intensity / static_cast<float>(MAX_LIGHT_INTENSITY)) * brightnessLevel;

    color.setRed(color.rF() * brightness);
    color.setBlue(color.bF() * brightness);
    color.setGreen(color.gF() * brightness);

    if(m_blendEquation == Painter::BlendEquation_Add && !m_lightMap.empty()) {
        const LightSource prevSource = m_lightMap.back();
        if(prevSource.center == center && prevSource.color == color && prevSource.radius == radius)
            return;
    }

    LightSource source;
    source.color = color;
    source.center = center;
    source.radius = radius;
    m_lightMap.push_back(source);
}

const static auto& POSITION_TRANSLATED_FNCS = { &Position::translatedToDirection, &Position::translatedToReverseDirection };
void LightView::addLightSourceV2(const Position& pos, const Point& center, float scaleFactor, const Light& light, const ThingPtr& thing)
{
    int intensity = light.intensity;
    if(intensity > MAX_LIGHT_INTENSITY) {
        const auto& awareRange = m_mapView->m_awareRange;
        intensity = std::max<int>(awareRange.right, awareRange.bottom) + 2;
    }

#if DEBUG_BUBBLE == 1
    const float extraRadius = 1;
#else
    const float extraRadius = intensity > 1 ? 1.8 + std::min<float>(intensity, MAX_LIGHT_INTENSITY) / 10 : 1.1;
#endif

    const int radius = (Otc::TILE_PIXELS * scaleFactor) * extraRadius;
    const Position posTile = pos.isValid() ? pos : m_mapView->getPosition(center, m_mapView->m_srcRect.size());

    std::pair<Point, Point> extraOffset = std::make_pair(Point(), Point());
    CreaturePtr creature;
    bool checkAround = false;
    if(thing && thing->isCreature()) {
        creature = thing->static_self_cast<Creature>();
        checkAround = intensity >= 4;
        extraOffset.first = Point(16, 16) * scaleFactor;
        extraOffset.second = (creature->getWalkOffset() + Point(16, 16)) * scaleFactor;
    }

    const auto& dimension = getDimensionConfig(intensity);
    for each(const auto & position in dimension.positions)
    {
        const auto posLight = posTile.translated(position.x, position.y);
        auto& lightSource = getLightSource(posLight);

        if(!lightSource.isValid() || lightSource.hasLight() && lightSource.intensity >= intensity) continue;

        float brightness = position.brightness;
        if(!canDraw(posLight, brightness)) continue;

        Color color = Color::from8bit(light.color);
        color.setRed(color.rF() * brightness);
        color.setBlue(color.bF() * brightness);
        color.setGreen(color.gF() * brightness);

        LightSource newLightSource;
        newLightSource.pos = posLight;
        newLightSource.color = color;
        newLightSource.radius = radius;
        newLightSource.canMove = lightSource.canMove;
        newLightSource.intensity = intensity;
        newLightSource.dimension = dimension;

        newLightSource.center = center + ((position.point * Otc::TILE_PIXELS) * scaleFactor);
        newLightSource.extraOffset = extraOffset;
        lightSource = newLightSource;
    }

    if(checkAround) {
        for each(const auto & position in dimension.positions) {
            auto posLight = posTile.translated(position.x, position.y);
            auto& lightSource = getLightSource(posLight);
            if(!lightSource.isValid()) continue;

            for(auto fnc : POSITION_TRANSLATED_FNCS)
            {
                const auto posCheck = std::invoke(fnc, posLight, creature->getDirection());

                if(lightSource.canMove) {
                    float bright = 0;
                    lightSource.canMove = canDraw(posCheck, bright);
                }

                auto& nextLightSource = getLightSource(posCheck);
                if(nextLightSource.isValid())
                    nextLightSource.canMove = lightSource.canMove;
            }
        }
    }
}

const DimensionConfig LightView::getDimensionConfig(const uint8 intensity)
{
#if DEBUG_BUBBLE == 1
    const float startBrightness = 3;
#else
    const float startBrightness = intensity == 1 ? .15 : .35;
#endif

    auto& dimension = m_dimensionCache[intensity];
    if(dimension.positions.empty()) {
        const int size = std::max<int>(1, std::floor(static_cast<float>(intensity) / 1.1)),
            start = size * -1,
            middle = (size / 2);

        // TODO: REFATORATION REQUIRED
        // Ugly algorithm
        {
            auto pushLight = [&](const int x, const int y) -> void {
                const float brightness = startBrightness - ((std::max<float>(std::abs(x), std::abs(y)) * 1.5) / 50);

                PositionLight posLight = PositionLight(x, y, brightness);
                posLight.point = Point(x, y);

                dimension.positions.push_back(posLight);
            };

            int i = 1;
            for(int x = start; x < 0; ++x) {
                for(int y = i * -1; y <= i; ++y) {
                    if(x == start || y == start || y == size) continue;
                    pushLight(x, y);
                }
                ++i;
            }

            i = 1;
            for(int x = size; x >= 0; --x) {
                for(int y = i * -1; y <= i; ++y) {
                    if(y >= size || y <= start || x == size) continue;
                    pushLight(x, y);
                }
                ++i;
            }
        }

        for each(auto & pos in dimension.positions)
        {
            for each(auto & posAround in pos.getPositionsAround())
            {
                if(std::find(dimension.positions.begin(), dimension.positions.end(), posAround) == dimension.positions.end()) {
                    dimension.edges.push_back(pos);
                    break;
                }
            }
        }
    }

    return dimension;
}

static auto INVALID_LIGHT_SOURCE = LightSource(-1);
LightSource& LightView::getLightSource(const Position& pos)
{
    const auto& point = m_mapView->transformPositionTo2D(pos, m_mapView->getCameraPosition());
    size_t index = (m_mapView->m_drawDimension.width() * (point.y / Otc::TILE_PIXELS)) + (point.x / Otc::TILE_PIXELS);

    if(index >= m_lightMap.size()) return INVALID_LIGHT_SOURCE;

    return m_lightMap[index];
}

bool LightView::canDraw(const Position& pos, float& brightness)
{
    TilePtr tile = g_map.getTile(pos);
    if(!tile || tile->isCovered() || tile->isTopGround() && !tile->hasBottomToDraw() || !tile->hasGround()) {
        return false;
    }

    Position tilePos = pos;
    while(tilePos.coveredUp() && tilePos.z >= m_mapView->getCachedFirstVisibleFloor()) {
        tile = g_map.getTile(tilePos);
        if(tile) {
            if(tile->blockLight() || tile->isTopGround()) {
                return false;
            }

            brightness -= 0.05;
        }
    }

    return true;
}

void LightView::drawGlobalLight(const Light& light)
{
    Color color = Color::from8bit(light.color);
    const float brightness = light.intensity / static_cast<float>(MAX_AMBIENT_LIGHT_INTENSITY);
    color.setRed(color.rF() * brightness);
    color.setGreen(color.gF() * brightness);
    color.setBlue(color.bF() * brightness);

    g_painter->setColor(color);
    g_painter->drawFilledRect(Rect(0, 0, m_lightbuffer->getSize()));
}

void LightView::drawLightSource(const LightSource& light)
{
    // debug draw
    //radius /= 16;

    const Rect dest = Rect(light.center - Point(light.radius, light.radius), Size(light.radius * 2, light.radius * 2));
    g_painter->setColor(light.color);
    g_painter->drawTexturedRect(dest, m_lightTexture);
}

void LightView::resize()
{
    m_lightbuffer->resize(m_mapView->m_frameCache.tile->getSize());

    if(m_version == 2) {
        m_lightMap.resize(m_mapView->m_drawDimension.area());
    }
}

void LightView::draw(const Rect& dest, const Rect& src)
{
    // draw light, only if there is darkness
    if(!isDark() || m_lightbuffer->getTexture() == nullptr) return;

    g_painter->saveAndResetState();
    if(m_lightbuffer->canUpdate()) {
        m_lightbuffer->bind();
        g_painter->setCompositionMode(Painter::CompositionMode_Replace);

        drawGlobalLight(m_globalLight);

        g_painter->setBlendEquation(m_blendEquation);
        g_painter->setCompositionMode(Painter::CompositionMode_Add);

        if(m_version == 1) {
            for(const LightSource& source : m_lightMap)
                drawLightSource(source);

            m_lightMap.clear();
        } else if(m_version == 2) {
            for(LightSource& source : m_lightMap) {
                if(source.pos.isValid()) {
                    source.center += (source.canMove ? source.extraOffset.second : source.extraOffset.first);

                    drawLightSource(source);
                    source.reset();
                }
            }
        }

        m_lightbuffer->release();
    }
    g_painter->setCompositionMode(Painter::CompositionMode_Light);

    m_lightbuffer->draw(dest, src);
    g_painter->restoreSavedState();
}
