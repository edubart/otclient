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
#include "thingtypemanager.h"
#include "map.h"
#include "tile.h"
#include "mapview.h"

#include <framework/core/clock.h>
#include <framework/core/eventdispatcher.h>

#include <framework/graphics/paintershaderprogram.h>
#include <framework/graphics/painterogl2_shadersources.h>
#include <framework/graphics/texturemanager.h>
#include <framework/graphics/framebuffermanager.h>
#include <framework/graphics/framebuffer.h>
#include <framework/graphics/graphics.h>
#include "spritemanager.h"

void Missile::draw(const Point& dest, float scaleFactor, bool animate)
{
    if(m_id == 0 || !animate)
        return;

    int xPattern = 0, yPattern = 0;
    if(m_direction == Otc::NorthWest) {
        xPattern = 0;
        yPattern = 0;
    } else if(m_direction == Otc::North) {
        xPattern = 1;
        yPattern = 0;
    } else if(m_direction == Otc::NorthEast) {
        xPattern = 2;
        yPattern = 0;
    } else if(m_direction == Otc::East) {
        xPattern = 2;
        yPattern = 1;
    } else if(m_direction == Otc::SouthEast) {
        xPattern = 2;
        yPattern = 2;
    } else if(m_direction == Otc::South) {
        xPattern = 1;
        yPattern = 2;
    } else if(m_direction == Otc::SouthWest) {
        xPattern = 0;
        yPattern = 2;
    } else if(m_direction == Otc::West) {
        xPattern = 0;
        yPattern = 1;
    } else {
        xPattern = 1;
        yPattern = 1;
    }

    float fraction = m_animationTimer.ticksElapsed() / m_duration;
    m_datType->draw(dest + m_delta * fraction * scaleFactor, scaleFactor, 0, xPattern, yPattern, 0, 0);
}
void Missile::drawLight(const Point& dest, float scaleFactor, bool animate, MapView* mapview)
{

    uint32_t lightSize = 25*getLightLevel();
    float fraction = m_animationTimer.ticksElapsed() / m_duration;

    glBlendFunc(GL_ONE, GL_ONE);
    mapview->m_framebuffer->release();
    mapview->m_lightbuffer->bind();

    Color lightColor = getLightColor();

        g_painter->setColor(lightColor);
        g_painter->drawTexturedRect(Rect(dest - Point(lightSize/2,lightSize/2) + m_delta * fraction * scaleFactor, Size(lightSize,lightSize)), mapview->m_lightTexture);
    mapview->m_lightbuffer->release();
    mapview->m_framebuffer->bind();
    g_painter->refreshState();
}

void Missile::setPath(const Position& fromPosition, const Position& toPosition)
{
    m_direction = fromPosition.getDirectionFromPosition(toPosition);

    m_position = fromPosition;
    m_delta = Point(toPosition.x - fromPosition.x, toPosition.y - fromPosition.y);
    m_duration = 150 * std::sqrt(m_delta.length());
    m_delta *= Otc::TILE_PIXELS;
    m_animationTimer.restart();

    // schedule removal
    auto self = asMissile();
    g_dispatcher.scheduleEvent([self]() { g_map.removeThing(self); }, m_duration);
}

void Missile::setId(uint32 id)
{
    m_id = id;
    m_datType = g_things.getDatType(m_id, DatMissileCategory);
}
