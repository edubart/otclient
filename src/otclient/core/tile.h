#ifndef TILE_H
#define TILE_H

#include "declarations.h"
#include <framework/luascript/luaobject.h>

class Tile : public LuaObject
{
public:
    Tile();

    void draw(int x, int y, int step);

    void addThing(ThingPtr thing, int stackpos);
    ThingPtr getThing(unsigned int stackpos);
    void removeThing(unsigned int stackpos);
    void removeThingByPtr(ThingPtr thing);

    void clean();

    bool hasGround() { return (!!m_ground); }

    int getStackSize(int stop);

    std::vector<ThingPtr> getCreatures() { return m_creatures; }
    int getDrawNextOffset() { return m_drawNextOffset; }

private:
    ThingPtr m_ground;
    std::vector<ThingPtr> m_itemsBottom;
    std::vector<ThingPtr> m_creatures;
    std::vector<ThingPtr> m_itemsTop;
    std::vector<ThingPtr> m_effects;

    int m_drawNextOffset;
};

#endif
