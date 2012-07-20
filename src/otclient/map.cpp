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

#include "map.h"
#include "game.h"
#include "localplayer.h"
#include "tile.h"
#include "item.h"
#include "missile.h"
#include "statictext.h"

#include <framework/core/eventdispatcher.h>
#include "mapview.h"
#include <framework/core/resourcemanager.h>
#include <framework/core/filestream.h>
#include <framework/core/binarytree.h>
#include <framework/core/application.h>
#include <framework/xml/tinyxml.h>

Map g_map;

void Map::terminate()
{
    clean();
}

void Map::addMapView(const MapViewPtr& mapView)
{
    m_mapViews.push_back(mapView);
}

void Map::removeMapView(const MapViewPtr& mapView)
{
    auto it = std::find(m_mapViews.begin(), m_mapViews.end(), mapView);
    if(it != m_mapViews.end())
        m_mapViews.erase(it);
}

void Map::notificateTileUpdateToMapViews(const Position& pos)
{
    for(const MapViewPtr& mapView : m_mapViews)
        mapView->onTileUpdate(pos);
}

void Map::loadOtbm(const std::string& fileName)
{
    FileStreamPtr fin = g_resources.openFile(fileName);
    if(!fin)
        stdext::throw_exception(stdext::format("Unable to load map '%s'", fileName));

    fin->cache();
    if(!g_things.isOtbLoaded())
        stdext::throw_exception("OTB isn't loaded yet to load a map.");

    if(fin->getU32())
        stdext::throw_exception("Unknown file version detected");

    BinaryTreePtr root = fin->getBinaryTree();
    if(root->getU8())
        stdext::throw_exception("could not read root property!");

    uint32 headerVersion = root->getU32();
    if(!headerVersion || headerVersion > 3)
        stdext::throw_exception(stdext::format("Unknown OTBM version detected: %u.", headerVersion));

    m_width = root->getU16();
    m_height = root->getU16();
    dump << "Map size: " << m_width << "x" << m_height;

    uint32 headerMajorItems = root->getU8();
    if(headerMajorItems < 3) {
        stdext::throw_exception(stdext::format("This map needs to be upgraded. read %d what it's supposed to be: %u",
                                               headerMajorItems, g_things.getOtbMajorVersion()));
    }

    if(headerMajorItems > g_things.getOtbMajorVersion()) {
        stdext::throw_exception(stdext::format("This map was saved with different OTB version. read %d what it's supposed to be: %d",
                                               headerMajorItems, g_things.getOtbMajorVersion()));
    }

    root->skip(3);
    uint32 headerMinorItems =  root->getU32();
    if(headerMinorItems > g_things.getOtbMinorVersion()) {
        g_logger.warning(stdext::format("This map needs an updated OTB. read %d what it's supposed to be: %d or less",
                                        headerMinorItems, g_things.getOtbMinorVersion()));
    }

    BinaryTreePtr node = root->getChildren()[0];
    if(node->getU8() != OTBM_MAP_DATA)
        stdext::throw_exception("Could not read root data node");

    while (node->canRead()) {
        uint8 attribute = node->getU8();
        std::string tmp = node->getString();
        switch (attribute) {
        case OTBM_ATTR_DESCRIPTION:
            m_description += tmp + "\n";
            break;
        case OTBM_ATTR_SPAWN_FILE:
            m_spawnFile = fileName.substr(0, fileName.rfind('/') + 1) + tmp;
            break;
        case OTBM_ATTR_HOUSE_FILE:
            m_houseFile = fileName.substr(0, fileName.rfind('/') + 1) + tmp;
            break;
        default:
            stdext::throw_exception(stdext::format("Invalid attribute '%c'", attribute));
        }
    }

    for(const BinaryTreePtr &nodeMapData : node->getChildren()) {
        uint8 mapDataType = nodeMapData->getU8();
        if(mapDataType == OTBM_TILE_AREA) {
            Position basePos = nodeMapData->getPosition();

            for(const BinaryTreePtr &nodeTile : nodeMapData->getChildren()) {
                uint8 type = nodeTile->getU8();
                if(type != OTBM_TILE && type != OTBM_HOUSETILE)
                    stdext::throw_exception(stdext::format("invalid node tile type %d", (int)type));

                HousePtr house = nullptr;
                uint32 flags = TILESTATE_NONE;
                Position pos = basePos + nodeTile->getPoint();

                if(type ==  OTBM_HOUSETILE) {
                    uint32 hId = nodeTile->getU32();
                    TilePtr tile = getOrCreateTile(pos);
                    if(!(house = m_houses.getHouse(hId))) {
                        house = HousePtr(new House(hId));
                        m_houses.addHouse(house);
                    }
                    house->setTile(tile);
                }

                while(nodeTile->canRead()) {
                    uint8 tileAttr = nodeTile->getU8();
                    switch (tileAttr) {
                        case OTBM_ATTR_TILE_FLAGS: {
                            uint32 _flags = nodeTile->getU32();
                            if((_flags & TILESTATE_PROTECTIONZONE) == TILESTATE_PROTECTIONZONE)
                                flags |= TILESTATE_PROTECTIONZONE;
                            else if((_flags & TILESTATE_OPTIONALZONE) == TILESTATE_OPTIONALZONE)
                                flags |= TILESTATE_OPTIONALZONE;
                            else if((_flags & TILESTATE_HARDCOREZONE) == TILESTATE_HARDCOREZONE)
                                flags |= TILESTATE_HARDCOREZONE;

                            if((_flags & TILESTATE_NOLOGOUT) == TILESTATE_NOLOGOUT)
                                flags |= TILESTATE_NOLOGOUT;

                            if((_flags & TILESTATE_REFRESH) == TILESTATE_REFRESH)
                                flags |= TILESTATE_REFRESH;

                            break;
                        }
                        case OTBM_ATTR_ITEM: {
                            addThing(Item::createFromOtb(nodeTile->getU16()), pos);
                            break;
                        }
                        default: {
                            stdext::throw_exception(stdext::format("invalid tile attribute %d at pos %s", (int)tileAttr, stdext::to_string(pos)));
                        }
                    }
                }

                for(const BinaryTreePtr &nodeItem : nodeTile->getChildren()) {
                    if(nodeItem->getU8() != OTBM_ITEM)
                        stdext::throw_exception("invalid item node");

                    ItemPtr item = Item::createFromOtb(nodeItem->getU16());
                    item->unserializeItem(nodeItem);

                    if(item->isContainer()) {
                        // This is a temporary way for reading container items.
                        MapContainerPtr mapContainer(new MapContainer);
                        for(const BinaryTreePtr &insideItem : nodeItem->getChildren()) {
                            if(insideItem->getU8() != OTBM_ITEM)
                                stdext::throw_exception("invalid container item node");

                            ItemPtr newItem = Item::createFromOtb(insideItem->getU16());
                            newItem->unserializeItem(insideItem);
                            mapContainer->add(newItem);
                        }
                        m_containers.push_back(mapContainer);
                    }

                    if(house && item->isMoveable()) {
                        g_logger.warning(stdext::format("Movable item found in house: %d at pos %s - escaping...", item->getId(), stdext::to_string(pos)));
                        item.reset();
                    }

                    addThing(item, pos);
                }

                if(const TilePtr& tile = getTile(pos))
                    tile->setFlags((tileflags_t)flags);
            }
        } else if(mapDataType == OTBM_TOWNS) {
            TownPtr town = nullptr;
            for(const BinaryTreePtr &nodeTown : nodeMapData->getChildren()) {
                if(nodeTown->getU8() != OTBM_TOWN)
                    stdext::throw_exception("invalid town node.");

                uint32 townId = nodeTown->getU32();
                std::string townName = nodeTown->getString();
                Position townCoords = nodeTown->getPosition();
                if(!(town = m_towns.getTown(townId))) {
                    town = TownPtr(new Town(townId, townName, townCoords));
                    m_towns.addTown(town);
                }
            }
        } else if(mapDataType == OTBM_WAYPOINTS && headerVersion > 1) {
            for(const BinaryTreePtr &nodeWaypoint : nodeMapData->getChildren()) {
                if(nodeWaypoint->getU8() != OTBM_WAYPOINT)
                    stdext::throw_exception("invalid waypoint node.");

                std::string name = nodeWaypoint->getString();
                Position waypointPos = nodeWaypoint->getPosition();
                if(waypointPos.isValid() && !name.empty() && m_waypoints.find(waypointPos) == m_waypoints.end())
                    m_waypoints.insert(std::make_pair(waypointPos, name));
            }
        } else
            stdext::throw_exception(stdext::format("Unknown map data node %d", (int)mapDataType));
    }

    int numItems = 0;
    for(const auto& it : m_tiles)
        numItems += it.second->getThingCount();

    g_logger.debug(stdext::format("Total items: %d", numItems));
    g_logger.debug(stdext::format("Total tiles: %d", m_tiles.size()));
    g_logger.debug("OTBM read successfully.");
    fin->close();

    loadSpawns(m_spawnFile);
    m_houses.load(m_houseFile);
}

