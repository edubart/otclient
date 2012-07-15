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

#ifndef TILE_H
#define TILE_H

#include "declarations.h"
#include <framework/luascript/luaobject.h>

enum tileflags_t
{
    TILESTATE_NONE = 0,
    TILESTATE_PROTECTIONZONE = 1 << 0,
    TILESTATE_TRASHED = 1 << 1,
    TILESTATE_OPTIONALZONE = 1 << 2,
    TILESTATE_NOLOGOUT = 1 << 3,
    TILESTATE_HPPARDCOREZONE = 1 << 4,
    TILESTATE_REFRESH = 1 << 5,

    // internal usage
    TILESTATE_HOUSE = 1 << 6,
    TILESTATE_TELEPORT = 1 << 17,
    TILESTATE_MAGICFIELD = 1 << 18,
    TILESTATE_MAILBOX = 1 << 19,
    TILESTATE_TRASHHOLDER = 1 << 20,
    TILESTATE_BED = 1 << 21,
    TILESTATE_DEPOT = 1 << 22
};

class Tile : public LuaObject
{
public:
    enum {
        MAX_THINGS = 10
    };

    Tile(const Position& position);

    void draw(const Point& dest, float scaleFactor, int drawFlags, MapView* mapview);

public:
    void clean();

    void addWalkingCreature(const CreaturePtr& creature);
    void removeWalkingCreature(const CreaturePtr& creature);

    ThingPtr addThing(const ThingPtr& thing, int stackPos = -1);
    bool removeThing(ThingPtr thing);
    ThingPtr getThing(int stackPos);
    bool hasThing(const ThingPtr& thing);
    int getThingStackpos(const ThingPtr& thing);
    ThingPtr getTopThing();

    ThingPtr getTopLookThing();
    ThingPtr getTopUseThing();
    CreaturePtr getTopCreature();
    ThingPtr getTopMoveThing();
    ThingPtr getTopMultiUseThing();

    const Position& getPosition() { return m_position; }
    int getDrawElevation() { return m_drawElevation; }
    std::vector<ItemPtr> getItems();
    std::vector<CreaturePtr> getCreatures();
    const std::vector<ThingPtr>& getThings() { return m_things; }
    ItemPtr getGround();
    int getGroundSpeed();
    uint8 getMinimapColorByte() { return m_minimapColorByte; }
    int getThingCount() { return m_things.size() + m_effects.size(); }
    bool isPathable();
    bool isWalkable();
    bool isFullGround();
    bool isFullyOpaque();
    bool isLookPossible();
    bool isClickable();
    bool isEmpty();
    bool mustHookSouth();
    bool mustHookEast();
    bool hasCreature();
    bool limitsFloorsView();
    bool canErase();

    TilePtr asTile() { return std::static_pointer_cast<Tile>(shared_from_this()); }
    void setFlags(tileflags_t flags) { m_flags |= (uint32)flags; }

private:
    void update();

    std::vector<CreaturePtr> m_walkingCreatures;
    std::vector<EffectPtr> m_effects; // leave this outside m_things because it has no stackpos.
    std::vector<ThingPtr> m_things;
    Position m_position;
    uint8 m_drawElevation;
    uint32 m_flags;
    uint8 m_minimapColorByte;
};

#endif
