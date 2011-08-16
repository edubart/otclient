#include "tile.h"
#include "item.h"
#include "datmanager.h"

Tile::Tile()
{
}

void Tile::addThing(ThingPtr thing, uint8 stackpos)
{
    if(!thing)
        return;

    const ThingAttributes& thingAttributes = thing->getAttributes();
    if(thing->asItem()) {
        if(thingAttributes.group == THING_GROUP_GROUND)
            m_ground = thing;
        else {
            if(thingAttributes.alwaysOnTop)
                m_itemsTop.push_back(thing);
            else
                m_itemsBottom.push_back(thing);
        }
    }
    else if(thing->asCreature()) {
        m_creatures.push_back(thing);
    }
    else if(thing->asEffect()) {
        m_effects.push_back(thing);
    }
}

void Tile::removeThing(ThingPtr thing, uint8 stackpos)
{
    if(thing->asEffect()) {
        auto it = std::find(m_effects.begin(), m_effects.end(), thing);
        assert(it != m_effects.end());
        m_effects.erase(it);
    }
}

void Tile::draw(int x, int y)
{
    if(m_ground)
        m_ground->draw(x, y);

    for(const ThingPtr& thing : m_itemsTop)
        thing->draw(x, y);

    for(const ThingPtr& thing : m_creatures)
        thing->draw(x, y);

    for(const ThingPtr& thing : m_itemsBottom)
        thing->draw(x, y);

    for(const ThingPtr& thing : m_effects)
        thing->draw(x, y);
}

int Tile::getStackSize()
{
    int ret = 0;
    if(m_ground)
        ret++;
    ret += m_itemsBottom.size();
    ret += m_creatures.size();
    ret += m_itemsTop.size();
    return ret;
}
