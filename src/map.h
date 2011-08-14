#ifndef MAP_H
#define MAP_H

#include "position.h"
#include "tile.h"
#include <graphics/graphicsdeclarations.h>
#include <unordered_map>

class Map
{
public:
    void addThing(Thing *thing, const Position& pos);

    void draw(int x, int y);

private:
    std::unordered_map<Position, TilePtr, PositionHash> m_tiles;

    FrameBufferPtr m_framebuffer;
};

#endif
