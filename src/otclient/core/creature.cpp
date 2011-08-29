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
#include "datmanager.h"
#include "localplayer.h"
#include "map.h"
#include <framework/platform/platform.h>
#include <framework/graphics/graphics.h>


Creature::Creature() : Thing(Otc::Creature)
{
    m_healthPercent = 0;
    m_direction = Otc::South;

    m_walking = false;
    m_walkOffsetX = 0;
    m_walkOffsetY = 0;

    m_informationFont = g_fonts.getFont("tibia-12px-rounded");
}

void Creature::draw(int x, int y)
{
    //dump << (int)m_speed;

    // gspeed = 100
    // pspeed = 234
    // (recorded) 400 - 866 = 466
    // (calc by ot eq) 1000 * 100 / 234 = 427

    // gspeed = 150
    // pspeed = 234
    // (recorded) 934 - 1597 = 663
    // (calc by ot eq) 1000 * 150 / 234 = 641

    // gspeed = 100
    // pspeed = 110
    // (recorded) 900 - 1833 = 933
    // (calc by ot eq) 1000 * 100 / 110 = 909

    // 1000 * groundSpeed / playerSpeed

    // TODO 1: FIX RENDER STEP 2
    // TODO 2: FIX DIAGONAL WALKING, BUGS MAP
    // TODO 3: ADD ANIMATION

    x += m_walkOffsetX;
    y += m_walkOffsetY;

    const ThingAttributes& attributes = getAttributes();

    // Render creature
    for(m_yDiv = 0; m_yDiv < attributes.ydiv; m_yDiv++) {

        // continue if we dont have this addon.
        if(m_yDiv > 0 && !(m_outfit.addons & (1 << (m_yDiv-1))))
            continue;

        // draw white item
        internalDraw(x, y, 0);

        // draw mask if exists
        if(attributes.blendframes > 1) {
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

    // Update animation and position
    if(m_walking && attributes.animcount > 1) {

        if(g_platform.getTicks() - m_lastTicks >= m_walkTimePerPixel) {
            int pixelsWalked = std::floor((g_platform.getTicks() - m_lastTicks) / m_walkTimePerPixel);
            int remainingTime = (g_platform.getTicks() - m_lastTicks) % (int)m_walkTimePerPixel;

            if(m_direction == Otc::North || m_direction == Otc::NorthEast || m_direction == Otc::NorthWest)
                m_walkOffsetY = std::max(m_walkOffsetY - pixelsWalked, 0);
            else if(m_direction == Otc::South || m_direction == Otc::SouthEast || m_direction == Otc::SouthWest)
                m_walkOffsetY = std::min(m_walkOffsetY + pixelsWalked, 0);

            if(m_direction == Otc::East || m_direction == Otc::NorthEast || m_direction == Otc::SouthEast)
                m_walkOffsetX = std::min(m_walkOffsetX + pixelsWalked, 0);
            else if(m_direction == Otc::West || m_direction == Otc::NorthWest || m_direction == Otc::SouthWest)
                m_walkOffsetX = std::max(m_walkOffsetX - pixelsWalked, 0);

            //double walkOffset = std::max(m_walkOffsetX, m_walkOffsetY);

            /*if((32 - fabs(walkOffset)) / 8 % 2 == 0) {
                if(m_animation+1 == attributes.animcount)
                    m_animation = 1;
                else
                    m_animation++;

                //m_lastTicks = g_platform.getTicks();
            }*/

            if(((m_walkOffsetX == 0 && m_walkOffsetY == 0) && m_walkOffsetX != m_walkOffsetY) ||
               ((m_walkOffsetX == 0 || m_walkOffsetY == 0) && m_walkOffsetX == m_walkOffsetY)) {
                m_walking = false;
                m_walkOffsetX = 0;
                m_walkOffsetY = 0;
                m_animation = 0;
            }

            m_lastTicks = g_platform.getTicks() - remainingTime;
        }
    }
}

void Creature::drawInformation(int x, int y, bool useGray)
{
    Color fillColor = Color(96, 96, 96);

    if(!useGray)
        fillColor = m_informationColor;

    Rect backgroundRect = Rect(x-(14.5), y, 27, 4);
    Rect healthRect = backgroundRect.expanded(-1);
    healthRect.setWidth((m_healthPercent/100.0)*25);

    g_graphics.bindColor(Fw::black);
    g_graphics.drawFilledRect(backgroundRect);

    g_graphics.bindColor(fillColor);
    g_graphics.drawFilledRect(healthRect);

    // name
    m_informationFont->renderText(m_name, Rect(x-100, y-15, 200, 15), Fw::AlignTopCenter, fillColor);
}

void Creature::walk(const Position& position)
{
    // set walking state
    m_walking = true;
    m_walkOffsetX = 0;
    m_walkOffsetY = 0;
    m_walkingFromPosition = m_position;
    int walkTimeFactor = 1;

    // update map tiles
    g_map.removeThingByPtr(asThing());
    m_position = position;
    g_map.addThing(asThing());

    // set new direction
    if(m_walkingFromPosition + Position(0, -1, 0) == m_position) {
        m_direction = Otc::North;
        m_walkOffsetY = 32;
    }
    else if(m_walkingFromPosition + Position(1, 0, 0) == m_position) {
        m_direction = Otc::East;
        m_walkOffsetX = -32;
    }
    else if(m_walkingFromPosition + Position(0, 1, 0) == m_position) {
        m_direction = Otc::South;
        m_walkOffsetY = -32;
    }
    else if(m_walkingFromPosition + Position(-1, 0, 0) == m_position) {
        m_direction = Otc::West;
        m_walkOffsetX = 32;
    }
    else if(m_walkingFromPosition + Position(1, -1, 0) == m_position) {
        m_direction = Otc::NorthEast;
        m_walkOffsetX = -32;
        m_walkOffsetY = 32;
        walkTimeFactor = 2;
    }
    else if(m_walkingFromPosition + Position(1, 1, 0) == m_position) {
        m_direction = Otc::SouthEast;
        m_walkOffsetX = -32;
        m_walkOffsetY = -32;
        walkTimeFactor = 2;
    }
    else if(m_walkingFromPosition + Position(-1, 1, 0) == m_position) {
        m_direction = Otc::SouthWest;
        m_walkOffsetX = 32;
        m_walkOffsetY = -32;
        walkTimeFactor = 2;
    }
    else if(m_walkingFromPosition + Position(-1, -1, 0) == m_position) {
        m_direction = Otc::NorthWest;
        m_walkOffsetX = 32;
        m_walkOffsetY = 32;
        walkTimeFactor = 2;
    }
    else { // Teleport
        // we teleported, dont walk or change direction
        m_walking = false;
    }

    // Calculate xDiv
    if(m_direction >= 4) {
        if(m_direction == Otc::NorthEast || m_direction == Otc::SouthEast)
            m_xDiv = Otc::East;
        else if(m_direction == Otc::NorthWest || m_direction == Otc::SouthWest)
            m_xDiv = Otc::West;
    }
    else {
        m_xDiv = m_direction;
    }

    // get walk speed
    int groundSpeed = 0;

    ThingPtr ground = g_map.getThing(m_position, 0);
    if(ground)
        groundSpeed = ground->getAttributes().speed;

    float walkTime = walkTimeFactor * 1000.0 * (float)groundSpeed / m_speed;
    walkTime = walkTime == 0 ? 1000 : walkTime;

    m_walkTimePerPixel = walkTime / 32.0;

    m_lastTicks = g_platform.getTicks();
}

void Creature::setHealthPercent(uint8 healthPercent)
{
    int oldHealthPercent = m_healthPercent;
    m_healthPercent = healthPercent;
    onHealthPercentChange(oldHealthPercent);
}

const ThingAttributes& Creature::getAttributes()
{
    return g_dat.getCreatureAttributes(m_outfit.type);
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
