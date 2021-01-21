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

#define DEBUG_BUBBLE 0

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

    const int bubbleRadius = 256;
    const int centerRadius = bubbleRadius * centerFactor;
    const int bubbleDiameter = bubbleRadius * 2;
    ImagePtr lightImage = ImagePtr(new Image(Size(bubbleDiameter, bubbleDiameter)));

    for(int x = 0; x < bubbleDiameter; ++x) {
        for(int y = 0; y < bubbleDiameter; ++y) {
            const float radius = std::sqrt((bubbleRadius - x) * (bubbleRadius - x) + (bubbleRadius - y) * (bubbleRadius - y));
            float intensity = stdext::clamp<float>((bubbleRadius - radius) / static_cast<float>(bubbleRadius - centerRadius), .0f, 1.0f);
            // light intensity varies inversely with the square of the distance
            intensity *= intensity;
            if(m_version > 1) intensity = std::min<float>(intensity, 0.6);
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
    } else if(m_version == 2) {
        addLightSourceV2(pos, center, scaleFactor, light, thing);
    }
}

void LightView::addLightSourceV1(const Point& center, float scaleFactor, const Light& light)
{
    const int intensity = light.intensity;
    const int radius = (intensity * Otc::TILE_PIXELS * scaleFactor) * 2.5;

    Color color = Color::from8bit(light.color);

    const float brightnessLevel = intensity > 1 ? 0.7 : 0.2f;
    const float brightness = brightnessLevel + (intensity / static_cast<float>(MAX_LIGHT_INTENSITY)) * brightnessLevel;

    color.setRed(color.rF() * brightness);
    color.setGreen(color.gF() * brightness);
    color.setBlue(color.bF() * brightness);

    if(m_blendEquation == Painter::BlendEquation_Add && !m_lightMap.empty()) {
        const LightSource prevSource = m_lightMap.back();
        if(prevSource.center == center && prevSource.color == color && prevSource.radius == radius)
            return;
    }

    LightSource source;
    source.center = center;
    source.color = color;
    source.radius = radius;
    m_lightMap.push_back(source);
}

