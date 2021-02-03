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

struct PositionLight : Position {
    PositionLight(int8 x, int8 y, float brightness) : brightness(brightness) { Position::x = x, Position::y = y, z = _UI8_MAX; }
    float brightness;
    Point point;
};

struct DimensionConfig {
    std::vector<PositionLight> positions;
    std::vector<PositionLight> edges;

    bool isEdge(const Position pos) const
    {
        return std::find(edges.begin(), edges.end(), pos) != edges.end();
    }
};

struct LightSource {
    LightSource() : radius(0) {}
    LightSource(int radius) : radius(radius) {}
    Color color = Color::alpha;
    Point center;
    std::pair<Point, Point> extraOffset;
    int radius;
    uint8_t intensity;
    bool canMove = true;
    bool reverter = false;
    float brightness = 0;

    void reset() { color = Color::alpha; canMove = true; reverter = false; }
    bool hasLight() const { return color != Color::alpha; }
    bool isValid() const { return radius > -1; }
};

class LightView : public LuaObject
{
public:
    LightView(const MapViewPtr& mapView, const uint8 version);

    const Light& getGlobalLight() { return m_globalLight; }

    void reset();
    void setGlobalLight(const Light& light);
    void addLightSource(const Position& pos, const Point& center, float scaleFactor, const Light& light, const ThingPtr& thing = nullptr);
    void resize();
    void draw(const Rect& dest, const Rect& src);

    void setBlendEquation(Painter::BlendEquation blendEquation) { m_blendEquation = blendEquation; }
    void schedulePainting(const uint16_t delay = FrameBuffer::MIN_TIME_UPDATE) { if(isDark()) m_lightbuffer->schedulePainting(delay); }
    bool canUpdate() const { return isDark() && m_lightbuffer->canUpdate(); }

    bool isDark() const { return m_globalLight.intensity < 250; }

    uint8 getVersion() const { return m_version; }
private:
    void addLightSourceV1(const Point& center, float scaleFactor, const Light& light);
    void addLightSourceV2(const Position& pos, const Point& center, float scaleFactor, const Light& light, const ThingPtr& thing);
    void drawGlobalLight(const Light& light);
    void drawLightSource(const LightSource& light);
    bool canDraw(const Position& pos, float& brightness);

    const DimensionConfig getDimensionConfig(const uint8 intensity);

    Light m_globalLight;

    TexturePtr generateLightBubble();
    TexturePtr m_lightTexture;

    Painter::BlendEquation m_blendEquation;

    FrameBufferPtr m_lightbuffer;

    std::vector<LightSource> m_lightMap;
    std::array<DimensionConfig, _UI8_MAX> m_dimensionCache;
    MapViewPtr m_mapView;

    LightSource& getLightSource(const Position& pos);

    uint8 m_version;
};

#endif
