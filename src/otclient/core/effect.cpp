#include "effect.h"
#include "datmanager.h"
#include "map.h"
#include <framework/platform/platform.h>
#include <framework/core/eventdispatcher.h>

Effect::Effect() : Thing(THING_EFFECT)
{
    m_lastTicks = g_platform.getTicks();
    m_animation = 0;
    m_finished = false;
}

void Effect::draw(int x, int y)
{
    if(!m_finished) {
        if(g_platform.getTicks() - m_lastTicks > 75) {
            const ThingAttributes& attributes = getAttributes();
            if(m_animation+1 == attributes.animcount) {
                //g_dispatcher.addEvent(std::bind(&Map::removeThing, &g_map, asThing()));
                m_finished = true;
            }
            else
                m_animation++;
            m_lastTicks = g_platform.getTicks();
        }

        internalDraw(x, y, 0, 0, 0, 0, m_animation);
    }
}

const ThingAttributes& Effect::getAttributes()
{
    return g_dat.getEffectAttributes(m_id);
}