void Map::saveOtbm(const std::string &fileName)
{
    // TODO: Continue sleepy work.
#if 0
    /// TODO: Use binary trees for this
    FileStreamPtr fin = g_resources.openFile(fileName);
    if(!fin)
        stdext::throw_exception(stdext::format("failed to open file '%s'", fileName));

    std::string dir;
    if(fileName.find_last_of('/') <= 0)
        dir = g_resources.getWorkDir();
    else
        dir = fileName.substr(0, fileName.find_last_of('/'));

    if(m_houseFile.empty())
        m_houseFile = "houses.xml";
    if(m_spawnFile.empty())
        m_spawnFile = "spawns.xml";

#if 0
    if(!m_houses->save(dir + "/" + m_houseFile))
        ;
    if(!m_spawns->save(dir + "/" + m_spawnFile))
        ;
#endif

    uint32 ver;
    if(g_things.getOtbMajorVersion() < 2)
        ver =0;
    else if(g_things.getOtbMajorVersion() < 10)
        ver = 1;
    else
        ver = 2;

    fin->addU32(0x00); // file version
    {
        fin->startNode(0x00); //  root
        fin->addU32(ver);
        fin->addU16(m_width); // some random width.
        fin->addU16(m_height); // some random height.

        fin->addU32(g_things.getOtbMajorVersion());
        fin->addU32(g_things.getOtbMinorVersion());

        fin->startNode(OTBM_MAP_DATA); // map data node
        {
            // own description.
            fin->addU8(OTBM_ATTR_DESCRIPTION);
            fin->addString(m_description);

            // special one
            fin->addU8(OTBM_ATTR_DESCRIPTION);
            fin->addString(stdext::format("Saved with %s v%d", g_app.getName(), stdext::unsafe_cast<int>(g_app.getVersion())));

            // spawn file.
            fin->addU8(OTBM_ATTR_SPAWN_FILE);
            fin->addString(m_spawnFile);

            // house file.
            if(ver > 1) {
                fin->addU8(OTBM_ATTR_HOUSE_FILE);
                fin->addString(m_houseFile);
            }

            Position pos(-1, -1, -1);
            Boolean<true> first;
            for (const auto& pair : m_tiles) {
                TilePtr tile = pair.second;
                if(!tile || tile->isEmpty())
                    continue;

                Position tilePos = pair.first;
                if(tilePos.x < pos.x || tilePos.x >= pos.x + 256 ||
                    tilePos.y < pos.y || tilePos.y >= pos.y + 256 ||
                    tilePos.z != pos.z) {
                    if(!first)
                        fin->endNode();

                    pos.x = tilePos.x & 0xFF00;
                    pos.y = tilePos.y & 0xFF00;
                    pos.z = tilePos.z;
                    fin->addU16(pos.x);
                    fin->addU16(pos.y);
                    fin->addU8(pos.z);
                }

                // TODO: hOUSES.
                fin->startNode(OTBM_TILE);
                fin->addU8(tilePos.x);
                fin->addU8(tilePos.y);

#if 0
                // TODO: hOUSES again.
                if(is house tile)
                    add u32 house id...;
#endif
                if(tile->flags()) {
                    fin->addU8(OTBM_ATTR_TILE_FLAGS);
                    fin->addU32(tile->flags());
                }


            }
        }
    }
#endif
}

