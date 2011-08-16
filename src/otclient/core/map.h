#ifndef MAP_H
#define MAP_H

#include "tile.h"
#include "effect.h"
#include <framework/graphics/declarations.h>

class Map
{
public:
    void draw(int x, int y);

    void addThing(ThingPtr thing, uint8 stackpos = 0);
    void removeThing(ThingPtr thing);

private:
    std::unordered_map<Position, TilePtr, PositionHasher> m_tiles;
    std::list<EffectPtr> m_effects;

    FrameBufferPtr m_framebuffer;
};

extern Map g_map;

#endif
