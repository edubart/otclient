#ifndef MAP_H
#define MAP_H

#include "position.h"
#include "tile.h"
#include <graphics/framebuffer.h>

class Map
{
public:
    void addThing(Thing *thing, const Position& pos);

    void draw(int x, int y);

private:
    // Visible tiles are 15x11, but we have a +3 value. We have 15 floors.
    Tile m_map[20][16][15];

    FrameBufferPtr m_framebuffer;
};

#endif // MAP_H
