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
        PLAYER_OFFSET_X = 8,
        PLAYER_OFFSET_Y = 6,
        MAP_VISIBLE_WIDTH = 15,
        MAP_VISIBLE_HEIGHT = 11,
        MAP_SIZE_X = MAP_VISIBLE_WIDTH + 3,
        MAP_SIZE_Y = MAP_VISIBLE_HEIGHT + 3,
        MAP_SIZE_Z = 8,
        MAX_Z = 15,
        NUM_TILE_PIXELS = 32
    };

public:
    void draw(const Rect& rect);
    void clean();

    int getFirstVisibleFloor();
    bool isLookPossible(const Position& pos);
    bool isCovered(const Position& pos, int firstFloor = 0);
    bool isCompletlyCovered(const Position& pos, int firstFloor = 0);

    void addThing(const ThingPtr& thing, const Position& pos, int stackPos = -1);
    ThingPtr getThing(const Position& pos, int stackPos);
    void removeThing(const Position& pos, int stackPos);
    void removeThing(const ThingPtr& thing);
    void cleanTile(const Position& pos);
    TilePtr getTile(const Position& pos);

    void setLight(const Light& light) { m_light = light; }
    Light getLight() { return m_light; }

    void setCentralPosition(const Position& centralPosition);
    Position getCentralPosition() { return m_centralPosition; }

    void addCreature(const CreaturePtr& creature);
    CreaturePtr getCreatureById(uint32 id);
    void removeCreatureById(uint32 id);

private:
    std::unordered_map<Position, TilePtr, PositionHasher> m_tiles;
    std::map<uint32, CreaturePtr> m_creatures;

    Light m_light;
    Position m_centralPosition;


    FrameBufferPtr m_framebuffer;
};

extern Map g_map;

#endif
