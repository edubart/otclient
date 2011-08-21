#ifndef MAP_H
#define MAP_H

#include "tile.h"
#include "creature.h"
#include <framework/graphics/declarations.h>

class Map
{
public:
    void draw(const Rect& rect);

    void addThing(ThingPtr thing, uint8 stackpos = 0);
    ThingPtr getThing(const Position& pos, uint8 stackpos);
    void removeThing(const Position& pos, uint8 stackpos);
    void removeThingByPtr(ThingPtr thing);

    void clean();
    void cleanTile(const Position& pos);

    void setLight(const Light& light) { m_light = light; }
    Light getLight() { return m_light; }

    CreaturePtr getCreatureById(uint32 id);
    void removeCreatureById(uint32 id);

private:
    std::unordered_map<Position, TilePtr, PositionHasher> m_tiles;
    std::map<uint32, CreaturePtr> m_creatures;

    Light m_light;


    FrameBufferPtr m_framebuffer;
};

extern Map g_map;

#endif
