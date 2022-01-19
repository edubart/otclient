/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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

#include <framework/luaengine/luaobject.h>
#include "declarations.h"
#include "effect.h"
#include "item.h"
#include "mapview.h"

enum tileflags_t : uint32
{
    TILESTATE_NONE = 0,
    TILESTATE_PROTECTIONZONE = 1 << 0,
    TILESTATE_TRASHED = 1 << 1,
    TILESTATE_OPTIONALZONE = 1 << 2,
    TILESTATE_NOLOGOUT = 1 << 3,
    TILESTATE_HARDCOREZONE = 1 << 4,
    TILESTATE_REFRESH = 1 << 5,

    // internal usage
    TILESTATE_HOUSE = 1 << 6,
    TILESTATE_TELEPORT = 1 << 17,
    TILESTATE_MAGICFIELD = 1 << 18,
    TILESTATE_MAILBOX = 1 << 19,
    TILESTATE_TRASHHOLDER = 1 << 20,
    TILESTATE_BED = 1 << 21,
    TILESTATE_DEPOT = 1 << 22,
    TILESTATE_TRANSLUECENT_LIGHT = 1 << 23,

    TILESTATE_LAST = 1 << 24
};

class Tile : public LuaObject
{
public:
    Tile(const Position& position);

    void onAddVisibleTileList(const MapViewPtr& mapView);
    void drawSurface(const Point& dest, float scaleFactor, LightView* lightView = nullptr);
    void drawGround(const Point& dest, float scaleFactor, LightView* lightView = nullptr);

    void clean();

    void addWalkingCreature(const CreaturePtr& creature);
    void removeWalkingCreature(const CreaturePtr& creature);

    void addThing(const ThingPtr& thing, int stackPos);
    bool removeThing(const ThingPtr thing);
    ThingPtr getThing(int stackPos);
    EffectPtr getEffect(uint16 id);
    bool hasThing(const ThingPtr& thing);
    int getThingStackPos(const ThingPtr& thing);
    ThingPtr getTopThing();

    ThingPtr getTopLookThing();
    ThingPtr getTopUseThing();
    CreaturePtr getTopCreature(bool checkAround = false);
    ThingPtr getTopMoveThing();
    ThingPtr getTopMultiUseThing();

    int getDrawElevation() { return m_drawElevation; }
    const Position& getPosition() { return m_position; }
    const std::vector<CreaturePtr>& getWalkingCreatures() { return m_walkingCreatures; }
    const std::vector<ThingPtr>& getThings() { return m_things; }
    const std::vector<EffectPtr>& getEffects() { return m_effects; }
    std::vector<CreaturePtr> getCreatures();

    std::vector<ItemPtr> getItems();
    ItemPtr getGround() { return m_ground; }
    int getGroundSpeed();
    uint8 getMinimapColorByte();
    int getThingCount() { return m_things.size() + m_effects.size(); }
    bool isPathable();
    bool isWalkable(bool ignoreCreatures = false);
    bool isFullGround();
    bool isFullyOpaque();
    bool isSingleDimension();
    bool isLookPossible();
    bool isClickable();
    bool isEmpty();
    bool isDrawable();
    bool isBorder() { return !m_borderDirections.empty(); };
    bool hasCreature();
    bool hasBlockingCreature();
    bool hasTallThings() { return m_countFlag.hasTallThings; }
    bool hasWideThings() { return m_countFlag.hasWideThings; }
    bool hasTallItems() { return m_countFlag.hasTallItems; }
    bool hasWideItems() { return m_countFlag.hasWideItems; }
    bool hasWall() { return m_countFlag.hasWall; }
    bool hasTranslucentLight() { return m_flags & TILESTATE_TRANSLUECENT_LIGHT; }
    bool mustHookSouth();
    bool mustHookEast();
    bool limitsFloorsView(bool isFreeView = false);
    bool canErase();

