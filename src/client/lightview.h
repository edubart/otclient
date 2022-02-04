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

#ifndef LIGHTVIEW_H
#define LIGHTVIEW_H

#include <framework/graphics/declarations.h>
#include <framework/graphics/framebuffer.h>
#include "declarations.h"
#include "thingtype.h"

struct LightSource {
    Point pos;
    uint8 color{ 0 };
    uint16 radius{ 0 };
    float brightness{ 0.f };
    float opacity{ 1.f };
};

class LightView : public LuaObject
{
public:
    LightView(const MapViewPtr& mapView);

    void resize();
    void draw(const Rect& dest, const Rect& src);

    void addLightSource(const Point& mainCenter, const Light& light);
    void addShade(const Point& point, const float opacity) { m_lights.push_back(LightSource{ point, 0, 0, 0, opacity });  m_lastPos = m_lights.size(); }

    void setGlobalLight(const Light& light) { m_globalLight = light; m_globalLightColor = Color::from8bit(m_globalLight.color, m_globalLight.intensity / static_cast<float>(UINT8_MAX)); }

    const Light& getGlobalLight() const { return m_globalLight; }
    bool isDark() const { return m_globalLight.intensity < 250; }

private:
    Light m_globalLight;
    Color m_globalLightColor;

    PoolFramedPtr m_pool;
    MapViewPtr m_mapView;

    std::vector<LightSource> m_lights;
    int m_lastPos{ 0 };
};

#endif
