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

LightView::LightView(const MapViewPtr& mapView)
{
    m_mapView = mapView;
    m_lightbuffer = g_framebuffers.createFrameBuffer();

    generateLightTexture();
    generateBrightnessTexture();

    resize();
}

void LightView::generateBrightnessTexture()
{
    const uint16 diameter = 6;
    const ImagePtr lightImage = ImagePtr(new Image(Size(diameter, diameter)));
    for(int_fast16_t x = -1; ++x < diameter;) {
        for(int_fast16_t y = -1; ++y < diameter;) {
            const uint8 alpha = x == 0 || y == 0 || x == diameter - 1 || y == diameter - 1 ? 0 : 255;
            uint8_t pixel[4] = { 255, 255, 255, alpha };
            lightImage->setPixel(x, y, pixel);
        }
    }

    m_brightnessTexture = TexturePtr(new Texture(lightImage, true));
    m_brightnessTexture->setSmooth(true);
}

void LightView::generateLightTexture()
{
    const float centerFactor = .0;
    const uint16 bubbleRadius = 5,
        centerRadius = bubbleRadius * centerFactor,
        bubbleDiameter = bubbleRadius * 2;

    const ImagePtr lightImage = ImagePtr(new Image(Size(bubbleDiameter, bubbleDiameter)));
    for(int_fast16_t x = -1; ++x < bubbleDiameter;) {
        for(int_fast16_t y = -1; ++y < bubbleDiameter;) {
            const float radius = std::sqrt((bubbleRadius - x) * (bubbleRadius - x) + (bubbleRadius - y) * (bubbleRadius - y));
            const float intensity = stdext::clamp<float>((bubbleRadius - radius) / static_cast<float>(bubbleRadius - centerRadius), .0f, 1.0f);

            // light intensity varies inversely with the square of the distance
            const uint8_t colorByte = std::min<float>(intensity * intensity, 0.4) * 0xFF;

            uint8_t pixel[4] = { 255, 255, 255, colorByte };
            lightImage->setPixel(x, y, pixel);
        }
    }

    m_lightTexture = TexturePtr(new Texture(lightImage, true));
    m_lightTexture->setSmooth(true);
}

void LightView::addLightSource(const Point& mainCenter, const Light& light)
{
    const uint8 intensity = std::min<uint8>(light.intensity, MAX_LIGHT_INTENSITY);

    const uint16 radius = (Otc::TILE_PIXELS * m_mapView->m_scaleFactor) * (intensity > 1 ? 2.3 : 1.1);
    const auto& dimension = getDimensionConfig(intensity);
    for(const auto& position : dimension.positions)
    {
        const Point& center = mainCenter + (position.point * m_mapView->m_tileSize);

        auto& lightPoint = getLightPoint(center);
        if(!lightPoint.isValid) continue;

        auto& lightList = lightPoint.floors[m_currentFloor];

        auto brightness = position.brightness;

        bool gotoNextLight = false;
        for(auto& prevLight : lightList) {
            if(prevLight.color == light.color) {
                if(prevLight.center == center) {
                    prevLight.brightness = std::min<float>(prevLight.brightness + brightness, 1);
                    gotoNextLight = true;
                }
            } else if(prevLight.color > light.color) {
                brightness -= prevLight.brightness;
            } else if(prevLight.color < light.color) {
                prevLight.brightness -= brightness;
            }
        }
        if(gotoNextLight) continue;

        lightList.push_back({ center , light.color, brightness , radius, position.isEdge });
    }
}

const DimensionConfig& LightView::getDimensionConfig(const uint8 intensity)
{
    auto& dimension = m_dimensionCache[intensity - 1];
    if(dimension.positions.empty()) {
        const uint8 size = std::max<int>(1, std::floor(static_cast<float>(intensity) / 1.1)),
            middle = (size / 2);
        const int8 start = size * -1;

        // TODO: REFATORATION REQUIRED
        // Ugly algorithm
        {
            const float startBrightness = intensity == 1 ? .15 : .5 + (static_cast<float>(intensity) / 16);
            auto pushLight = [&](const int8 x, const int8 y) -> void {
                const float brightness = startBrightness - ((std::max<float>(std::abs(x), std::abs(y))) / intensity);
                dimension.positions.push_back({ x, y, std::min<float>(brightness, 1) });
            };

            uint8 i = 1;
            for(int_fast8_t x = start; x < 0; ++x) {
                for(int_fast8_t y = i * -1; y <= i; ++y) {
                    if(x == start || y == start || y == size) continue;
                    pushLight(x, y);
                }
                ++i;
            }

            i = 1;
            for(int_fast8_t x = size; x >= 0; --x) {
                for(int_fast8_t y = i * -1; y <= i; ++y) {
                    if(y >= size || y <= start || x == size) continue;
                    pushLight(x, y);
                }
                ++i;
            }
        }

        for(auto& pos : dimension.positions)
        {
            for(const auto& posAround : pos.getPositionsAround())
            {
                if(std::find(dimension.positions.begin(), dimension.positions.end(), posAround) == dimension.positions.end()) {
                    dimension.edges.push_back(pos);
                    pos.isEdge = true;
                    break;
                }
            }
        }
    }

    return dimension;
}

