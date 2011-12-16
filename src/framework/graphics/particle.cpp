/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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

Particle::Particle(const Point& pos, const Size& size, const PointF& velocity, const PointF& acceleration, float duration, const Color& color, TexturePtr texture)
{
    m_rect = Rect(pos, size);
    m_position = PointF(pos.x, pos.y);
    m_size = size;
    m_velocity = velocity;
    m_acceleration = acceleration;
    m_color = color;
    m_texture = texture;
    m_duration = duration;
    m_startTime = g_clock.time();
    m_lastUpdateTime = g_clock.time();
    m_finished = false;
}

void Particle::render()
{
    g_painter.setColor(m_color);

    if(!m_texture)
        g_painter.drawFilledRect(m_rect);
    else {
        //g_painter.setCompositionMode(Painter::CompositionMode_AdditiveSource);
        g_painter.drawTexturedRect(m_rect, m_texture);
        //g_painter.setCompositionMode(Painter::CompositionMode_SourceOver);
    }
}

void Particle::update()
{
    float elapsedTime = g_clock.timeElapsed(m_lastUpdateTime);
    m_lastUpdateTime = g_clock.time();

    // check if finished
    if(m_duration > 0 && g_clock.timeElapsed(m_startTime) >= m_duration) {
        m_finished = true;
        return;
    }

    // update position
    PointF delta = m_velocity * elapsedTime;
    delta.y *= -1; // painter orientate Y axis in the inverse direction
    m_position += delta;

    // update acceleration
    m_velocity += m_acceleration * elapsedTime;

    m_rect.moveTo((int)m_position.x - m_size.width() / 2, (int)m_position.y - m_size.height() / 2);
}
