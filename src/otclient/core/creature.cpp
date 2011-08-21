 #include "creature.h"
#include "datmanager.h"
#include <framework/graphics/graphics.h>
#include <framework/graphics/framebuffer.h>
#include "game.h"
#include <framework/graphics/fontmanager.h>

Creature::Creature() : Thing(THING_CREATURE)
{
    m_healthPercent = 0;
    m_direction = DIRECTION_SOUTH;
}

void Creature::draw(int x, int y)
{
    //ThingAttributes *creatureAttributes = getAttributes();
    int anim = 0;

    // draw outfit
    internalDraw(x, y, 0, m_direction, 0, 0, anim);

    const ThingAttributes& attributes = getAttributes();
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
            internalDraw(x, y, 1, m_direction, 0, 0, anim, (SpriteMask)mask);
        }

        g_graphics.bindBlendFunc(BLEND_NORMAL);
        g_graphics.bindColor(Color::white);
    }
}

void Creature::drawInformation(int x, int y, bool useGray)
{
    Color fillColor = Color(96, 96, 96);

    if(!useGray) {
        // health bar
        fillColor = Color::black;
        if(m_healthPercent > 60 && m_healthPercent <= 100) {
            fillColor.setRed(0.0f);
            fillColor.setGreen(0.75f);
        } else if(m_healthPercent > 30 && m_healthPercent <= 60) {
            fillColor.setRed(0.75f);
            fillColor.setGreen(0.75f);
        } else if(m_healthPercent > 10 && m_healthPercent <= 30) {
            fillColor.setRed(0.75f);
            fillColor.setGreen(0.00f);
        } else if(m_healthPercent > 0 && m_healthPercent <= 10) {
            fillColor.setRed(0.25f);
            fillColor.setGreen(0.00f);
        }
    }

    Rect backgroundRect = Rect(x-(14.5), y-2, 27, 4);
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
    font->renderText(m_name, Rect(x-100, y-16, 200, 16), AlignTopCenter, fillColor);
}

const ThingAttributes& Creature::getAttributes()
{
    return g_dat.getCreatureAttributes(m_outfit.type);
}
