#include "effect.h"
#include "tibiadat.h"

Effect::Effect()
{
    m_type = Thing::TYPE_EFFECT;
}

ThingAttributes *Effect::getAttributes()
{
    return g_tibiaDat.getEffectAttributes(m_id);
}

void Effect::draw(int x, int y)
{
    int anim = 0;
    internalDraw(x, y, 0, 0, 0, 0, anim);
}
