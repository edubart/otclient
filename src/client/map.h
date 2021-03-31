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

#ifndef MAP_H
#define MAP_H

#include "creature.h"
#include "houses.h"
#include "towns.h"
#include "creatures.h"
#include "animatedtext.h"
#include "statictext.h"
#include "tile.h"

#include <framework/core/clock.h>

enum OTBM_ItemAttr
{
    OTBM_ATTR_DESCRIPTION = 1,
    OTBM_ATTR_EXT_FILE = 2,
    OTBM_ATTR_TILE_FLAGS = 3,
    OTBM_ATTR_ACTION_ID = 4,
    OTBM_ATTR_UNIQUE_ID = 5,
    OTBM_ATTR_TEXT = 6,
    OTBM_ATTR_DESC = 7,
    OTBM_ATTR_TELE_DEST = 8,
    OTBM_ATTR_ITEM = 9,
    OTBM_ATTR_DEPOT_ID = 10,
    OTBM_ATTR_SPAWN_FILE = 11,
    OTBM_ATTR_RUNE_CHARGES = 12,
    OTBM_ATTR_HOUSE_FILE = 13,
    OTBM_ATTR_HOUSEDOORID = 14,
    OTBM_ATTR_COUNT = 15,
    OTBM_ATTR_DURATION = 16,
    OTBM_ATTR_DECAYING_STATE = 17,
    OTBM_ATTR_WRITTENDATE = 18,
    OTBM_ATTR_WRITTENBY = 19,
    OTBM_ATTR_SLEEPERGUID = 20,
    OTBM_ATTR_SLEEPSTART = 21,
    OTBM_ATTR_CHARGES = 22,
    OTBM_ATTR_CONTAINER_ITEMS = 23,
    OTBM_ATTR_ATTRIBUTE_MAP = 128,
    /// just random numbers, they're not actually used by the binary reader...
    OTBM_ATTR_WIDTH = 129,
    OTBM_ATTR_HEIGHT = 130
};

enum OTBM_NodeTypes_t
{
    OTBM_ROOTV2 = 1,
    OTBM_MAP_DATA = 2,
    OTBM_ITEM_DEF = 3,
    OTBM_TILE_AREA = 4,
    OTBM_TILE = 5,
    OTBM_ITEM = 6,
    OTBM_TILE_SQUARE = 7,
    OTBM_TILE_REF = 8,
    OTBM_SPAWNS = 9,
    OTBM_SPAWN_AREA = 10,
    OTBM_MONSTER = 11,
    OTBM_TOWNS = 12,
    OTBM_TOWN = 13,
    OTBM_HOUSETILE = 14,
    OTBM_WAYPOINTS = 15,
    OTBM_WAYPOINT = 16
};

enum {
    OTCM_SIGNATURE = 0x4D43544F,
    OTCM_VERSION = 1
};

enum {
    BLOCK_SIZE = 32
};

enum : uint8 {
    Animation_Force,
    Animation_Show
};

class TileBlock {
public:
    TileBlock() { m_tiles.fill(nullptr); }

    const TilePtr& create(const Position& pos) {
        TilePtr& tile = m_tiles[getTileIndex(pos)];
        tile = TilePtr(new Tile(pos));
        return tile;
    }
    const TilePtr& getOrCreate(const Position& pos) {
        TilePtr& tile = m_tiles[getTileIndex(pos)];
        if(!tile)
            tile = TilePtr(new Tile(pos));
        return tile;
    }
    const TilePtr& get(const Position& pos) { return m_tiles[getTileIndex(pos)]; }
    void remove(const Position& pos) { m_tiles[getTileIndex(pos)] = nullptr; }

    uint getTileIndex(const Position& pos) { return ((pos.y % BLOCK_SIZE) * BLOCK_SIZE) + (pos.x % BLOCK_SIZE); }

    const std::array<TilePtr, BLOCK_SIZE*BLOCK_SIZE>& getTiles() const { return m_tiles; }

private:
    std::array<TilePtr, BLOCK_SIZE*BLOCK_SIZE> m_tiles;
};

struct AwareRange
{
    int top;
    int right;
    int bottom;
    int left;

    int horizontal() { return left + right + 1; }
    int vertical() { return top + bottom + 1; }
};

//@bindsingleton g_map
class Map
{
public:
    void init();
    void terminate();

    void addMapView(const MapViewPtr& mapView);
    void removeMapView(const MapViewPtr& mapView);
    void notificateTileUpdate(const Position& pos);

    bool loadOtcm(const std::string& fileName);
    void saveOtcm(const std::string& fileName);

    void loadOtbm(const std::string& fileName);
    void saveOtbm(const std::string& fileName);

    // otbm attributes (description, size, etc.)
    void setHouseFile(const std::string& file) { m_attribs.set(OTBM_ATTR_HOUSE_FILE, file); }
    void setSpawnFile(const std::string& file) { m_attribs.set(OTBM_ATTR_SPAWN_FILE, file); }
    void setDescription(const std::string& desc) { m_attribs.set(OTBM_ATTR_DESCRIPTION, desc); }

    void clearDescriptions() { m_attribs.remove(OTBM_ATTR_DESCRIPTION); }
    void setWidth(uint16 w) { m_attribs.set(OTBM_ATTR_WIDTH, w); }
    void setHeight(uint16 h) { m_attribs.set(OTBM_ATTR_HEIGHT, h); }

