/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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

#include "particle.h"
#include "graphics.h"
#include <framework/core/clock.h>

Particle::Particle(const Point& pos, const Size& startSize, const Size& finalSize, const PointF& velocity, const PointF& acceleration, float duration, float ignorePhysicsAfter, const std::vector<Color>& colors, const std::vector<float>& colorsStops, Painter::CompositionMode compositionMode, TexturePtr texture)
{
    m_colors = colors;
    m_colorsStops = colorsStops;

    m_pos = PointF(pos.x, pos.y);
    m_startSize = startSize;
    m_finalSize = finalSize;
    m_velocity = velocity;
    m_acceleration = acceleration;

    m_compositionMode = compositionMode;
    m_texture = texture;
    m_duration = duration;
    m_ignorePhysicsAfter = ignorePhysicsAfter;
    m_elapsedTime = 0;
    m_finished = false;
}

void Particle::render()
{
    g_painter.setColor(m_color);

    if(!m_texture)
        g_painter.drawFilledRect(m_rect);
    else {
        g_painter.setCompositionMode(m_compositionMode);
        g_painter.drawTexturedRect(m_rect, m_texture);
        g_painter.setCompositionMode(Painter::CompositionMode_Normal);
    }
}

void Particle::update(double elapsedTime)
{
    // check if finished
    if(m_duration >= 0 && m_elapsedTime >= m_duration) {
        m_finished = true;
        return;
    }

    updateColor();
    updateSize();
    updatePosition(elapsedTime);

    m_elapsedTime += elapsedTime;
}

void Particle::updatePosition(double elapsedTime)
{
    bool mustRedraw = false;

    if(m_ignorePhysicsAfter < 0 || m_elapsedTime < m_ignorePhysicsAfter ) {
        // update position
        PointF delta = m_velocity * elapsedTime;
        delta.y *= -1; // painter orientate Y axis in the inverse direction

        PointF position = m_pos + delta;

        if(m_pos != position) {
            mustRedraw = true;
            m_pos += delta;
        }

        // update acceleration
        m_velocity += m_acceleration * elapsedTime;
    }

    m_rect.move((int)m_pos.x - m_size.width() / 2, (int)m_pos.y - m_size.height() / 2);
}

void Particle::updateSize()
{
    bool mustRedraw = false;

    Size size = m_startSize + (m_finalSize - m_startSize) / m_duration * m_elapsedTime;
    if(m_size != size) {
        mustRedraw = true;
        m_size = size;
    }

    m_rect.resize(m_size);
}

void Particle::updateColor()
{
    bool mustRedraw = false;

    if(m_elapsedTime < m_colorsStops[1]) {
        Color color = Color(m_colors[0].r() + (m_colors[1].r() - m_colors[0].r()) / (m_colorsStops[1] - m_colorsStops[0]) * (m_elapsedTime - m_colorsStops[0]),
                            m_colors[0].g() + (m_colors[1].g() - m_colors[0].g()) / (m_colorsStops[1] - m_colorsStops[0]) * (m_elapsedTime - m_colorsStops[0]),
                            m_colors[0].b() + (m_colors[1].b() - m_colors[0].b()) / (m_colorsStops[1] - m_colorsStops[0]) * (m_elapsedTime - m_colorsStops[0]),
                            m_colors[0].a() + (m_colors[1].a() - m_colors[0].a()) / (m_colorsStops[1] - m_colorsStops[0]) * (m_elapsedTime - m_colorsStops[0]));
        if(m_color != color) {
            mustRedraw = true;
            m_color = color;
        }
    }
    else {
        if(m_colors.size() > 1) {
            m_colors.erase(m_colors.begin());
            m_colorsStops.erase(m_colorsStops.begin());
        }
        else {
            if(m_color != m_colors[0]) {
                mustRedraw = true;
                m_color = m_colors[0];
            }
        }
    }
}