    bool hasEffect() { return !m_effects.empty(); }
    bool hasGround() { return (m_ground && m_ground->isSingleGround()) || m_countFlag.hasGroundBorder; };
    bool hasTopGround() { return (m_ground && m_ground->isTopGround()) || m_countFlag.hasTopGroundBorder; }
    bool hasSurface() { return m_countFlag.hasTopItem || !m_effects.empty() || m_countFlag.hasBottomItem || m_countFlag.hasCommonItem || m_countFlag.hasCreature || !m_walkingCreatures.empty() || hasTopGround(); }

    std::vector<Otc::Direction> getBorderDirections() { return m_borderDirections; };

    int getElevation() const;
    bool hasElevation(int elevation = 1);
    void overwriteMinimapColor(uint8 color) { m_minimapColor = color; }

    bool isCompletelyCovered(int8 firstFloor = -1);

    void remFlag(uint32 flag) { m_flags &= ~flag; }
    void setFlag(uint32 flag) { m_flags |= flag; }
    void setFlags(uint32 flags) { m_flags = flags; }
    bool hasFlag(uint32 flag) { return (m_flags & flag) == flag; }
    uint32 getFlags() { return m_flags; }

    void setHouseId(uint32 hid) { m_houseId = hid; }
    uint32 getHouseId() { return m_houseId; }
    bool isHouseTile() { return m_houseId != 0 && (m_flags & TILESTATE_HOUSE) == TILESTATE_HOUSE; }

    void select(bool noFilter = false);
    void unselect();
    bool isSelected() { return m_highlight.enabled; }

    TilePtr asTile() { return static_self_cast<Tile>(); }

    bool hasDisplacement() { return m_countFlag.hasDisplacement > 0; }
    bool hasLight();
    bool isTopGround() const { return m_ground && m_ground->isTopGround(); }
    bool isCovered() { return m_covered; };

    void analyzeThing(const ThingPtr& thing, bool add);

    bool canRender(bool drawViewportEdge, const Position& cameraPosition, AwareRange viewPort, LightView* lightView);

private:
    struct CountFlag {
        int fullGround = 0;
        int notWalkable = 0;
        int notPathable = 0;
        int notSingleDimension = 0;
        int blockProjectile = 0;
        int totalElevation = 0;
        int hasDisplacement = 0;
        int isNotPathable = 0;
        int elevation = 0;
        int opaque = 0;
        int hasLight = 0;
        int hasTallThings = 0;
        int hasWideThings = 0;
        int hasTallItems = 0;
        int hasWideItems = 0;
        int hasWall = 0;
        int hasHookEast = 0;
        int hasHookSouth = 0;
        int hasNoWalkableEdge = 0;
        int hasCreature = 0;
        int hasCommonItem = 0;
        int hasTopItem = 0;
        int hasBottomItem = 0;
        int hasGroundBorder = 0;
        int hasTopGroundBorder = 0;
    };

    void drawTop(const Point& dest, float scaleFactor, LightView* lightView = nullptr);
    void drawBottom(const Point& dest, float scaleFactor, LightView* lightView = nullptr);
    void drawCreature(const Point& dest, float scaleFactor, LightView* lightView = nullptr);
    void drawThing(const ThingPtr& thing, const Point& dest, float scaleFactor, bool animate, LightView* lightView);

    void checkTranslucentLight();
    bool checkForDetachableThing();

    Position m_position;

    uint8 m_drawElevation{ 0 }, m_minimapColor{ 0 };
    uint32 m_flags{ 0 }, m_houseId{ 0 };

    std::array<Position, 8> m_positionsAround;
    std::vector<std::pair<Otc::Direction, Position>> m_positionsBorder;
    std::vector<Otc::Direction> m_borderDirections;

    std::vector<CreaturePtr> m_walkingCreatures;
    std::vector<ThingPtr> m_things;
    std::vector<EffectPtr> m_effects;
    ItemPtr m_ground;

    CountFlag m_countFlag;
    Highlight m_highlight;

    bool m_highlightWithoutFilter{ false },
        m_covered{ false },
        m_completelyCovered{ false },
        m_ignoreCompletelyCoveredCheck{ false };
};

#endif