void Map::loadSpawns(const std::string &fileName)
{
    if(!m_creatures.isLoaded())
        stdext::throw_exception("cannot load spawns; monsters/nps aren't loaded.");

    TiXmlDocument doc;
    doc.Parse(g_resources.loadFile(fileName).c_str());
    if(doc.Error())
        stdext::throw_exception(stdext::format("cannot load spawns xml file '%s: '%s'", fileName, doc.ErrorDesc()));

    TiXmlElement* root = doc.FirstChildElement();
    if(!root || root->ValueStr() != "spawns")
        stdext::throw_exception("malformed spawns file");

    for(TiXmlElement* node = root->FirstChildElement(); node; node = node->NextSiblingElement()) {
        if(node->ValueTStr() != "spawn")
            stdext::throw_exception("invalid spawn node");

        Position centerPos = node->readPos("center");
        for(TiXmlElement* mType = node->FirstChildElement(); mType; mType = mType->NextSiblingElement()) {
            if(mType->ValueStr() != "monster" && mType->ValueStr() != "npc")
                stdext::throw_exception(stdext::format("invalid spawn-subnode %s", mType->ValueStr()));

            std::string mName = mType->Attribute("name");
            CreatureTypePtr m = getCreature(mName);
            if (!m)
                stdext::throw_exception(stdext::format("unkown monster '%s'", stdext::trim(stdext::tolower(mName))));

            m->setPos(centerPos + mType->readPoint());
            m->setSpawnTime(mType->readType<int>("spawntime"));
        }
    }

    doc.Clear();
}

