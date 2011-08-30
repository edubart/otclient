/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef MAP_H
#define MAP_H

#include "creature.h"
#include <framework/graphics/declarations.h>

class Map
{
    enum {
        NUM_X_TILES = 19,
        NUM_Y_TILES = 15,
        NUM_Z_TILES = 15,
        NUM_VISIBLE_X_TILES = 15,
        NUM_VISIBLE_Y_TILES = 11,
        NUM_TILE_PIXELS = 32
    };

public:
    void draw(const Rect& rect);
    void update();

    int getMaxVisibleFloor();
    bool isCovered(const Position& pos, int maxFloor);
    bool isCompletlyCovered(const Position& pos, int maxFloor);

    void addThing(ThingPtr thing, int stackpos = -1);
    ThingPtr getThing(const Position& pos, int stackpos);
    void removeThing(const Position& pos, int stackpos);
    void removeThingByPtr(ThingPtr thing);

    void clean();
    void cleanTile(const Position& pos);

    void setLight(const Light& light) { m_light = light; }
    Light getLight() { return m_light; }

    void setCentralPosition(const Position& centralPosition);
    Position getCentralPosition() { return m_centralPosition; }

    CreaturePtr getCreatureById(uint32 id);
    void removeCreatureById(uint32 id);

private:
    std::unordered_map<Position, TilePtr, PositionHasher> m_tiles;
    std::map<uint32, CreaturePtr> m_creatures;
    std::array<std::vector<TilePtr>, NUM_Z_TILES> m_visibleTiles;
    int m_zstart;

    Light m_light;
    Position m_centralPosition;


    FrameBufferPtr m_framebuffer;
};

extern Map g_map;

#endif
