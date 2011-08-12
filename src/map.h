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
    // Visible tiles are 15x11, but we have a +7 value cause itens visible at other floors. We have 15 floors.
    Tile m_map[25][21][15];

    FrameBufferPtr m_framebuffer;
};

#endif // MAP_H