    std::string getHouseFile() { return m_attribs.get<std::string>(OTBM_ATTR_HOUSE_FILE); }
    std::string getSpawnFile() { return m_attribs.get<std::string>(OTBM_ATTR_SPAWN_FILE); }
    Size getSize() { return Size(m_attribs.get<uint16>(OTBM_ATTR_WIDTH), m_attribs.get<uint16>(OTBM_ATTR_HEIGHT)); }
    std::vector<std::string> getDescriptions() { return stdext::split(m_attribs.get<std::string>(OTBM_ATTR_DESCRIPTION), "\n"); }

    void clean();
    void cleanDynamicThings();
    void cleanTexts();

    // thing related
    void addThing(const ThingPtr& thing, const Position& pos, int stackPos = -1);
    ThingPtr getThing(const Position& pos, int stackPos);
    bool removeThing(const ThingPtr& thing);
    bool removeThingByPos(const Position& pos, int stackPos);
    void colorizeThing(const ThingPtr& thing, const Color& color);
    void removeThingColor(const ThingPtr& thing);

    StaticTextPtr getStaticText(const Position& pos);

    // tile related
    const TilePtr& createTile(const Position& pos);
    template <typename... Items>
    const TilePtr& createTileEx(const Position& pos, const Items&... items);
    const TilePtr& getOrCreateTile(const Position& pos);
    const TilePtr& getTile(const Position& pos);
    const TileList getTiles(int floor = -1);
    void cleanTile(const Position& pos);

    // tile zone related
    void setShowZone(tileflags_t zone, bool show);
    void setShowZones(bool show);
    void setZoneColor(tileflags_t zone, const Color& color);
    void setZoneOpacity(float opacity) { m_zoneOpacity = opacity; }

    float getZoneOpacity() { return m_zoneOpacity; }
    Color getZoneColor(tileflags_t flag);
    tileflags_t getZoneFlags() { return (tileflags_t)m_zoneFlags; }
    bool showZones() { return m_zoneFlags != 0; }
    bool showZone(tileflags_t zone) { return (m_zoneFlags & zone) == zone; }

    void setForceShowAnimations(bool force);
    bool isForcingAnimations();
    bool isShowingAnimations();
    void setShowAnimations(bool show);

    void beginGhostMode(float opacity);
    void endGhostMode();

    std::map<Position, ItemPtr> findItemsById(uint16 clientId, uint32 max);

    // known creature related
    void addCreature(const CreaturePtr& creature);
    CreaturePtr getCreatureById(uint32 id);
    void removeCreatureById(uint32 id);
    std::vector<CreaturePtr> getSightSpectators(const Position& centerPos, bool multiFloor);
    std::vector<CreaturePtr> getSpectators(const Position& centerPos, bool multiFloor);
    std::vector<CreaturePtr> getSpectatorsInRange(const Position& centerPos, bool multiFloor, int xRange, int yRange);
    std::vector<CreaturePtr> getSpectatorsInRangeEx(const Position& centerPos, bool multiFloor, int minXRange, int maxXRange, int minYRange, int maxYRange);

    void setLight(const Light& light) { m_light = light; }
    void setCentralPosition(const Position& centralPosition);

    bool isLookPossible(const Position& pos);
    bool isCovered(const Position& pos, int firstFloor = 0);
    bool isCompletelyCovered(const Position& pos, int firstFloor = 0);
    bool isAwareOfPosition(const Position& pos);

    void setAwareRange(const AwareRange& range);
    void resetAwareRange();
    AwareRange getAwareRange() { return m_awareRange; }

    Light getLight() { return m_light; }
    Position getCentralPosition() { return m_centralPosition; }
    int getFirstAwareFloor();
    int getLastAwareFloor();
    const std::vector<MissilePtr>& getFloorMissiles(int z) { return m_floorMissiles[z]; }

    std::vector<AnimatedTextPtr> getAnimatedTexts() { return m_animatedTexts; }
    std::vector<StaticTextPtr> getStaticTexts() { return m_staticTexts; }

    std::tuple<std::vector<Otc::Direction>, Otc::PathFindResult> findPath(const Position& start, const Position& goal, int maxComplexity, int flags = 0);

private:
    void removeUnawareThings();
    uint getBlockIndex(const Position& pos) { return ((pos.y / BLOCK_SIZE) * (65536 / BLOCK_SIZE)) + (pos.x / BLOCK_SIZE); }

    std::unordered_map<uint, TileBlock> m_tileBlocks[Otc::MAX_Z+1];
    std::unordered_map<uint32, CreaturePtr> m_knownCreatures;
    std::array<std::vector<MissilePtr>, Otc::MAX_Z+1> m_floorMissiles;
    std::vector<AnimatedTextPtr> m_animatedTexts;
    std::vector<StaticTextPtr> m_staticTexts;
    std::vector<MapViewPtr> m_mapViews;
    std::unordered_map<Position, std::string, Position::Hasher> m_waypoints;

    uint8 m_animationFlags;
    uint32 m_zoneFlags;
    std::map<uint32, Color> m_zoneColors;
    float m_zoneOpacity;

    Light m_light;
    Position m_centralPosition;
    Rect m_tilesRect;

    stdext::packed_storage<uint8> m_attribs;
    AwareRange m_awareRange;
    static TilePtr m_nulltile;
};

extern Map g_map;

#endif
