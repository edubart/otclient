#ifndef MAP_H
#define MAP_H

#include "position.h"
#include "tile.h"

class Map
{
public:
    void addThing(Thing *thing, const Position& pos);

    void draw(int x, int y, int width, int heigth);

private:
    // Visible tiles are 15x11, but we have a +3 value. We have 15 floors.
    Tile m_map[19][15][15];
};

#endif // MAP_H
