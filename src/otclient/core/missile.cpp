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

#include "missile.h"
#include "thingstype.h"
#include "map.h"
#include "tile.h"
#include <framework/core/clock.h>
#include <framework/core/eventdispatcher.h>

Missile::Missile() : Thing()
{
    m_startTicks = 0;
}

void Missile::draw(const Point& p)
{
    float time = (g_clock.ticks() - m_startTicks) / m_duration;
    internalDraw(p + Point(m_positionDelta.x * time, m_positionDelta.y * time), 0);
}

void Missile::setPath(const Position& fromPosition, const Position& toPosition)
{
    m_position = fromPosition;
    m_positionDelta = toPosition - fromPosition;

    if(m_positionDelta.x == 0 && m_positionDelta.y == 0) {
        m_xPattern = 1;
        m_yPattern = 1;
    }
    else if(m_positionDelta.x == 0) {
        m_xPattern = 1;
        if(m_positionDelta.y < 0) {
            m_yPattern = 0;
        }
        else if(m_positionDelta.y > 0) {
            m_yPattern = 2;
        }
    }
    else if(m_positionDelta.y == 0) {
        m_yPattern = 1;
        if(m_positionDelta.x < 0) {
            m_xPattern = 0;
        }
        else if(m_positionDelta.x > 0) {
            m_xPattern = 2;
        }
    }
    else {
        float angle = std::atan2(m_positionDelta.y * -1, m_positionDelta.x) * 180.0 / 3.141592;
        if(angle < 0)
            angle += 360;

        if(angle >= 360 - 22.5 || angle < 0 + 22.5) {
            m_xPattern = 2;
            m_yPattern = 1;
        }
        else if(angle >= 45 - 22.5 && angle < 45 + 22.5) {
            m_xPattern = 2;
            m_yPattern = 0;
        }
        else if(angle >= 90 - 22.5 && angle < 90 + 22.5) {
            m_xPattern = 1;
            m_yPattern = 0;
        }
        else if(angle >= 135 - 22.5 && angle < 135 + 22.5) {
            m_xPattern = 0;
            m_yPattern = 0;
        }
        else if(angle >= 180 - 22.5 && angle < 180 + 22.5) {
            m_xPattern = 0;
            m_yPattern = 1;
        }
        else if(angle >= 225 - 22.5 && angle < 225 + 22.5) {
            m_xPattern = 0;
            m_yPattern = 2;
        }
        else if(angle >= 270 - 22.5 && angle < 270 + 22.5) {
            m_xPattern = 1;
            m_yPattern = 2;
        }
        else if(angle >= 315 - 22.5 && angle < 315 + 22.5) {
            m_xPattern = 2;
            m_yPattern = 2;
        }
    }

    m_duration = 150 * std::sqrt(Point(m_positionDelta.x, m_positionDelta.y).length());
    m_positionDelta.x *= 32;
    m_positionDelta.y *= 32;

    m_startTicks = g_clock.ticks();

    // schedule removal
    auto self = asMissile();
    g_dispatcher.scheduleEvent([self]() {
        g_map.removeThing(self);
    }, m_duration);
}

const ThingType& Missile::getType()
{
    return g_thingsType.getThingType(m_id, ThingsType::Missile);
}