bool Map::loadOtcm(const std::string& fileName)
{
    try {
        FileStreamPtr fin = g_resources.openFile(fileName);
        if(!fin)
            stdext::throw_exception("unable to open file");

        fin->cache();

        uint32 signature = fin->getU32();
        if(signature != OTCM_SIGNATURE)
            stdext::throw_exception("invalid otcm file");

        uint16 start = fin->getU16();
        uint16 version = fin->getU16();
        fin->getU32(); // flags

        switch(version) {
            case 1: {
                fin->getString(); // description
                uint32 datSignature = fin->getU32();
                fin->getU16(); // protocol version
                fin->getString(); // world name

                if(datSignature != g_things.getDatSignature())
                    g_logger.warning("otcm map loaded was created with a different dat signature");

                break;
            }
            default:
                stdext::throw_exception("otcm version not supported");
        }

        fin->seek(start);

        while(true) {
            Position pos;

            pos.x = fin->getU16();
            pos.y = fin->getU16();
            pos.z = fin->getU8();

            // end of file
            if(!pos.isValid())
                break;

            while(true) {
                uint16 id = fin->getU16();

                // end of tile
                if(id == 0xFFFF)
                    break;

                uint8 countOrSubType = fin->getU8();

                ItemPtr item = Item::create(id);
                if(item->isStackable() || item->isFluidContainer() || item->isSplash() || item->isChargeable())
                    item->setCountOrSubType(countOrSubType);

                if(item->isValid())
                    addThing(item, pos);
            }
        }

        fin->close();

        return true;
    } catch(stdext::exception& e) {
        g_logger.error(stdext::format("failed to load OTCM map: %s", e.what()));
        return false;
    }
}

