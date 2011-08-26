#include "creature.h"
#include "datmanager.h"
#include "localplayer.h"
#include "map.h"
#include <framework/platform/platform.h>
#include <framework/graphics/graphics.h>
#include <framework/graphics/fontmanager.h>

Creature::Creature() : Thing(THING_CREATURE)
{
    m_healthPercent = 0;
    m_direction = DIRECTION_SOUTH;
    m_animation = 0;

    m_walking = false;
    m_walkOffsetX = 0;
    m_walkOffsetY = 0;
    m_lastTicks = g_platform.getTicks();
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
    // TODO 2: FIX SHAKY EFFECT
    // TODO 3: ADD ANIMATION
    // TODO 4: ADD DIAGONAL WALKING

    const ThingAttributes& attributes = getAttributes();

    // we must walk 32 pixels in m_speed miliseconds
    if(m_walking && attributes.animcount > 1) {
        double offset = (32.0 / m_walkTime) * (g_platform.getTicks() - m_lastTicks);

        if(m_direction == DIRECTION_NORTH)
            m_walkOffsetY = std::max(m_walkOffsetY - offset, 0.0);
        else if(m_direction == DIRECTION_EAST)
            m_walkOffsetX = std::min(m_walkOffsetX + offset, 0.0);
        else if(m_direction == DIRECTION_SOUTH)
            m_walkOffsetY = std::min(m_walkOffsetY + offset, 0.0);
        else if(m_direction == DIRECTION_WEST)
            m_walkOffsetX = std::max(m_walkOffsetX - offset, 0.0);

        /*if(g_platform.getTicks() - m_lastTicks > m_speed / 4) {
            if(m_animation+1 == attributes.animcount)
                m_animation = 1;
            else
                m_animation++;

            m_lastTicks = g_platform.getTicks();
        }*/

        if(((m_walkOffsetX == 0 && m_walkOffsetY == 0) && m_walkOffsetX != m_walkOffsetY) ||
           ((m_walkOffsetX == 0 || m_walkOffsetY == 0) && m_walkOffsetX == m_walkOffsetY)) {
            m_walking = false;
            m_walkOffsetX = 0;
            m_walkOffsetY = 0;
        }

        //m_lastTicks = g_platform.getTicks();
    }

    m_lastTicks = g_platform.getTicks();

    x += m_walkOffsetX;
    y += m_walkOffsetY;


    for(int ydiv = 0; ydiv < attributes.ydiv; ydiv++) {

        // continue if we dont have this addon.
        if(ydiv > 0 && !(m_outfit.addons & (1 << (ydiv-1))))
            continue;

        // draw white item
        internalDraw(x, y, 0, m_direction, ydiv, 0, m_animation);

        // draw mask if exists
        if(attributes.blendframes > 1) {
            g_graphics.bindBlendFunc(BLEND_COLORIZING);

            for(int mask = 0; mask < 4; ++mask) {

                int outfitColorId = 0;
                if(mask == SpriteMaskYellow)
                    outfitColorId = m_outfit.head;
                else if(mask == SpriteMaskRed)
                    outfitColorId = m_outfit.body;
                else if(mask == SpriteMaskGreen)
                    outfitColorId = m_outfit.legs;
                else if(mask ==  SpriteMaskBlue)
                    outfitColorId = m_outfit.feet;

                g_graphics.bindColor(OutfitColors[outfitColorId]);
                internalDraw(x, y, 1, m_direction, ydiv, 0, m_animation, (SpriteMask)mask);
            }

            g_graphics.bindBlendFunc(BLEND_NORMAL);
            g_graphics.bindColor(Color::white);
        }
    }
}

void Creature::drawInformation(int x, int y, bool useGray)
{
    Color fillColor = Color(96, 96, 96);

    if(!useGray) {
        // health bar according to yatc
        fillColor = Color::black;
        if(m_healthPercent > 92) {
            fillColor.setGreen(188);
        }
        else if(m_healthPercent > 60) {
            fillColor.setRed(80);
            fillColor.setGreen(161);
            fillColor.setBlue(80);
        }
        else if(m_healthPercent > 30) {
            fillColor.setRed(161);
            fillColor.setGreen(161);
        }
        else if(m_healthPercent > 8) {
            fillColor.setRed(160);
            fillColor.setGreen(39);
            fillColor.setBlue(39);
        }
        else if(m_healthPercent > 3) {
            fillColor.setRed(160);
        }
        else {
            fillColor.setRed(79);
        }
    }

    Rect backgroundRect = Rect(x-(14.5), y, 27, 4);
    Rect healthRect = backgroundRect.expanded(-1);
    healthRect.setWidth((m_healthPercent/100.0)*25);

    g_graphics.bindColor(Color::black);
    g_graphics.drawFilledRect(backgroundRect);

    g_graphics.bindColor(fillColor);
    g_graphics.drawFilledRect(healthRect);

    // restore white color
    g_graphics.bindColor(Color::white);

    // name
    FontPtr font = g_fonts.getFont("tibia-12px-rounded");
    font->renderText(m_name, Rect(x-100, y-15, 200, 15), AlignTopCenter, fillColor);
}

void Creature::walk(const Position& position)
{
    // set walking state
    m_walking = true;
    m_walkOffsetX = 0;
    m_walkOffsetY = 0;
    m_walkingFromPosition = m_position;

    // update map tiles
    g_map.removeThingByPtr(asThing());
    m_position = position;
    g_map.addThing(asThing());

    // set new direction
    if(m_walkingFromPosition + Position(0, -1, 0) == m_position) {
        m_direction = DIRECTION_NORTH;
        m_walkOffsetY = 32;
    }
    else if(m_walkingFromPosition + Position(1, 0, 0) == m_position) {
        m_direction = DIRECTION_EAST;
        m_walkOffsetX = -32;
    }
    else if(m_walkingFromPosition + Position(0, 1, 0) == m_position) {
        m_direction = DIRECTION_SOUTH;
        m_walkOffsetY = -32;
    }
    else if(m_walkingFromPosition + Position(-1, 0, 0) == m_position) {
        m_direction = DIRECTION_WEST;
        m_walkOffsetX = 32;
    }
    else { // Teleport
        // we teleported, dont walk or change direction
        m_walking = false;
    }

    // get walk speed
    int groundSpeed = 0;

    ThingPtr ground = g_map.getThing(m_position, 0);
    if(ground)
        groundSpeed = ground->getAttributes().speed;

    m_walkTime = 1000.0 * (float)groundSpeed / m_speed;
    m_walkTime = m_walkTime == 0 ? 1000 : m_walkTime;
}

const ThingAttributes& Creature::getAttributes()
{
    return g_dat.getCreatureAttributes(m_outfit.type);
}