static LightPoint INVALID_LIGHT_POINT(false);
LightPoint& LightView::getLightPoint(const Point& point)
{
    size_t index = ((m_mapView->m_drawDimension.width() + static_cast<uint8>(2)) * (point.y / m_mapView->m_tileSize)) + (point.x / m_mapView->m_tileSize);
    if(index >= m_lightMap.size()) return INVALID_LIGHT_POINT;
    return m_lightMap[index];
}

void LightView::resetBrightness(const Point& point)
{
    auto& lightPoint = getLightPoint(point);
    if(lightPoint.isValid)
        lightPoint.brightness = { static_cast<int8>(m_currentFloor), point, false };
}

void LightView::drawLights()
{
    // GlobalLight
    Color globalColor = Color::from8bit(m_globalLight.color);
    g_painter->setCompositionMode(Painter::CompositionMode_Replace);
    {
        const float brightness = m_globalLight.intensity / static_cast<float>(MAX_AMBIENT_LIGHT_INTENSITY);
        globalColor.setRed(globalColor.rF() * brightness);
        globalColor.setGreen(globalColor.gF() * brightness);
        globalColor.setBlue(globalColor.bF() * brightness);
        g_painter->setColor(globalColor);
    }
    g_painter->drawFilledRect(Rect(0, 0, m_lightbuffer->getSize()));

    const auto& compareLights = [](const LightSource& a, const LightSource& b) -> bool const { return a.color < b.color; };

    // Lights
    g_painter->setCompositionMode(Painter::CompositionMode_Normal);
    g_painter->setBlendEquation(Painter::BlendEquation_Add);
    for(LightPoint& lightPoint : m_lightMap) {
        if(lightPoint.brightness.floor == -1) continue;

        for(auto& pointAround : lightPoint.brightness.pos.getPointsAround(m_mapView->m_tileSize)) {
            const auto& lightPointAround = getLightPoint(lightPoint.brightness.pos + (Point(pointAround.diffX, pointAround.diffY) * m_mapView->m_tileSize));
            if(lightPoint.brightness.floor != lightPointAround.brightness.floor) {
                lightPoint.brightness.isEdge = true;
                break;
            }
        }
    }

    for(int_fast8_t z = m_mapView->m_floorMax; z >= m_mapView->m_floorMin; --z) {
        if(z < m_mapView->m_floorMax) {
            g_painter->setColor(globalColor);
            for(LightPoint& lightPoint : m_lightMap) {
                if(lightPoint.brightness.floor != z) continue;
                lightPoint.brightness.floor = -1;

                const uint8 size = m_mapView->m_tileSize;
                const Rect dest = Rect(lightPoint.brightness.pos - Point(size, size) / 1.9, Size(size, size) * 1.9);
                g_painter->drawTexturedRect(dest, m_brightnessTexture);
            }
        }

        for(auto& lightPoint : m_lightMap) {
            auto& lights = lightPoint.floors[z];
            if(!lightPoint.isCovered(z)) {
                if(lights.size() > 1) {
                    std::sort(lights.begin(), lights.end(), compareLights);
                }

                for(const auto& light : lights) {
                    if(light.brightness <= 0.1) {
                        continue;
                    }

                    Color color = Color::from8bit(light.color);
                    color.setAlpha(light.brightness);
                    g_painter->setColor(color);
                    g_painter->drawTexturedRect(Rect(light.center - Point(light.radius, light.radius), Size(light.radius, light.radius) * 2), m_lightTexture);
                }
            }

            lights.clear();
        }
    }
}

void LightView::resize()
{
    m_lightbuffer->resize(m_mapView->m_frameCache.tile->getSize());
    m_lightMap.resize(m_mapView->m_drawDimension.area());
}

void LightView::draw(const Rect& dest, const Rect& src)
{
    // draw light, only if there is darkness
    if(!isDark()) return;

    g_painter->saveAndResetState();
    if(m_lightbuffer->canUpdate()) {
        m_lightbuffer->bind();
        drawLights();
        m_lightbuffer->release();
    }
    g_painter->setCompositionMode(Painter::CompositionMode_Light);

    m_lightbuffer->draw(dest, src);
    g_painter->restoreSavedState();
}