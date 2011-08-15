#include "tile.h"
#include "item.h"
#include "tibiadat.h"

Tile::Tile()
{
    m_ground = NULL;
}

void Tile::addThing(ThingPtr thing, uint8 stackpos)
{
    if(!thing)
        return;

    ThingAttributes *thingAttributes = thing->getAttributes();
    if(thingAttributes) {
        if(thing->getType() == Thing::TYPE_ITEM) {
            if(thingAttributes->group == THING_GROUP_GROUND)
                m_ground = thing;
            else {
                if(thingAttributes->alwaysOnTop)
                    m_itemsTop.push_back(thing);
                else
                    m_itemsBot.push_back(thing);
            }
        }
        else if(thing->getType() == Thing::TYPE_CREATURE) {
            m_creatures.push_back(thing);
        }
    }

}

void Tile::draw(int x, int y)
{
    if(m_ground)
        m_ground->draw(x, y);

    for(auto it = m_itemsTop.begin(), end = m_itemsTop.end(); it != end; ++it)
        (*it)->draw(x, y);

    for(auto it = m_creatures.begin(), end = m_creatures.end(); it != end; ++it)
        (*it)->draw(x, y);

    for(auto it = m_itemsBot.begin(), end = m_itemsBot.end(); it != end; ++it)
        (*it)->draw(x, y);

}

bool Tile::hasGround()
{
    return m_ground != 0;
}

int Tile::getStackSize()
{
    int ret = 0;
    if(m_ground)
        ret++;

    ret += m_itemsBot.size();
    ret += m_creatures.size();
    ret += m_itemsTop.size();

    return ret;
}
