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
#include <framework/graphics/drawpool.h>
#include "map.h"
#include "mapview.h"
#include "spritemanager.h"

LightView::LightView() : m_pool(g_drawPool.createPoolF(LIGHT)) {}

void LightView::resize(const Size& size, const uint8_t tileSize)
{
    m_tileSize = tileSize;
    m_dimension = size;

    m_pool->resize(size * tileSize);
    m_shades.resize(size.area(), -1);
};

void LightView::addLightSource(const Point& pos, const Light& light)
{
    if(!isDark()) return;

    if(!m_lights.empty()) {
        auto& prevLight = m_lights.back();
        if(prevLight.pos == pos && prevLight.color == light.color) {
            prevLight.intensity = std::max<uint16>(prevLight.intensity, light.intensity);
            return;
        }
    }

    m_lights.push_back(LightSource{ pos , light.color, light.intensity, g_drawPool.getOpacity() });
}

void LightView::addShade(const Point& point, const float opacity)
{
    const size_t index = (m_dimension.width() * (point.y / m_tileSize)) + (point.x / m_tileSize);
    if(index >= m_shades.size()) return;

    const int indexLight = m_shades[index];
    if(indexLight > -1) {
        m_lights[indexLight].pos = {};
        m_lights[indexLight].opacity = 0;
    }

    m_shades[index] = m_lights.size();
    m_lights.push_back(LightSource{ point, 0, static_cast<uint16_t>(index), opacity });
}

// m_lights.push_back(LightSource{ point, 0, isSimpleShade, opacity });

void LightView::draw(const Rect& dest, const Rect& src)
{
    // draw light, only if there is darkness
    m_pool->setEnable(isDark());
    if(!isDark()) return;

    g_drawPool.use(m_pool, dest, src);
    g_drawPool.addFilledRect(Rect(0, 0, m_pool->getSize()), m_globalLightColor);

    const int size = 12;
    const float pos = size / 2.25f;

    const auto& shadeBase = std::make_pair<Point, Size>(Point(m_tileSize * pos), Size(m_tileSize * size));
    const auto& shadeBase2 = std::make_pair<Point, Size>(Point(m_tileSize / 1.3), Size(m_tileSize * 3.3));

    for(auto& light : m_lights) {
        if(light.color) {
            const Color color = Color::from8bit(light.color, std::min<float>(light.opacity, light.intensity / 6.f));
            const uint16 radius = light.intensity * m_tileSize;

            g_painter->setBlendEquation(Painter::BlendEquation_Max);
            g_drawPool.addTexturedRect(Rect(light.pos - Point(radius), Size(radius * 2)), g_sprites.getLightTexture(), color);
        } else if(light.opacity) {
            g_painter->setBlendEquation(Painter::BlendEquation_Add);
            g_drawPool.setOpacity(light.opacity);

            g_drawPool.addTexturedRect(Rect(light.pos - shadeBase2.first, shadeBase2.second), g_sprites.getShadeTexture(true), m_globalLightColor);
            g_drawPool.resetOpacity();

            m_shades[light.intensity] = -1;
        }
    }

    m_lights.clear();

    g_painter->setBlendEquation(Painter::BlendEquation_Add);
}
