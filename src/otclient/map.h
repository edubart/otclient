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
#include "houses.h"
#include "towns.h"
#include "animatedtext.h"
#include <framework/core/clock.h>

enum OTBM_AttrTypes_t
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
    OTBM_ATTR_ATTRIBUTE_MAP = 128
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

/// Temporary way for reading container items
struct MapContainer {
private:
    std::vector<ItemPtr> m_items;

public:
    void add(const ItemPtr& item) { m_items.push_back(item); }
    ItemPtr operator[](uint idx) { return getItem(idx); }
    ItemPtr getItem(int index) {
        if(index < 0 || index > (int)m_items.size())
            return nullptr;

        return m_items[index];
    }
};
typedef std::shared_ptr<MapContainer> MapContainerPtr;

//@bindsingleton g_map
class Map
{
public:
    void terminate();

    void addMapView(const MapViewPtr& mapView);
    void removeMapView(const MapViewPtr& mapView);
    void notificateTileUpdateToMapViews(const Position& pos);

    bool loadOtcm(const std::string& fileName);
    void saveOtcm(const std::string& fileName);

    void loadOtbm(const std::string& fileName);
    void saveOtbm(const std::string& fileName);

    void clean();
    void cleanDynamicThings();
    void cleanTexts();

    // thing related
    void addThing(const ThingPtr& thing, const Position& pos, int stackPos = -1);
    ThingPtr getThing(const Position& pos, int stackPos);
    bool removeThingByPos(const Position& pos, int stackPos);

    // tile related
    template <typename... Items>
    TilePtr createTileEx(const Position& pos, const Items&... items);
    TilePtr createTile(const Position& pos);
    const TilePtr& getTile(const Position& pos);
    TilePtr getOrCreateTile(const Position& pos);
    void cleanTile(const Position& pos);
    bool removeThing(const ThingPtr& thing);

    // known creature related
    void addCreature(const CreaturePtr& creature);
    CreaturePtr getCreatureById(uint32 id);
    void removeCreatureById(uint32 id);
    std::vector<CreaturePtr> getSpectators(const Position& centerPos, bool multiFloor);
    std::vector<CreaturePtr> getSpectatorsInRange(const Position& centerPos, bool multiFloor, int xRange, int yRange);
    std::vector<CreaturePtr> getSpectatorsInRangeEx(const Position& centerPos, bool multiFloor, int minXRange, int maxXRange, int minYRange, int maxYRange);

    // town/house related
    TownPtr getTown(uint32 tid) { return m_towns.getTown(tid); }
    HousePtr getHouse(uint32 hid) { return m_houses.getHouse(hid); }

    void setLight(const Light& light) { m_light = light; }
    void setCentralPosition(const Position& centralPosition);

    bool isLookPossible(const Position& pos);
    bool isCovered(const Position& pos, int firstFloor = 0);
    bool isCompletelyCovered(const Position& pos, int firstFloor = 0);
    bool isAwareOfPosition(const Position& pos);

    Light getLight() { return m_light; }
    Position getCentralPosition() { return m_centralPosition; }
    int getFirstAwareFloor();
    int getLastAwareFloor();
    const std::vector<MissilePtr>& getFloorMissiles(int z) { return m_floorMissiles[z]; }

    std::vector<AnimatedTextPtr> getAnimatedTexts() { return m_animatedTexts; }
    std::vector<StaticTextPtr> getStaticTexts() { return m_staticTexts; }

    std::tuple<std::vector<Otc::Direction>, Otc::PathFindResult> findPath(const Position& start, const Position& goal, int maxSteps);

private:
    std::unordered_map<Position, TilePtr, PositionHasher> m_tiles;
    std::map<uint32, CreaturePtr> m_knownCreatures;
    std::array<std::vector<MissilePtr>, Otc::MAX_Z+1> m_floorMissiles;
    std::vector<AnimatedTextPtr> m_animatedTexts;
    std::vector<StaticTextPtr> m_staticTexts;
    std::vector<MapViewPtr> m_mapViews;
    std::unordered_map<Position, std::string, PositionHasher> m_waypoints;
    std::vector<MapContainerPtr> m_containers;

    Light m_light;
    Position m_centralPosition;

    std::string m_description, m_spawnFile, m_houseFile;

    Houses m_houses;
    Towns m_towns;

    uint16 m_width, m_height;
};

extern Map g_map;

#endif
