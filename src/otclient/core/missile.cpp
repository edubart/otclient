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

void Missile::draw(const Point& p, const Rect&)
{
    float time = (g_clock.ticks() - m_startTicks) / m_duration;
    internalDraw(p + Point(m_posDelta.x * time, m_posDelta.y * time), 0);
}

void Missile::setPath(const Position& fromPosition, const Position& toPosition)
{
    Otc::Direction direction = fromPosition.getDirectionFromPosition(toPosition);

    if(direction == Otc::NorthWest) {
        m_xPattern = 0;
        m_yPattern = 0;
    }
    else if(direction == Otc::North) {
        m_xPattern = 1;
        m_yPattern = 0;
    }
    else if(direction == Otc::NorthEast) {
        m_xPattern = 2;
        m_yPattern = 0;
    }
    else if(direction == Otc::East) {
        m_xPattern = 2;
        m_yPattern = 1;
    }
    else if(direction == Otc::SouthEast) {
        m_xPattern = 2;
        m_yPattern = 2;
    }
    else if(direction == Otc::South) {
        m_xPattern = 1;
        m_yPattern = 2;
    }
    else if(direction == Otc::SouthWest) {
        m_xPattern = 0;
        m_yPattern = 2;
    }
    else if(direction == Otc::West) {
        m_xPattern = 0;
        m_yPattern = 1;
    }
    else {
        m_xPattern = 1;
        m_yPattern = 1;
    }

    m_pos = fromPosition;
    m_posDelta = toPosition - fromPosition;
    m_startTicks = g_clock.ticks();
    m_duration = 150 * std::sqrt(Point(m_posDelta.x, m_posDelta.y).length());
    m_posDelta.x *= Map::NUM_TILE_PIXELS;
    m_posDelta.y *= Map::NUM_TILE_PIXELS;

    // schedule removal
    auto self = asMissile();
    g_dispatcher.scheduleEvent([self]() {
        g_map.removeThing(self);
    }, m_duration);
}

ThingType *Missile::getType()
{
    return g_thingsType.getThingType(m_id, ThingsType::Missile);
}