void Map::saveOtcm(const std::string& fileName)
{
    try {
        g_clock.update();

        FileStreamPtr fin = g_resources.createFile(fileName);
        fin->cache();

        //TODO: compression flag with zlib
        uint32 flags = 0;

        // header
        fin->addU32(OTCM_SIGNATURE);
        fin->addU16(0); // data start, will be overwritten later
        fin->addU16(OTCM_VERSION);
        fin->addU32(flags);

        // version 1 header
        fin->addString("OTCM 1.0"); // map description
        fin->addU32(g_things.getDatSignature());
        fin->addU16(g_game.getProtocolVersion());
        fin->addString(g_game.getWorldName());

        // go back and rewrite where the map data starts
        uint32 start = fin->tell();
        fin->seek(4);
        fin->addU16(start);
        fin->seek(start);

        for(auto& pair : m_tiles) {
            const TilePtr& tile = pair.second;
            if(!tile || tile->isEmpty())
                continue;

            Position pos = pair.first;
            fin->addU16(pos.x);
            fin->addU16(pos.y);
            fin->addU8(pos.z);

            const auto& list = tile->getThings();
            auto first = std::find_if(list.begin(), list.end(), [](const ThingPtr& thing) { return thing->isItem(); });
            for(auto it = first, end = list.end(); it != end; ++it) {
                if(ItemPtr item = (*it)->asItem()) {
                    fin->addU16(item->getId());
                    fin->addU8(item->getCountOrSubType());
                }
            }

            // end of tile
            fin->addU16(0xFFFF);
        }

        // end of file
        Position invalidPos;
        fin->addU16(invalidPos.x);
        fin->addU16(invalidPos.y);
        fin->addU8(invalidPos.z);

        fin->flush();
        fin->close();
    } catch(stdext::exception& e) {
        g_logger.error(stdext::format("failed to save OTCM map: %s", e.what()));
    }
}

void Map::clean()
{
    cleanDynamicThings();
    m_tiles.clear();
    m_waypoints.clear();

    m_towns.clear();
    m_houses.clear();
    m_creatures.clear();
    m_tilesRect = Rect(65534, 65534, 0, 0);
}

void Map::cleanDynamicThings()
{
    for(const auto& pair : m_knownCreatures) {
        const CreaturePtr& creature = pair.second;
        removeThing(creature);
        creature->setRemoved(true);
    }
    m_knownCreatures.clear();

    for(int i=0;i<=Otc::MAX_Z;++i)
        m_floorMissiles[i].clear();

    cleanTexts();
}

void Map::cleanTexts()
{
    m_animatedTexts.clear();
    m_staticTexts.clear();
}

void Map::addThing(const ThingPtr& thing, const Position& pos, int stackPos)
{
    if(!thing)
        return;

    TilePtr tile = getOrCreateTile(pos);

    Position oldPos = thing->getPosition();

    if(thing->isItem() || thing->isCreature() || thing->isEffect()) {
        tile->addThing(thing, stackPos);
    } else if(thing->isMissile()) {
        m_floorMissiles[pos.z].push_back(thing->asMissile());
    } else if(thing->isAnimatedText()) {
        m_animatedTexts.push_back(thing->asAnimatedText());
    } else if(thing->isStaticText()) {
        StaticTextPtr staticText = thing->asStaticText();
        bool mustAdd = true;
        for(auto it = m_staticTexts.begin(), end = m_staticTexts.end(); it != end; ++it) {
            StaticTextPtr cStaticText = *it;
            if(cStaticText->getPosition() == pos) {
                // try to combine messages
                if(cStaticText->addMessage(staticText->getName(), staticText->getMessageType(), staticText->getFirstMessage())) {
                    mustAdd = false;
                    break;
                } else {
                    // must add another message and rearrenge current
                }
            }

        }

        if(mustAdd)
            m_staticTexts.push_back(staticText);
    }

    thing->startAnimation();
    thing->setPosition(pos);

    if(thing->isCreature()) {
        CreaturePtr creature = thing->asCreature();
        if(oldPos != pos) {
            if(oldPos.isInRange(pos,1,1))
                g_game.processCreatureMove(creature, oldPos, pos);
            else
                g_game.processCreatureTeleport(creature);
        }
    }

    notificateTileUpdateToMapViews(pos);
}

ThingPtr Map::getThing(const Position& pos, int stackPos)
{
    if(TilePtr tile = getTile(pos))
        return tile->getThing(stackPos);
    return nullptr;
}

