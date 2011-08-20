#include "creature.h"
#include "datmanager.h"
#include <framework/graphics/graphics.h>
#include <framework/graphics/framebuffer.h>
#include "game.h"

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

    // health bar
    // TODO: draw outside framebuffer
    Color healthColor = Color::black;
    if(m_healthPercent > 60 && m_healthPercent <= 100) {
        healthColor.setRed(0.0f);
        healthColor.setGreen(0.75f);
    } else if(m_healthPercent > 30 && m_healthPercent <= 60) {
        healthColor.setRed(0.75f);
        healthColor.setGreen(0.75f);
    } else if(m_healthPercent > 10 && m_healthPercent <= 30) {
        healthColor.setRed(0.75f);
        healthColor.setGreen(0.00f);
    } else if(m_healthPercent > 0 && m_healthPercent <= 10) {
        healthColor.setRed(0.25f);
        healthColor.setGreen(0.00f);
    }

    Rect healthRect = Rect(x - 14, y - 11, 27, 4);

    g_graphics.bindColor(Color::black);
    g_graphics.drawBoundingRect(healthRect);

    g_graphics.bindColor(healthColor);
    g_graphics.drawFilledRect(healthRect.expanded(-1));

    // restore white color
    g_graphics.bindColor(Color::white);
}

const ThingAttributes& Creature::getAttributes()
{
    return g_dat.getCreatureAttributes(m_outfit.type);
}
