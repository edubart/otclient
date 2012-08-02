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
#include "tile.h"

#include <framework/core/eventdispatcher.h>
#include <framework/core/resourcemanager.h>
#include <framework/core/filestream.h>
#include <framework/core/binarytree.h>
#include <framework/xml/tinyxml.h>

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

    setWidth(root->getU16());
    setHeight(root->getU16());

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
            setDescription(tmp);
            break;
        case OTBM_ATTR_SPAWN_FILE:
            setSpawnFile(fileName.substr(0, fileName.rfind('/') + 1) + tmp);
            break;
        case OTBM_ATTR_HOUSE_FILE:
            setHouseFile(fileName.substr(0, fileName.rfind('/') + 1) + tmp);
            break;
        default:
            stdext::throw_exception(stdext::format("Invalid attribute '%d'", (int)attribute));
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
                        for(const BinaryTreePtr& containerItem : nodeItem->getChildren()) {
                            if(containerItem->getU8() != OTBM_ITEM)
                                stdext::throw_exception("invalid container item node");

                            ItemPtr cItem = Item::createFromOtb(containerItem->getU16());
                            cItem->unserializeItem(containerItem);
                            //item->addContainerItem(cItem);
                        }
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

    g_logger.debug("OTBM read successfully.");
    fin->close();

    /*
    loadSpawns(getSpawnFile());
    m_houses.load(getHouseFile());
    */
}

