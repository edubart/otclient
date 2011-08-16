#ifndef MAP_H
#define MAP_H

#include "tile.h"
#include "effect.h"
#include <framework/graphics/declarations.h>

class Map
{
public:
    void addThing(ThingPtr thing, uint8 stackpos = 0);

    void draw(int x, int y);

private:
    std::unordered_map<Position, TilePtr, PositionHasher> m_tiles;
    std::list<ThingPtr> m_effects;

    FrameBufferPtr m_framebuffer;
};

extern Map g_map;

#endif
