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

#include "creature.h"
#include "thingstype.h"
#include "localplayer.h"
#include "map.h"
#include "tile.h"
#include "item.h"

#include <framework/platform/platform.h>
#include <framework/graphics/graphics.h>
#include <framework/core/eventdispatcher.h>


Creature::Creature() : Thing()
{
    m_healthPercent = 0;
    m_direction = Otc::South;

    m_walking = false;
    m_walkOffsetX = 0;
    m_walkOffsetY = 0;

    m_informationFont = g_fonts.getFont("verdana-11px-rounded");
}

void Creature::draw(int x, int y)
{
    if(m_walking) {
        x += m_walkOffsetX;
        y += m_walkOffsetY;
    }

    const ThingType& type = getType();

    // Render creature
    for(m_yPattern = 0; m_yPattern < type.yPattern; m_yPattern++) {

        // continue if we dont have this addon.
        if(m_yPattern > 0 && !(m_outfit.addons & (1 << (m_yPattern-1))))
            continue;

        // draw white item
        internalDraw(x, y, 0);

        // draw mask if exists
        if(type.layers > 1) {
            // switch to blend color mode
            g_graphics.bindBlendFunc(Fw::BlendColorzing);

            // head
            g_graphics.bindColor(Otc::OutfitColors[m_outfit.head]);
            internalDraw(x, y, 1, Otc::SpriteYellowMask);

            // body
            g_graphics.bindColor(Otc::OutfitColors[m_outfit.body]);
            internalDraw(x, y, 1, Otc::SpriteRedMask);

            // legs
            g_graphics.bindColor(Otc::OutfitColors[m_outfit.legs]);
            internalDraw(x, y, 1, Otc::SpriteGreenMask);

            // feet
            g_graphics.bindColor(Otc::OutfitColors[m_outfit.feet]);
            internalDraw(x, y, 1, Otc::SpriteBlueMask);

            // restore default blend func
            g_graphics.bindBlendFunc(Fw::BlendDefault);
            g_graphics.bindColor(Fw::white);
        }
    }
}

void Creature::drawInformation(int x, int y, bool useGray, const Rect& rect)
{
    Color fillColor = Color(96, 96, 96);

    if(!useGray)
        fillColor = m_informationColor;

    // calculate main rects
    Rect backgroundRect = Rect(x-(13.5), y, 27, 4);
    backgroundRect.bound(rect);

    Size textSize = m_informationFont->calculateTextRectSize(m_name);
    Rect textRect = Rect(x - textSize.width() / 2.0, y-15, textSize);
    textRect.bound(rect);

    // distance them
    if(textRect.top() == rect.top())
        backgroundRect.moveTop(textRect.top() + 15);
    if(backgroundRect.bottom() == rect.bottom())
        textRect.moveTop(backgroundRect.top() - 15);

    // health rect is based on background rect, so no worries
    Rect healthRect = backgroundRect.expanded(-1);
    healthRect.setWidth((m_healthPercent / 100.0) * 25);

    // draw
    g_graphics.bindColor(Fw::black);
    g_graphics.drawFilledRect(backgroundRect);

    g_graphics.bindColor(fillColor);
    g_graphics.drawFilledRect(healthRect);

    m_informationFont->renderText(m_name, textRect, Fw::AlignTopCenter, fillColor);
}

void Creature::walk(const Position& position, bool inverse)
{
    // set walking state
    bool sameWalk = m_walking && !m_inverseWalking && inverse;
    m_walking = true;
    m_inverseWalking = inverse;
    int walkTimeFactor = 1;

    // set new direction
    if(m_position + Position(0, -1, 0) == position) {
        m_direction = Otc::North;
        m_walkOffsetY = 32;
    }
    else if(m_position + Position(1, 0, 0) == position) {
        m_direction = Otc::East;
        m_walkOffsetX = -32;
    }
    else if(m_position + Position(0, 1, 0) == position) {
        m_direction = Otc::South;
        m_walkOffsetY = -32;
    }
    else if(m_position + Position(-1, 0, 0) == position) {
        m_direction = Otc::West;
        m_walkOffsetX = 32;
    }
    else if(m_position + Position(1, -1, 0) == position) {
        m_direction = Otc::NorthEast;
        m_walkOffsetX = -32;
        m_walkOffsetY = 32;
        walkTimeFactor = 2;
    }
    else if(m_position + Position(1, 1, 0) == position) {
        m_direction = Otc::SouthEast;
        m_walkOffsetX = -32;
        m_walkOffsetY = -32;
        walkTimeFactor = 2;
    }
    else if(m_position + Position(-1, 1, 0) == position) {
        m_direction = Otc::SouthWest;
        m_walkOffsetX = 32;
        m_walkOffsetY = -32;
        walkTimeFactor = 2;
    }
    else if(m_position + Position(-1, -1, 0) == position) {
        m_direction = Otc::NorthWest;
        m_walkOffsetX = 32;
        m_walkOffsetY = 32;
        walkTimeFactor = 2;
    }
    else { // Teleport
        // we teleported, dont walk or change direction
        m_walking = false;
        m_walkOffsetX = 0;
        m_walkOffsetY = 0;
        m_animation = 0;
    }

    if(!m_inverseWalking) {
        m_walkOffsetX = 0;
        m_walkOffsetY = 0;
    }

    if(m_walking) {
        // Calculate xPattern
        if(m_direction >= 4) {
            if(m_direction == Otc::NorthEast || m_direction == Otc::SouthEast)
                m_xPattern = Otc::East;
            else if(m_direction == Otc::NorthWest || m_direction == Otc::SouthWest)
                m_xPattern = Otc::West;
        }
        else {
            m_xPattern = m_direction;
        }

        // get walk speed
        int groundSpeed = 100;

        ItemPtr ground = g_map.getTile(position)->getGround();
        if(ground)
            groundSpeed = ground->getType().groundSpeed;

        float walkTime = walkTimeFactor * 1000.0 * (float)groundSpeed / m_speed;
        walkTime = (walkTime == 0) ? 1000 : walkTime;

        m_walkTimePerPixel = walkTime / 32.0;
        if(!sameWalk)
            m_walkStartTicks = g_platform.getTicks();
        updateWalk();
    }
}

