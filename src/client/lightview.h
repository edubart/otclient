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

#include <framework/graphics/framebuffer.h>
#include <framework/graphics/declarations.h>
#include <framework/graphics/painter.h>
#include "declarations.h"
#include "thingtype.h"

enum : uint8 {
    MAX_LIGHT_INTENSITY = 8,
    MAX_AMBIENT_LIGHT_INTENSITY = _UI8_MAX
};

struct PositionLight : Position {
    const float brightness;
    const Point point;
    bool isEdge = false;

    PositionLight(int8 x, int8 y, float brightness) :
        brightness(brightness), point(Point(x, y))
    {
        Position::x = x, Position::y = y, z = _UI8_MAX;
    }
};

struct DimensionConfig {
    std::vector<PositionLight>
        positions,
        edges;

    bool isEdge(const Position pos) const
    {
        return std::find(edges.begin(), edges.end(), pos) != edges.end();
    }
};

struct LightSource {
    Point center;
    uint8 color;
    float brightness;
    uint16 radius;
    bool isEdge;
    bool isStatic;
};

struct TileBrightness {
    int8 floor = -1;
    Point pos;
    bool isEdge;
};

struct LightPoint {
    LightPoint(const bool valid = true) : isValid(valid) {}

    bool isValid;
    TileBrightness brightness;

    std::array<std::vector<LightSource>, Otc::MAX_Z + 1 > floors;

    bool isCovered(const uint8 floor)
    {
        return !brightness.isEdge && brightness.floor > -1 && floor > brightness.floor;
    }
};

class LightView : public LuaObject
{
public:
    LightView(const MapViewPtr& mapView);

    const Light& getGlobalLight() { return m_globalLight; }

    void resize();
    void draw(const Rect& dest, const Rect& src);
    void addLightSource(const Point& mainCenter, const Light& light, const bool isStatic = false);

    void setGlobalLight(const Light& light) { m_globalLight = light; }
    void schedulePainting(const uint16_t delay = FrameBuffer::MIN_TIME_UPDATE) const { if(isDark()) m_lightbuffer->schedulePainting(delay); }

    bool canUpdate() const { return isDark() && m_lightbuffer->canUpdate(); }
    bool isDark() const { return m_globalLight.intensity < 250; }
    void resetBrightness(const Point& point);
    void setFloor(const uint8 floor) { m_currentFloor = floor; }

private:
    const DimensionConfig& getDimensionConfig(const uint8 intensity);

    void generateLightTexture();
    void generateBrightnessTexture();
    void drawLights();

    LightPoint& getLightPoint(const Point& point);

    TexturePtr m_lightTexture;
    TexturePtr m_brightnessTexture;
    Light m_globalLight;

    FrameBufferPtr m_lightbuffer;
    MapViewPtr m_mapView;

    uint8 m_currentFloor;
    std::array<DimensionConfig, MAX_LIGHT_INTENSITY> m_dimensionCache;
    std::vector<LightPoint> m_lightMap;
};

#endif
