#ifndef TILE_H
#define TILE_H

#include <global.h>
#include "thing.h"

class Tile
{
public:
    Tile();

    void addThing(Thing *thing);

    void draw(int x, int y, int z);

private:
    Item *m_ground;
    std::list<Thing*> m_itemsBot;
    std::list<Thing*> m_creatures;
    std::list<Thing*> m_itemsTop;
};

#endif // TILE_H
