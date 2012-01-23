/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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
#include "animatedtext.h"
#include <framework/core/clock.h>
#include <framework/graphics/declarations.h>

class Map
{
public:
    enum {
        MAP_VISIBLE_WIDTH = 15,
        MAP_VISIBLE_HEIGHT = 11,
        MAP_SIZE_Z = 8,
        MAX_WIDTH = 24,
        MAX_HEIGHT = 24,
        MAX_Z = 16,
        NUM_TILE_PIXELS = 32
    };

    Map();

    void draw(const Rect& rect);
    void clean();

    int getFirstVisibleFloor();
    bool isLookPossible(const Position& pos);
    bool isCovered(const Position& pos, int firstFloor = 0);
    bool isCompletlyCovered(const Position& pos, int firstFloor = 0);

    void addThing(const ThingPtr& thing, const Position& pos, int stackPos = -1);
    ThingPtr getThing(const Position& pos, int stackPos);
    void removeThingByPos(const Position& pos, int stackPos);
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

    void setVisibleSize(const Size& visibleSize);
    Size getVibibleSize() { return m_visibleSize; }
    Point getCentralOffset() { return m_centralOffset; }

    Point positionTo2D(const Position& position);

private:
    std::unordered_map<Position, TilePtr, PositionHasher> m_tiles;
    std::map<uint32, CreaturePtr> m_creatures;
    std::array<std::vector<MissilePtr>, MAX_Z> m_missilesAtFloor;
    std::vector<AnimatedTextPtr> m_animatedTexts;
    std::vector<StaticTextPtr> m_staticTexts;

    Light m_light;
    Position m_centralPosition;
    Size m_size;
    Size m_visibleSize;
    Point m_centralOffset;
    Point m_drawOffset;

    FrameBufferPtr m_framebuffer;
    PainterShaderProgramPtr m_shaderProgram;
};

extern Map g_map;

#endif
