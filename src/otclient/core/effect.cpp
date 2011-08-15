#include "effect.h"
#include "datmanager.h"

Effect::Effect() : Thing(THING_EFFECT)
{
}

void Effect::draw(int x, int y)
{
    int anim = 0;
    internalDraw(x, y, 0, 0, 0, 0, anim);
}

const ThingAttributes& Effect::getAttributes()
{
    return g_dat.getEffectAttributes(m_id);
}