bool Map::removeThing(const ThingPtr& thing)
{
    if(!thing)
        return false;

    notificateTileUpdateToMapViews(thing->getPosition());

    if(MissilePtr missile = thing->asMissile()) {
        int z = missile->getPosition().z;
        auto it = std::find(m_floorMissiles[z].begin(), m_floorMissiles[z].end(), missile);
        if(it != m_floorMissiles[z].end()) {
            m_floorMissiles[z].erase(it);
            return true;
        }
    } else if(AnimatedTextPtr animatedText = thing->asAnimatedText()) {
        auto it = std::find(m_animatedTexts.begin(), m_animatedTexts.end(), animatedText);
        if(it != m_animatedTexts.end()) {
            m_animatedTexts.erase(it);
            return true;
        }
    } else if(StaticTextPtr staticText = thing->asStaticText()) {
        auto it = std::find(m_staticTexts.begin(), m_staticTexts.end(), staticText);
        if(it != m_staticTexts.end()) {
            m_staticTexts.erase(it);
            return true;
        }
    } else if(const TilePtr& tile = thing->getTile())
        return tile->removeThing(thing);

    return false;
}

bool Map::removeThingByPos(const Position& pos, int stackPos)
{
    if(TilePtr tile = getTile(pos))
        return removeThing(tile->getThing(stackPos));
    return false;
}

template <typename... Items>
TilePtr Map::createTileEx(const Position& pos, const Items&... items)
{
    TilePtr tile = getOrCreateTile(pos);
    auto vec = {items...};
    for (auto it : vec)
        addThing(it, pos);

    return tile;
}

TilePtr Map::createTile(const Position& pos)
{
    TilePtr tile = TilePtr(new Tile(pos));
    m_tiles[pos] = tile;
    if(pos.x < m_tilesRect.left())
        m_tilesRect.setLeft(pos.x);
    if(pos.y < m_tilesRect.top())
        m_tilesRect.setTop(pos.y);
    if(pos.x > m_tilesRect.right())
        m_tilesRect.setRight(pos.x);
    if(pos.y > m_tilesRect.bottom())
        m_tilesRect.setBottom(pos.y);
    return tile;
}

const TilePtr& Map::getTile(const Position& pos)
{
    static TilePtr nulltile;
    if(!m_tilesRect.contains(Point(pos.x, pos.y)))
        return nulltile;

    auto it = m_tiles.find(pos);
    if(it != m_tiles.end())
        return it->second;
    return nulltile;
}

TilePtr Map::getOrCreateTile(const Position& pos)
{
    const TilePtr& tile = getTile(pos);
    if(tile)
        return tile;
    else
        return createTile(pos);
}

void Map::cleanTile(const Position& pos)
{
    if(TilePtr tile = getTile(pos)) {
        tile->clean();
        if(tile->canErase())
            m_tiles.erase(m_tiles.find(pos));

        notificateTileUpdateToMapViews(pos);
    }
}

void Map::addCreature(const CreaturePtr& creature)
{
    m_knownCreatures[creature->getId()] = creature;
}

CreaturePtr Map::getCreatureById(uint32 id)
{
    auto it = m_knownCreatures.find(id);
    if(it == m_knownCreatures.end())
        return nullptr;
    return it->second;
}

void Map::removeCreatureById(uint32 id)
{
    if(id == 0)
        return;

    auto it = m_knownCreatures.find(id);
    if(it != m_knownCreatures.end())
        it->second->setRemoved(true);

    m_knownCreatures.erase(it);
}

void Map::setCentralPosition(const Position& centralPosition)
{
    m_centralPosition = centralPosition;

    // remove creatures from tiles that we are not aware anymore
    for(const auto& pair : m_knownCreatures) {
        const CreaturePtr& creature = pair.second;
        if(!isAwareOfPosition(creature->getPosition()))
            removeThing(creature);
    }

    // this fixes local player position when the local player is removed from the map,
    // the local player is removed from the map when there are too many creatures on his tile,
    // so there is no enough stackpos to the server send him
    g_dispatcher.addEvent([this] {
        LocalPlayerPtr localPlayer = g_game.getLocalPlayer();
        if(!localPlayer || localPlayer->getPosition() == m_centralPosition)
            return;
        TilePtr tile = localPlayer->getTile();
        if(tile && tile->hasThing(localPlayer))
            return;

        Position oldPos = localPlayer->getPosition();
        Position pos = m_centralPosition;
        localPlayer->setPosition(pos);
        if(oldPos != pos) {
            if(oldPos.isInRange(pos,1,1))
                g_game.processCreatureMove(localPlayer, oldPos, pos);
            else
                g_game.processCreatureTeleport(localPlayer);
        }
    });

    for(const MapViewPtr& mapView : m_mapViews)
        mapView->onMapCenterChange(centralPosition);
}