void Map::saveOtbm(const std::string &fileName)
{
#if 0
    /// FIXME: Untested code
    FileStreamPtr fin = g_resources.appendFile(fileName);
    if(!fin)
        stdext::throw_exception(stdext::format("failed to open file '%s' for write", fileName));

    std::string dir;
    if(fileName.find_last_of('/') == std::string::npos)
        dir = g_resources.getWorkDir();
    else
        dir = fileName.substr(0, fileName.find_last_of('/'));

    uint32 version = 0;
    /// Support old versions (< 810 or 860 IIRC)
    /// TODO: Use constants?
    if(g_things.getOtbMajorVersion() < 10)
        version = 1;
    else
        version = 2;

    /// Usually when a map has empty house/spawn file it means the map is new.
    /// TODO: Ask the user for a map name instead of those ugly uses of substr
    std::string houseFile = getHouseFile(), spawnFile = getSpawnFile();
    if(houseFile.empty() && version > 1)
        houseFile = fileName.substr(fileName.find_last_of('/')) + "-houses.xml";
    if(spawnFile.empty())
        spawnFile = fileName.substr(fileName.find_last_of('/')) + "-spawns.xml";

#if 0
    if(version > 1)
        m_houses->save(dir + "/" + houseFile);

    saveSpawns(dir + "/" + spawnFile);
#endif

    time_t start = time(0);
    fin->addU32(0); // file version
    BinaryTreePtr root = fin->makeTree();
    {
        root->writeU32(version);

        Size mapSize = getSize();
        root->writeU16(mapSize.width());
        root->writeU16(mapSize.height());

        root->writeU32(g_things.getOtbMajorVersion());
        root->writeU32(g_things.getOtbMinorVersion());

        BinaryTreePtr mapData = root->makeChild(OTBM_MAP_DATA);
        {
            // own description.
            for(const auto& desc : getDescriptions()) {
                mapData->writeU8(OTBM_ATTR_DESCRIPTION);
                mapData->writeString(desc);
            }

            // special one
            mapData->writeU8(OTBM_ATTR_DESCRIPTION);
            mapData->writeString(stdext::format("Saved with %s v%d", g_app.getName(), stdext::unsafe_cast<int>(g_app.getVersion())));

            // spawn file.
            mapData->writeU8(OTBM_ATTR_SPAWN_FILE);
            mapData->writeString(spawnFile);

            // house file.
            if(version > 1) {
                mapData->writeU8(OTBM_ATTR_HOUSE_FILE);
                mapData->writeString(houseFile);
            }

            /// write tiles first
            BinaryTreePtr tileArea = mapData->makeChild(OTBM_TILE_AREA);
            Position base(-1, -1, -1);
            for(const auto& pair : m_tiles) {
                Position pos = pair.first;
                TilePtr tile = pair.second;

                /// base position
                if(pos.x < base.x || pos.x >= base.x + 256 || pos.y < base.y|| pos.y >= base.y + 256 ||
                        pos.z != base.z) {
                    tileArea->writePos(base = pos & 0xFF00);
                }

                BinaryTreePtr tileNode(nullptr);
                uint32 flags = tile->getFlags();
                if((flags & TILESTATE_HOUSE) == TILESTATE_HOUSE)
                    tileNode = tileArea->makeChild(OTBM_HOUSETILE);
                else
                    tileNode = tileArea->makeChild(OTBM_TILE);

                tileNode->writePoint(Point(pos.x, pos.y));
//                if(tileNode->getType() == OTBM_HOUSETILE)
//                    tileNode->writeU32(tile->getHouseId());

                /// Tile flags
                if(flags != 0) {
                    tileNode->writeU8(OTBM_ATTR_TILE_FLAGS);
                    tileNode->writeU32(flags);
                }

                /// start writing tile items
                for(const ItemPtr& item : tile->getItems()) {
                    BinaryTreePtr itemNode = tileNode->makeChild(OTBM_ATTR_ITEM);
                    item->serializeItem(itemNode);
                }
            }

            /// write towns
            BinaryTreePtr townNode = mapData->makeChild(OTBM_TOWNS);
            for(const TownPtr& town : m_towns.getTowns()) {
                BinaryTreePtr newTown = townNode->makeChild(OTBM_TOWN);
                {
                    newTown->writeU32(town->getId());
                    newTown->writeString(town->getName());
                    newTown->writePos(town->getPos());
                }
            }

            /// write waypoints
            if(version > 1) {
                BinaryTreePtr waypointNode = mapData->makeChild(OTBM_WAYPOINTS);
                for(const auto& it : m_waypoints) {
                    BinaryTreePtr newWaypoint = waypointNode->makeChild(OTBM_WAYPOINT);
                    {
                        newWaypoint->writeString(it.second);
                        newWaypoint->writePos(it.first);
                    }
                }
            }
        }
    }

    root->writeToFile();
    g_logger.debug(stdext::format("OTBM saving took %ld", time(0) - start));
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

    CreatureTypePtr cType(nullptr);
    for(TiXmlElement* node = root->FirstChildElement(); node; node = node->NextSiblingElement()) {
        if(node->ValueTStr() != "spawn")
            stdext::throw_exception("invalid spawn node");

        Position centerPos = node->readPos("center");
        for(TiXmlElement* cNode = node->FirstChildElement(); cNode; cNode = cNode->NextSiblingElement()) {
            if(cNode->ValueStr() != "monster" && cNode->ValueStr() != "npc")
                stdext::throw_exception(stdext::format("invalid spawn-subnode %s", cNode->ValueStr()));

            std::string cName = cNode->Attribute("name");
            stdext::tolower(cName);
            stdext::trim(cName);

            if (!(cType = m_creatures.getCreature(cName)))
                continue;

            cType->setSpawnTime(cNode->readType<int>("spawntime"));
            CreaturePtr creature(new Creature);
            creature->setOutfit(cType->getOutfit());
            creature->setName(cType->getName());

            addThing(creature, centerPos + cNode->readPoint());
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

        stdext::timer loadTimer;
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

            const TilePtr& tile = g_map.createTile(pos);

            int stackPos = 0;
            while(true) {
                int id = fin->getU16();

                // end of tile
                if(id == 0xFFFF)
                    break;

                int countOrSubType = fin->getU8();

                ItemPtr item = Item::create(id);
                item->setCountOrSubType(countOrSubType);

                if(item->isValid())
                    tile->addThing(item, stackPos++);
            }
        }

        fin->close();

        g_logger.debug(stdext::format("Otcm load time: %.2f seconds", loadTimer.elapsed_seconds()));
        return true;
    } catch(stdext::exception& e) {
        g_logger.error(stdext::format("failed to load OTCM map: %s", e.what()));
        return false;
    }
}

void Map::saveOtcm(const std::string& fileName)
{
#if 0
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
        fin->addU16(g_game.getClientVersion());
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
                const ThingPtr& thing = *it;
                if(thing->isItem()) {
                    ItemPtr item = thing->static_self_cast<Item>();
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
#endif
}