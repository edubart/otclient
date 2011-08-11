#ifndef TILE_H
#define TILE_H

#include <global.h>
#include "thing.h"

class Tile
{
public:
    void addThing(Thing *thing);

    void draw(int x, int y);

private:
    // ground
    std::list<Thing*> m_itemsBot;
    std::list<Thing*> m_creatures;
    std::list<Thing*> m_itemsTop;
};

#endif // TILE_H