void LightView::addLightSourceV2(const Position& pos, const Point& center, float scaleFactor, const Light& light, const ThingPtr& thing)
{
    const uint defaultIntensity = 3;

    int intensity = light.intensity;
    if(intensity > MAX_LIGHT_INTENSITY) {
        const auto& awareRange = m_mapView->m_awareRange;
        intensity = std::max<int>(awareRange.right, awareRange.bottom) + 2;
    }

#if DEBUG_BUBBLE == 1
    const int extraRadius = 1;
    const float brightnessLevel = 0.5;
#else
    int extraRadius;
    float brightnessLevel;
    if(intensity > 1) {
        extraRadius = 2.2;
        brightnessLevel = 0.2;
    } else {
        extraRadius = 1.3;
        brightnessLevel = 0.1;
    }

#endif

    const int radius = (Otc::TILE_PIXELS * scaleFactor) * extraRadius;
    const float brightness = brightnessLevel + (defaultIntensity / static_cast<float>(MAX_LIGHT_INTENSITY)) * brightnessLevel;
    Position posTile = pos.isValid() ? pos : m_mapView->getPosition(center, m_mapView->m_srcRect.size());
    const Point cleanPoint = Point(16, 16) * scaleFactor;


    std::pair<Point, Point> extraOffset = std::make_pair(Point(), Point());
    CreaturePtr creature;
    bool isMoving = false, checkAround = false;
    if(thing && thing->isCreature()) {
        creature = thing->static_self_cast<Creature>();
        if(!creature->getWalkOffset().isNull()) {
            isMoving = true;
        }

        extraOffset.first = Point(16, 16);
        extraOffset.second = creature->getWalkOffset() + extraOffset.first;
        extraOffset.first *= scaleFactor;
        extraOffset.second *= scaleFactor;
        checkAround = intensity >= 4;
    }

    Color color = Color::from8bit(light.color);
    color.setRed(color.rF() * brightness);
    color.setGreen(color.gF() * brightness);
    color.setBlue(color.bF() * brightness);

    const auto& dimension = getDimensionConfig(intensity);
    for each(const auto position in dimension.positions)
    {
        const auto x = position.x;
        const auto y = position.y;
        Position virtualPos = Position(x, y, 255);

        auto posLight = posTile.translated(x, y);
        if(!canDraw(posLight)) continue;

        int index = getLightSourceIndex(posLight);
        if(index == -1) continue;

        auto& lightSource = m_lightMap[index];
        if(lightSource.hasLight() && lightSource.originalIntensity > intensity) continue;

        LightSource newLightSource;
        newLightSource.color = color;
        newLightSource.radius = radius;
        newLightSource.pos = posLight;
        newLightSource.intensity = defaultIntensity;
        newLightSource.originalIntensity = intensity;
        newLightSource.canMove = lightSource.canMove;
        newLightSource.dimension = dimension;

        newLightSource.center = center + ((Point(x, y) * Otc::TILE_PIXELS) * scaleFactor);
        newLightSource.extraOffset = extraOffset;
        lightSource = newLightSource;
    }

    if(checkAround) {
        for each(const auto position in dimension.positions) {
            const auto x = position.x;
            const auto y = position.y;
            Position virtualPos = Position(x, y, 255);

            auto posLight = posTile.translated(position.x, position.y);
            int index = getLightSourceIndex(posLight);
            if(index == -1) continue;

            auto& lightSource = m_lightMap[index];

            const auto& arr = { &Position::translatedToDirection, &Position::translatedToReverseDirection };
            for(auto fnc : arr)
            {
                const auto posCheck = std::invoke(fnc, posLight, creature->getDirection());

                if(lightSource.canMove)
                    lightSource.canMove = canDraw(posCheck);

                index = getLightSourceIndex(posCheck);
                if(index > -1) {
                    auto& nextLightSource = m_lightMap[index];
                    nextLightSource.canMove = lightSource.canMove;
                }
            }
        }
    }
}

DimensionConfig LightView::getDimensionConfig(const uint8 intensity)
{
    auto& dimension = m_dimensionCache[intensity];
    if(dimension.positions.empty()) {
        const int size = std::max<int>(1, std::floor(static_cast<float>(intensity) / 1.3)),
            start = size * -1,
            middle = size / 2;

        dimension.min = 0;
        dimension.max = size;

        for(int x = start; x <= size; ++x) {
            for(int y = start; y <= size; ++y) {
                const int absY = std::abs(y);
                const int absX = std::abs(x);

                if(absX == size && absY >= 1 || absY == size && absX >= 1) continue;
                if(absY > middle && absX > middle && (
                    absY == absX || absX - middle == absY || absX == absY - middle || absX - middle == absY - middle
                    ) || absX == size && absY == 0 || absY == size && absX == 0) continue;

                if(x < dimension.min)
                    dimension.min = x;

                if(x > dimension.max)
                    dimension.max = x;

                dimension.positions.push_back(Position(x, y, 255));
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

int LightView::getLightSourceIndex(const Position& pos)
{
    const auto& point = m_mapView->transformPositionTo2D(pos, m_mapView->getCameraPosition());
    size_t index = (m_mapView->m_drawDimension.width() * (point.y / Otc::TILE_PIXELS)) + (point.x / Otc::TILE_PIXELS);

    if(index >= m_lightMap.size()) return -1;

    return index;
}

bool LightView::canDraw(const Position& pos)
{
    TilePtr tile = g_map.getTile(pos);
    if(!tile || tile->isCovered() || tile->isTopGround() && !tile->hasBottomToDraw() || !tile->hasGround()) {
        return false;
    }


    Position tilePos = pos;
    while(tilePos.coveredUp() && tilePos.z >= m_mapView->getCachedFirstVisibleFloor()) {
        TilePtr tile = g_map.getTile(tilePos);
        if(tile && (tile->blockLight() || tile->isTopGround())) return false;
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

                    g_painter->setOpacity(0.1);
                    drawLightSource(source);
                    g_painter->resetOpacity();
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