std::vector<CreaturePtr> Map::getSpectators(const Position& centerPos, bool multiFloor)
{
    return getSpectatorsInRange(centerPos, multiFloor, (Otc::VISIBLE_X_TILES - 1)/2, (Otc::VISIBLE_Y_TILES - 1)/2);
}

std::vector<CreaturePtr> Map::getSpectatorsInRange(const Position& centerPos, bool multiFloor, int xRange, int yRange)
{
    return getSpectatorsInRangeEx(centerPos, multiFloor, xRange, xRange, yRange, yRange);
}

std::vector<CreaturePtr> Map::getSpectatorsInRangeEx(const Position& centerPos, bool multiFloor, int minXRange, int maxXRange, int minYRange, int maxYRange)
{
    int minZRange = 0;
    int maxZRange = 0;
    std::vector<CreaturePtr> creatures;

    if(multiFloor) {
        minZRange = 0;
        maxZRange = Otc::MAX_Z;
    }

    //TODO: optimize
    //TODO: get creatures from other floors corretly
    //TODO: delivery creatures in distance order

    for(int iz=-minZRange; iz<=maxZRange; ++iz) {
        for(int iy=-minYRange; iy<=maxYRange; ++iy) {
            for(int ix=-minXRange; ix<=maxXRange; ++ix) {
                TilePtr tile = getTile(centerPos.translated(ix,iy,iz));
                if(!tile)
                    continue;

                auto tileCreatures = tile->getCreatures();
                creatures.insert(creatures.end(), tileCreatures.rbegin(), tileCreatures.rend());
            }
        }
    }

    return creatures;
}

bool Map::isLookPossible(const Position& pos)
{
    TilePtr tile = getTile(pos);
    return tile && tile->isLookPossible();
}

bool Map::isCovered(const Position& pos, int firstFloor)
{
    // check for tiles on top of the postion
    Position tilePos = pos;
    while(tilePos.coveredUp() && tilePos.z >= firstFloor) {
        TilePtr tile = getTile(tilePos);
        // the below tile is covered when the above tile has a full ground
        if(tile && tile->isFullGround())
            return true;
    }
    return false;
}

bool Map::isCompletelyCovered(const Position& pos, int firstFloor)
{
    Position tilePos = pos;
    while(tilePos.coveredUp() && tilePos.z >= firstFloor) {
        bool covered = true;
        // check in 2x2 range tiles that has no transparent pixels
        for(int x=0;x<2;++x) {
            for(int y=0;y<2;++y) {
                const TilePtr& tile = getTile(tilePos.translated(-x, -y));
                if(!tile || !tile->isFullyOpaque()) {
                    covered = false;
                    break;
                }
            }
        }
        if(covered)
            return true;
    }
    return false;
}

bool Map::isAwareOfPosition(const Position& pos)
{
    if(pos.z < getFirstAwareFloor() || pos.z > getLastAwareFloor())
        return false;

    Position groundedPos = pos;
    while(groundedPos.z != m_centralPosition.z) {
        if(groundedPos.z > m_centralPosition.z)
            groundedPos.coveredUp();
        else
            groundedPos.coveredDown();
    }
    return m_centralPosition.isInRange(groundedPos, Otc::AWARE_X_LEFT_TILES,
                                                    Otc::AWARE_X_RIGHT_TILES,
                                                    Otc::AWARE_Y_TOP_TILES,
                                                    Otc::AWARE_Y_BOTTOM_TILES);
}

int Map::getFirstAwareFloor()
{
    if(m_centralPosition.z > Otc::SEA_FLOOR)
        return m_centralPosition.z-Otc::AWARE_UNDEGROUND_FLOOR_RANGE;
    else
        return 0;
}

int Map::getLastAwareFloor()
{
    if(m_centralPosition.z > Otc::SEA_FLOOR)
        return std::min(m_centralPosition.z+Otc::AWARE_UNDEGROUND_FLOOR_RANGE, (int)Otc::MAX_Z);
    else
        return Otc::SEA_FLOOR;
}

