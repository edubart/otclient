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
#include "mapview.h"

#include <framework/core/eventdispatcher.h>
#include <framework/core/application.h>

Map g_map;
TilePtr Map::m_nulltile;

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

void Map::clean()
{
    cleanDynamicThings();

    for(int i=0;i<=Otc::MAX_Z;++i)
        m_tileBlocks[i].clear();

    m_waypoints.clear();

    g_towns.clear();
    g_houses.clear();
    g_creatures.clearSpawns();
    m_tilesRect = Rect(65534, 65534, 0, 0);
}

void Map::cleanDynamicThings()
{
    for(const auto& pair : m_knownCreatures) {
        const CreaturePtr& creature = pair.second;
        removeThing(creature);
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

    if(thing->isItem() || thing->isCreature() || thing->isEffect()) {
        const TilePtr& tile = getOrCreateTile(pos);
        tile->addThing(thing, stackPos);
    } else {
        if(thing->isMissile()) {
            m_floorMissiles[pos.z].push_back(thing->static_self_cast<Missile>());
            thing->onAppear();
        } else if(thing->isAnimatedText()) {
            AnimatedTextPtr animatedText = thing->static_self_cast<AnimatedText>();
            m_animatedTexts.push_back(animatedText);
        } else if(thing->isStaticText()) {
            StaticTextPtr staticText = thing->static_self_cast<StaticText>();
            bool mustAdd = true;
            for(auto other : m_staticTexts) {
                // try to combine messages
                if(other->getPosition() == pos && other->addMessage(staticText->getName(), staticText->getMessageMode(), staticText->getFirstMessage())) {
                    mustAdd = false;
                    break;
                }
            }

            if(mustAdd) {
                m_staticTexts.push_back(staticText);
                staticText->onAppear();
            }
        }

        thing->setPosition(pos);
        thing->onAppear();
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

    if(thing->isMissile()) {
        MissilePtr missile = thing->static_self_cast<Missile>();
        int z = missile->getPosition().z;
        auto it = std::find(m_floorMissiles[z].begin(), m_floorMissiles[z].end(), missile);
        if(it != m_floorMissiles[z].end()) {
            m_floorMissiles[z].erase(it);
            return true;
        }
    } else if(thing->isAnimatedText()) {
        AnimatedTextPtr animatedText = thing->static_self_cast<AnimatedText>();
        auto it = std::find(m_animatedTexts.begin(), m_animatedTexts.end(), animatedText);
        if(it != m_animatedTexts.end()) {
            m_animatedTexts.erase(it);
            return true;
        }
    } else if(thing->isStaticText()) {
        StaticTextPtr staticText = thing->static_self_cast<StaticText>();
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

const TilePtr& Map::createTile(const Position& pos)
{
    if(!pos.isMapPosition())
        return m_nulltile;
    if(pos.x < m_tilesRect.left())
        m_tilesRect.setLeft(pos.x);
    if(pos.y < m_tilesRect.top())
        m_tilesRect.setTop(pos.y);
    if(pos.x > m_tilesRect.right())
        m_tilesRect.setRight(pos.x);
    if(pos.y > m_tilesRect.bottom())
        m_tilesRect.setBottom(pos.y);
    TileBlock& block = m_tileBlocks[pos.z][getBlockIndex(pos)];
    return block.create(pos);
}

template <typename... Items>
const TilePtr& Map::createTileEx(const Position& pos, const Items&... items)
{
    if(!pos.isValid())
        return m_nulltile;
    const TilePtr& tile = getOrCreateTile(pos);
    auto vec = {items...};
    for(auto it : vec)
        addThing(it, pos);

    return tile;
}

const TilePtr& Map::getOrCreateTile(const Position& pos)
{
    if(!pos.isMapPosition())
        return m_nulltile;
    if(pos.x < m_tilesRect.left())
        m_tilesRect.setLeft(pos.x);
    if(pos.y < m_tilesRect.top())
        m_tilesRect.setTop(pos.y);
    if(pos.x > m_tilesRect.right())
        m_tilesRect.setRight(pos.x);
    if(pos.y > m_tilesRect.bottom())
        m_tilesRect.setBottom(pos.y);
    TileBlock& block = m_tileBlocks[pos.z][getBlockIndex(pos)];
    return block.getOrCreate(pos);
}

const TilePtr& Map::getTile(const Position& pos)
{
    if(!pos.isMapPosition())
        return m_nulltile;
    auto it = m_tileBlocks[pos.z].find(getBlockIndex(pos));
    if(it != m_tileBlocks[pos.z].end())
        return it->second.get(pos);
    return m_nulltile;
}

void Map::cleanTile(const Position& pos)
{
    if(!pos.isMapPosition())
        return;
    auto it = m_tileBlocks[pos.z].find(getBlockIndex(pos));
    if(it != m_tileBlocks[pos.z].end()) {
        TileBlock& block = it->second;
        if(const TilePtr& tile = block.get(pos)) {
            tile->clean();
            if(tile->canErase())
                block.remove(pos);

            notificateTileUpdateToMapViews(pos);
        }
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
        if(oldPos != pos) {
            if(!localPlayer->isRemoved())
                localPlayer->onDisappear();
            localPlayer->setPosition(pos);
            localPlayer->onAppear();
            g_logger.debug("forced player position update");
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

std::tuple<std::vector<Otc::Direction>, Otc::PathFindResult> Map::findPath(const Position& startPos, const Position& goalPos, int maxSteps, int flags)
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

    result = Otc::PATHFIND_RESULT_NO_WAY;

    if(startPos == goalPos) {
        result = Otc::PATHFIND_RESULT_SAME_POSITION;
        return ret;
    }

    if(startPos.z != goalPos.z) {
        result = Otc::PATHFIND_RESULT_IMPOSSIBLE;
        return ret;
    }

    if(startPos.distance(goalPos) > maxSteps) {
        result = Otc::PATHFIND_RESULT_TOO_FAR;
        return ret;
    }

    std::unordered_map<Position, Node*, PositionHasher> nodes;
    std::priority_queue<Node*, std::vector<Node*>, LessNode> searchList;

    Node *currentNode = new Node(startPos);
    currentNode->pos = startPos;
    nodes[startPos] = currentNode;
    Node *foundNode = nullptr;
    while(currentNode) {
        // too far
        if(currentNode->steps >= maxSteps) {
            result = Otc::PATHFIND_RESULT_TOO_FAR;
            break;
        }

        // path found
        if(currentNode->pos == goalPos && (!foundNode || currentNode->cost < foundNode->cost))
            foundNode = currentNode;

        // cost too high
        if(foundNode && currentNode->totalCost >= foundNode->cost)
            break;

        for(int i=-1;i<=1;++i) {
            for(int j=-1;j<=1;++j) {
                if(i == 0 && j == 0)
                    continue;

                Position neighborPos = currentNode->pos.translated(i, j);
                const TilePtr& tile = getTile(neighborPos);

                if(neighborPos != goalPos) {
                    if(!(flags & Otc::PATHFIND_ALLOW_NULLTILES) && !tile)
                        continue;
                    if(tile) {
                        if(!(flags & Otc::PATHFIND_ALLOW_CREATURES) && tile->hasCreature())
                            continue;
                        if(!(flags & Otc::PATHFIND_ALLOW_NONPATHABLE) && !tile->isPathable())
                            continue;
                        if(!(flags & Otc::PATHFIND_ALLOW_NONWALKABLE) && !tile->isWalkable())
                            continue;
                    }
                }

                float walkFactor;
                Otc::Direction walkDir = currentNode->pos.getDirectionFromPosition(neighborPos);
                if(walkDir >= Otc::NorthEast)
                    walkFactor = 3.0f;
                else
                    walkFactor = 1.0f;

                int groundSpeed = tile ? tile->getGroundSpeed() : 100;
                float cost = currentNode->cost + (groundSpeed * walkFactor) / 100.0f;

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
        result = Otc::PATHFIND_RESULT_OK;
    }

    for(auto it : nodes)
        delete it.second;

    return ret;
}
