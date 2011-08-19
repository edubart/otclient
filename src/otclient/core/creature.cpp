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

    // TODO: move this shit hahaha
    static const Color outfitColors[] = {
        Color(255,255,255),
        Color(255,212,191),
        Color(255,233,191),
        Color(255,255,191),
        Color(233,255,191),
        Color(212,255,191),
        Color(191,255,191),
        Color(191,255,212),
        Color(191,255,233),
        Color(191,255,255),
        Color(191,233,255),
        Color(191,212,255),
        Color(191,191,255),
        Color(212,191,255),
        Color(233,191,255),
        Color(255,191,255),
        Color(255,191,233),
        Color(255,191,212),
        Color(255,191,191),

        Color(128,128,128),
        Color(191,159,143),
        Color(191,175,143),
        Color(191,191,143),
        Color(175,191,143),
        Color(159,191,143),
        Color(143,191,143),
        Color(143,191,159),
        Color(143,191,175),
        Color(143,191,191),
        Color(143,175,191),
        Color(143,159,191),
        Color(143,143,191),
        Color(159,143,191),
        Color(175,143,191),
        Color(191,143,191),
        Color(191,143,175),
        Color(191,143,159),
        Color(191,143,143),

        Color(182,182,182),
        Color(191,127,95),
        Color(191,159,95),
        Color(191,191,95),
        Color(159,191,95),
        Color(127,191,95),
        Color(95,191,95),
        Color(95,191,127),
        Color(95,191,159),
        Color(95,191,191),
        Color(95,159,191),
        Color(95,127,191),
        Color(95,95,191),
        Color(127,95,191),
        Color(159,95,191),
        Color(191,95,191),
        Color(191,95,159),
        Color(191,95,127),
        Color(191,95,95),

        Color(145,145,145),
        Color(191,106,63),
        Color(191,148,63),
        Color(191,191,63),
        Color(148,191,63),
        Color(107,191,63),
        Color(63,191,63),
        Color(63,191,106),
        Color(63,191,148),
        Color(63,191,191),
        Color(63,148,191),
        Color(63,106,191),
        Color(63,63,191),
        Color(106,63,191),
        Color(148,63,191),
        Color(191,63,191),
        Color(191,63,148),
        Color(191,63,106),
        Color(191,63,63),

        Color(109,109,109),
        Color(255,85,0),
        Color(255,170,0),
        Color(255,255,0),
        Color(170,255,0),
        Color(84,255,0),
        Color(0,255,0),
        Color(0,255,84),
        Color(0,255,170),
        Color(0,255,255),
        Color(0,169,255),
        Color(0,85,255),
        Color(0,0,255),
        Color(85,0,255),
        Color(169,0,255),
        Color(254,0,255),
        Color(255,0,170),
        Color(255,0,85),
        Color(255,0,0),

        Color(72,72,72),
        Color(191,63,0),
        Color(191,127,0),
        Color(191,191,0),
        Color(127,191,0),
        Color(63,191,0),
        Color(0,191,0),
        Color(0,191,63),
        Color(0,191,127),
        Color(0,191,191),
        Color(0,127,191),
        Color(0,63,191),
        Color(0,0,191),
        Color(63,0,191),
        Color(127,0,191),
        Color(191,0,191),
        Color(191,0,127),
        Color(191,0,63),
        Color(191,0,0),

        Color(36,36,36),
        Color(127,42,0),
        Color(127,85,0),
        Color(127,127,0),
        Color(85,127,0),
        Color(42,127,0),
        Color(0,127,0),
        Color(0,127,42),
        Color(0,127,85),
        Color(0,127,127),
        Color(0,84,127),
        Color(0,42,127),
        Color(0,0,127),
        Color(42,0,127),
        Color(84,0,127),
        Color(127,0,127),
        Color(191,0,85),
        Color(127,0,42),
        Color(127,0,0)
    };

    const ThingAttributes& attributes = getAttributes();
    if(attributes.blendframes > 1) {
        g_graphics.bindBlendFunc(BLEND_COLORIZING);

        for(int mask = 0; mask < 4; ++mask) {

            int outfitColorId = 0;
            if(mask == SpriteMaskYellow)
                outfitColorId = m_outfit.head;
            else if(mask == SpriteMaskRed)
                outfitColorId = m_outfit.body;
            else if(mask == SpriteMaskBlue)
                outfitColorId = m_outfit.legs;
            else if(mask ==  SpriteMaskGreen)
                outfitColorId = m_outfit.feet;

            g_graphics.bindColor(outfitColors[outfitColorId]);
            internalDraw(x, y, 1, m_direction, 0, 0, anim, (SpriteMask)mask);
        }

        g_graphics.bindBlendFunc(BLEND_NORMAL);
        g_graphics.bindColor(Color::white);
    }
}

const ThingAttributes& Creature::getAttributes()
{
    return g_dat.getCreatureAttributes(m_outfit.type);
}
