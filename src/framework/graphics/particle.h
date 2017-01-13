/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
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

#ifndef PARTICLE_H
#define PARTICLE_H

#include "declarations.h"
#include "painter.h"

class Particle : public stdext::shared_object
{
public:
    Particle(const Point& pos, const Size& startSize, const Size& finalSize, const PointF& velocity, const PointF& acceleration, float duration, float ignorePhysicsAfter, const std::vector<Color>& colors, const std::vector<float>& colorsStops, Painter::CompositionMode compositionMode = Painter::CompositionMode_Normal, TexturePtr texture = nullptr);

    void render();
    void update(float elapsedTime);

    bool hasFinished() { return m_finished; }

    PointF getPosition() { return m_position; }
    PointF getVelocity() { return m_velocity; }

    void setPosition(const PointF& position) { m_position = position; }
    void setVelocity(const PointF& velocity) { m_velocity = velocity; }

private:
    void updateColor();
    void updatePosition(float elapsedTime);
    void updateSize();

    Color m_color;
    std::vector<Color> m_colors;
    std::vector<float> m_colorsStops;
    TexturePtr m_texture;
    PointF m_position;
    PointF m_velocity;
    PointF m_acceleration;
    Size m_size, m_startSize, m_finalSize;
    Rect m_rect;
    Painter::CompositionMode m_compositionMode;
    float m_duration, m_ignorePhysicsAfter;
    float m_elapsedTime;
    bool m_finished;
};

#endif
