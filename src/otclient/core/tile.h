#ifndef TILE_H
#define TILE_H

#include "thing.h"

class Tile
{
public:
    Tile();

    void addThing(ThingPtr thing, uint8 stackpos);

    void draw(int x, int y);

    bool hasGround() { return (!!m_ground); }

    int getStackSize();

private:
    ThingPtr m_ground;
    std::deque<ThingPtr> m_itemsBot;
    std::deque<ThingPtr> m_creatures;
    std::deque<ThingPtr> m_itemsTop;
};

#endif
