#ifndef MAP_H
#define MAP_H

#include "position.h"
#include "tile.h"
#include "effect.h"
#include <graphics/graphicsdeclarations.h>
#include <unordered_map>

class Map
{
public:
    void addThing(ThingPtr thing, uint8 stackpos = 0);

    void draw(int x, int y);

    FrameBufferPtr getFramebuffer() { return m_framebuffer; }

private:
    std::unordered_map<Position, TilePtr, PositionHash> m_tiles;
    std::list<ThingPtr> m_effects;

    FrameBufferPtr m_framebuffer;
};

#endif
