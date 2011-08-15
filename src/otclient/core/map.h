#ifndef MAP_H
#define MAP_H

#include "tile.h"
#include "effect.h"
#include <framework/graphics/declarations.h>
#include <unordered_map>

struct MapPositionHasher : std::unary_function<Position, std::size_t> {
    std::size_t operator()(const Position& pos) const {
        return ((((pos.x * 65536) + pos.y) * 15) + pos.z) % 1000000;
    }
};

class Map
{
public:
    void addThing(ThingPtr thing, uint8 stackpos = 0);

    void draw(int x, int y);

private:
    std::unordered_map<Position, TilePtr, MapPositionHasher> m_tiles;
    std::list<ThingPtr> m_effects;

    FrameBufferPtr m_framebuffer;
};

#endif
