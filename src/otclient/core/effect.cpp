#include "effect.h"
#include "datmanager.h"

Effect::Effect()
{
    m_type = Thing::TYPE_EFFECT;
}

const ThingAttributes& Effect::getAttributes()
{
    return g_dat.getEffectAttributes(m_id);
}

void Effect::draw(int x, int y)
{
    int anim = 0;
    internalDraw(x, y, 0, 0, 0, 0, anim);
}
