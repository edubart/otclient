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

#include <framework/graphics/framebuffer.h>
#include <framework/graphics/framebuffermanager.h>
#include <framework/graphics/image.h>
#include <framework/graphics/painter.h>
#include "lightview.h"
#include "mapview.h"
#include "map.h"

LightView::LightView(const MapViewPtr& mapView)
{
    m_mapView = mapView;
    m_lightbuffer = g_framebuffers.createFrameBuffer();

    generateLightTexture();
    generateShadeTexture();

    resize();
}

void LightView::generateLightTexture()
{
    float brightnessIntensity = 1.3f,
        centerFactor = .0f;

    const uint16 bubbleRadius = 256,
        centerRadius = bubbleRadius * centerFactor,
        bubbleDiameter = bubbleRadius * 2;

    ImagePtr lightImage = ImagePtr(new Image(Size(bubbleDiameter, bubbleDiameter)));
    for(int_fast16_t x = -1; ++x < bubbleDiameter;) {
        for(int_fast16_t y = -1; ++y < bubbleDiameter;) {
            const float radius = std::sqrt((bubbleRadius - x) * (bubbleRadius - x) + (bubbleRadius - y) * (bubbleRadius - y));
            float intensity = stdext::clamp<float>((bubbleRadius - radius) / static_cast<float>(bubbleRadius - centerRadius), .0f, 1.0f);

            // light intensity varies inversely with the square of the distance
            const uint8_t colorByte = std::min<int16>((intensity * intensity * brightnessIntensity) * 0xff, 0xff);

            uint8_t pixel[4] = { 0xff, 0xff, 0xff, colorByte };
            lightImage->setPixel(x, y, pixel);
        }
    }

    m_lightTexture = TexturePtr(new Texture(lightImage));
    m_lightTexture->setSmooth(true);
}

void LightView::generateShadeTexture()
{
    const uint16 diameter = 10;
    const ImagePtr image = ImagePtr(new Image(Size(diameter, diameter)));
    for(int_fast16_t x = -1; ++x < diameter;) {
        for(int_fast16_t y = -1; ++y < diameter;) {
            const uint8 alpha = x == 0 || y == 0 || x == diameter - 1 || y == diameter - 1 ? 0 : 0xff;
            uint8_t pixel[4] = { 0xff, 0xff, 0xff, alpha };
            image->setPixel(x, y, pixel);
        }
    }

    m_shadeTexture = TexturePtr(new Texture(image));
    m_shadeTexture->setSmooth(true);
}

void LightView::addLightSource(const Point& pos, const Light& light)
{
    const uint16 radius = light.intensity * Otc::TILE_PIXELS * m_mapView->m_scaleFactor;

    auto& lights = m_lights[m_currentFloor];
    if(!lights.empty()) {
        auto& prevLight = lights.back();
        if(prevLight.pos == pos && prevLight.color == light.color) {
            prevLight.radius = std::max<uint16>(prevLight.radius, radius);
            return;
        }
    }

    lights.push_back(LightSource{ pos , light.color, radius, light.brightness });
}

void LightView::setShade(const Point& point)
{
    size_t index = (m_mapView->m_drawDimension.width() * (point.y / m_mapView->m_tileSize)) + (point.x / m_mapView->m_tileSize);
    if(index >= m_shades.size()) return;
    m_shades[index] = ShadeBlock{ m_currentFloor, point };
}

void LightView::drawLights()
{
    const auto& shadeBase = std::make_pair<Point, Size>(Point(m_mapView->m_tileSize / 4.8), Size(m_mapView->m_tileSize * 1.4));

    g_painter->setColor(m_globalLightColor);
    g_painter->drawFilledRect(m_mapView->m_rectDimension);
    for(int_fast8_t z = m_mapView->m_floorMax; z >= m_mapView->m_floorMin; --z) {
        if(z < m_mapView->m_floorMax) {
            g_painter->setColor(m_globalLightColor);
            for(auto& shade : m_shades) {
                if(shade.floor != z) continue;
                shade.floor = -1;

                g_painter->drawTexturedRect(Rect(shade.pos - shadeBase.first, shadeBase.second), m_shadeTexture);
            }
        }

        auto& lights = m_lights[z];
        std::sort(lights.begin(), lights.end(), orderLightComparator);
        for(LightSource& light : lights) {
            g_painter->setColor(Color::from8bit(light.color, light.brightness));
            g_painter->drawTexturedRect(Rect(light.pos - Point(light.radius), Size(light.radius * 2)), m_lightTexture);
        }
        lights.clear();
    }
}

void LightView::resize()
{
    m_lightbuffer->resize(m_mapView->m_frameCache.tile->getSize());
    m_shades.resize(m_mapView->m_drawDimension.area());
}

void LightView::draw(const Rect& dest, const Rect& src)
{
    // draw light, only if there is darkness
    if(!isDark()) return;

    if(m_lightbuffer->canUpdate()) {
        m_lightbuffer->bind();
        drawLights();
        m_lightbuffer->release();
    }

    g_painter->setCompositionMode(Painter::CompositionMode_Light);
    m_lightbuffer->draw(dest, src);
    g_painter->resetCompositionMode();
}