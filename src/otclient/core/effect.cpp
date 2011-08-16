#include "effect.h"
#include "datmanager.h"

Effect::Effect() : Thing(THING_EFFECT)
{
    m_timer = 0;
    m_animation = 0;
    m_finished = false;
}

void Effect::draw(int x, int y)
{
    internalDraw(x, y, 0, 0, 0, 0, m_animation);
}

void Effect::update(int elapsedTime)
{
    if(m_finished)
        return;

    if(m_timer > 75) {
        const ThingAttributes& attributes = getAttributes();

        if(m_animation+1 == attributes.animcount) {
            m_finished = true;
            return;
        }
        m_animation++;
        m_timer = 0;
    }
    m_timer += elapsedTime;
}

const ThingAttributes& Effect::getAttributes()
{
    return g_dat.getEffectAttributes(m_id);
}
