#ifndef TILE_H
#define TILE_H

#include "thing.h"

class Tile
{
public:
    Tile();

    void addThing(ThingPtr thing, uint8 stackpos);

    void draw(int x, int y);
    bool hasGround();
    int getStackSize();

private:
    ThingPtr m_ground;
    std::list<ThingPtr> m_itemsBot;
    std::list<ThingPtr> m_creatures;
    std::list<ThingPtr> m_itemsTop;
};

#endif
