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

LightView::LightView(const MapViewPtr& mapView) : m_pool(g_drawPool.createPoolF(LIGHT)), m_mapView(mapView) { resize(); }

void LightView::addLightSource(const Point& pos, const Light& light)
{
    if(!isDark()) return;

    const uint16 radius = light.intensity * SPRITE_SIZE * m_mapView->m_scaleFactor;

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

void LightView::setShade(const Point& point, const std::vector<Otc::Direction>& dirs)
{
    const size_t index = (m_mapView->m_drawDimension.width() * (point.y / m_mapView->m_tileSize)) + (point.x / m_mapView->m_tileSize);
    if(index >= m_shades.size()) return;
    auto& shade = m_shades[index];
    shade.floor = m_currentFloor;
    shade.pos = point;
    shade.dirs = dirs;
}

void LightView::resize()
{
    m_pool->resize(m_mapView->m_rectDimension.size());
    m_shades.resize(m_mapView->m_drawDimension.area());
}

void LightView::draw(const Rect& dest, const Rect& src)
{
    // draw light, only if there is darkness
    m_pool->setEnable(isDark());
    if(!isDark()) return;

    const float intensity = m_globalLight.intensity / static_cast<float>(UINT8_MAX);

    g_drawPool.use(m_pool, dest, src);
    g_drawPool.addFilledRect(m_mapView->m_rectDimension, m_globalLightColor);
    const auto& shadeBase = std::make_pair<Point, Size>(Point(m_mapView->getTileSize() / 2.8), Size(m_mapView->getTileSize() * 1.6));
    for(int_fast8_t z = m_mapView->m_floorMax; z >= m_mapView->m_floorMin; --z) {
        if(z < m_mapView->m_floorMax) {
            for(auto& shade : m_shades) {
                if(shade.floor != z) continue;
                shade.floor = -1;

                auto newPos = shade.pos;
                for(const auto dir : shade.dirs) {
                    if(dir == Otc::South)
                        newPos.y -= SPRITE_SIZE / 1.6;
                    else if(dir == Otc::East)
                        newPos.x -= SPRITE_SIZE / 1.6;
                }

                g_drawPool.addTexturedRect(Rect(newPos - shadeBase.first, shadeBase.second), g_sprites.getShadeTexture(), m_globalLightColor);
            }
        }

        auto& lights = m_lights[z];
        std::sort(lights.begin(), lights.end(), orderLightComparator);
        for(LightSource& light : lights) {
            if(light.brightness < 1.f) light.brightness = std::min<float>(light.brightness + intensity, 1.f);
            g_drawPool.addTexturedRect(Rect(light.pos - Point(light.radius), Size(light.radius * 2)), g_sprites.getLightTexture(), Color::from8bit(light.color, light.brightness));
        }
        lights.clear();
    }
}