void Creature::updateWalk()
{
    const ThingType& type = getType();
    if(m_walking) {
        int elapsedTicks = g_platform.getTicks() - m_walkStartTicks;
        int totalPixelsWalked = std::min((int)std::round(elapsedTicks / m_walkTimePerPixel), 32);

        if(m_inverseWalking) {
            if(m_direction == Otc::North || m_direction == Otc::NorthEast || m_direction == Otc::NorthWest)
                m_walkOffsetY = 32 - totalPixelsWalked;
            else if(m_direction == Otc::South || m_direction == Otc::SouthEast || m_direction == Otc::SouthWest)
                m_walkOffsetY = totalPixelsWalked - 32;

            if(m_direction == Otc::East || m_direction == Otc::NorthEast || m_direction == Otc::SouthEast)
                m_walkOffsetX = totalPixelsWalked - 32;
            else if(m_direction == Otc::West || m_direction == Otc::NorthWest || m_direction == Otc::SouthWest)
                m_walkOffsetX = 32 - totalPixelsWalked;

            if(m_walkOffsetX == 0 && m_walkOffsetY == 0)
                cancelWalk(m_direction);
        }
        else {
            if(m_direction == Otc::North || m_direction == Otc::NorthEast || m_direction == Otc::NorthWest)
                m_walkOffsetY = -totalPixelsWalked;
            else if(m_direction == Otc::South || m_direction == Otc::SouthEast || m_direction == Otc::SouthWest)
                m_walkOffsetY = totalPixelsWalked;

            if(m_direction == Otc::East || m_direction == Otc::NorthEast || m_direction == Otc::SouthEast)
                m_walkOffsetX = totalPixelsWalked;
            else if(m_direction == Otc::West || m_direction == Otc::NorthWest || m_direction == Otc::SouthWest)
                m_walkOffsetX = -totalPixelsWalked;
        }

        int totalWalkTileTicks = (int)m_walkTimePerPixel*32 * 0.5;
        m_animation = (g_platform.getTicks() % totalWalkTileTicks) / (totalWalkTileTicks / (type.animationPhases - 1)) + 1;
        g_dispatcher.scheduleEvent(std::bind(&Creature::updateWalk, asCreature()), m_walkTimePerPixel);

        if(totalPixelsWalked == 32)
            cancelWalk(m_direction);
    }
}

void Creature::cancelWalk(Otc::Direction direction)
{
    m_walking = false;
    m_walkStartTicks = 0;
    m_walkOffsetX = 0;
    m_walkOffsetY = 0;
    m_direction = direction;

    auto self = asCreature();
    g_dispatcher.scheduleEvent([=]() {
        if(!self->m_walking)
            self->m_animation = 0;
    }, m_walkTimePerPixel * 2);
}

void Creature::setHealthPercent(uint8 healthPercent)
{
    int oldHealthPercent = m_healthPercent;
    m_healthPercent = healthPercent;
    onHealthPercentChange(oldHealthPercent);
}

const ThingType& Creature::getType()
{
    return g_thingsType.getCreatureType(m_outfit.type);
}

void Creature::onHealthPercentChange(int)
{
    m_informationColor = Fw::black;

    if(m_healthPercent > 92) {
        m_informationColor.setGreen(188);
    }
    else if(m_healthPercent > 60) {
        m_informationColor.setRed(80);
        m_informationColor.setGreen(161);
        m_informationColor.setBlue(80);
    }
    else if(m_healthPercent > 30) {
        m_informationColor.setRed(161);
        m_informationColor.setGreen(161);
    }
    else if(m_healthPercent > 8) {
        m_informationColor.setRed(160);
        m_informationColor.setGreen(39);
        m_informationColor.setBlue(39);
    }
    else if(m_healthPercent > 3) {
        m_informationColor.setRed(160);
    }
    else {
        m_informationColor.setRed(79);
    }
}
