#include "tile.h"
#include "item.h"
#include "tibiadat.h"

void Tile::addThing(Thing *thing)
{
    if(!thing)
        return;

    if(thing->getType() == Thing::TYPE_ITEM) {
        const Item *item = thing->getItem();
        if(item) {
            ItemAttributes *itemAttributes = g_tibiaDat.getItemAttributes(item->getId());

            if(itemAttributes->alwaysOnTop)
                m_itemsTop.push_back(thing);
            else
                m_itemsBot.push_back(thing);
        }
    }
    else if(thing->getType() == Thing::TYPE_CREATURE) {

    }
}

void Tile::draw(int x, int y)
{
    for(auto it = m_itemsBot.begin(), end = m_itemsBot.end(); it != end; ++it)
        (*it)->draw(x, y);
    for(auto it = m_creatures.begin(), end = m_creatures.end(); it != end; ++it)
        (*it)->draw(x, y);
    for(auto it = m_itemsTop.begin(), end = m_itemsTop.end(); it != end; ++it)
        (*it)->draw(x, y);
}