std::tuple<std::vector<Otc::Direction>, Otc::PathFindResult> Map::findPath(const Position& startPos, const Position& goalPos, int maxSteps)
{
    // pathfinding using A* search algorithm
    // as described in http://en.wikipedia.org/wiki/A*_search_algorithm

    struct Node {
        Node(const Position& pos) : cost(0), totalCost(0), steps(0), pos(pos), prev(nullptr), dir(Otc::InvalidDirection), evaluated(false) { }
        bool operator<(const Node& other) const { return  totalCost < other.totalCost; }
        float cost;
        float totalCost;
        int steps;
        Position pos;
        Node *prev;
        Otc::Direction dir;
        bool evaluated;
    };

    struct LessNode : std::binary_function<Node*, Node*, bool> {
        bool operator()(Node* a, Node* b) const {
            return b->totalCost < a->totalCost;
        }
    };

    std::tuple<std::vector<Otc::Direction>, Otc::PathFindResult> ret;
    std::vector<Otc::Direction>& dirs = std::get<0>(ret);
    Otc::PathFindResult& result = std::get<1>(ret);

    result = Otc::PATHFIND_OK;

    if(startPos == goalPos) {
        result = Otc::PATHFIND_SAME_POSITION;
        return ret;
    }

    if(startPos.z != goalPos.z) {
        result = Otc::PATHFIND_IMPOSSIBLE;
        return ret;
    }

    if(startPos.distance(goalPos) > maxSteps) {
        result = Otc::PATHFIND_TOO_FAR;
        return ret;
    }

    std::unordered_map<Position, Node*, PositionHasher> nodes;
    std::priority_queue<Node*, std::vector<Node*>, LessNode> searchList;

    Node *currentNode = new Node(startPos);
    currentNode->pos = startPos;
    nodes[startPos] = currentNode;
    Node *foundNode = nullptr;
    while(currentNode && currentNode->steps < maxSteps) {
        if(currentNode->pos == goalPos && (!foundNode || currentNode->cost < foundNode->cost))
            foundNode = currentNode;

        if(foundNode && currentNode->totalCost >= foundNode->cost)
            break;

        for(int i=-1;i<=1;++i) {
            for(int j=-1;j<=1;++j) {
                if(i == 0 && j == 0)
                    continue;

                Position neighborPos = currentNode->pos.translated(i, j);
                const TilePtr& tile = getTile(neighborPos);
                if(!tile || (!tile->isPathable() && neighborPos != goalPos) || (!tile->isWalkable() && neighborPos == goalPos))
                    continue;

                float walkFactor;
                Otc::Direction walkDir = currentNode->pos.getDirectionFromPosition(neighborPos);
                if(walkDir >= Otc::NorthEast)
                    walkFactor = 3.0f;
                else
                    walkFactor = 1.0f;

                float cost = currentNode->cost + (tile->getGroundSpeed() * walkFactor) / 100.0f;

                Node *neighborNode;
                if(nodes.find(neighborPos) == nodes.end()) {
                    neighborNode = new Node(neighborPos);
                    nodes[neighborPos] = neighborNode;
                } else {
                    neighborNode = nodes[neighborPos];
                    if(neighborNode->cost < cost)
                        continue;
                }

                neighborNode->prev = currentNode;
                neighborNode->cost = cost;
                neighborNode->steps = currentNode->steps + 1;
                neighborNode->totalCost = neighborNode->cost + neighborPos.distance(goalPos);
                neighborNode->dir = walkDir;
                neighborNode->evaluated = false;
                searchList.push(neighborNode);
            }
        }

        currentNode->evaluated = true;
        currentNode = nullptr;
        while(searchList.size() > 0 && !currentNode) {
            Node *node = searchList.top();
            searchList.pop();

            if(!node->evaluated)
                currentNode = node;
        }
    }

    if(foundNode) {
        currentNode = foundNode;
        while(currentNode) {
            dirs.push_back(currentNode->dir);
            currentNode = currentNode->prev;
        }
        dirs.pop_back();
        std::reverse(dirs.begin(), dirs.end());
    } else
        result = Otc::PATHFIND_NO_WAY;

    for(auto it : nodes)
        delete it.second;

    return ret;
}
