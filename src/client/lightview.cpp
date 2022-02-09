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

void LightView::resize(const Size& size) { m_pool->resize(size); };

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

void LightView::draw(const Rect& dest, const Rect& src, const uint8 tileSize)
{
    // draw light, only if there is darkness
    m_pool->setEnable(isDark());
    if(!isDark()) return;

    g_drawPool.use(m_pool, dest, src);
    g_drawPool.addFilledRect(Rect(0, 0, m_pool->getSize()), m_globalLightColor);

    const int size = 12;
    const float pos = size / 2.25f;

    const auto& shadeBase = std::make_pair<Point, Size>(Point(tileSize * pos), Size(tileSize * size));
    const auto& shadeBase2 = std::make_pair<Point, Size>(Point(tileSize / 2.8), Size(tileSize * 1.6));

    for(auto& light : m_lights) {
        if(light.color) {
            const Color color = Color::from8bit(light.color, std::min<float>(light.opacity, light.intensity / 6.f));
            const uint16 radius = light.intensity * tileSize;

            g_painter->setBlendEquation(Painter::BlendEquation_Max);
            g_drawPool.addTexturedRect(Rect(light.pos - Point(radius), Size(radius * 2)), g_sprites.getLightTexture(), color);
        } else {
            g_painter->setBlendEquation(Painter::BlendEquation_Add);
            g_drawPool.setOpacity(light.opacity);

            auto base = light.intensity ? shadeBase2 : shadeBase;

            g_drawPool.addTexturedRect(Rect(light.pos - base.first, base.second), g_sprites.getShadeTexture(light.intensity), m_globalLightColor);
            g_drawPool.resetOpacity();
        }
    }

    m_lights.clear();

    g_painter->setBlendEquation(Painter::BlendEquation_Add);
}