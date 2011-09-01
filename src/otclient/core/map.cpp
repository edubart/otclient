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

#include "map.h"
#include "game.h"
#include "localplayer.h"
#include "tile.h"
#include "item.h"
#include <framework/graphics/graphics.h>
#include <framework/graphics/framebuffer.h>

Map g_map;

void Map::draw(const Rect& rect)
{
    if(!m_framebuffer)
        m_framebuffer = FrameBufferPtr(new FrameBuffer(MAP_VISIBLE_WIDTH * NUM_TILE_PIXELS, MAP_VISIBLE_HEIGHT * NUM_TILE_PIXELS));

    g_graphics.bindColor(Fw::white);
    m_framebuffer->bind();

    // draw offsets
    LocalPlayerPtr localPlayer = g_game.getLocalPlayer();
    int walkOffsetX = localPlayer->getWalkOffsetX();
    int walkOffsetY = localPlayer->getWalkOffsetY();

    // draw from bottom floors to top floors
    int firstFloor = getFirstVisibleFloor();
    const int lastFloor = MAX_Z-1;
    for(int iz = lastFloor; iz >= firstFloor; --iz) {
        // draw tiles in linus pauling's rule order
        const int numDiagonals = MAP_SIZE_X + MAP_SIZE_Y - 1;
        for(int diagonal = 0; diagonal < numDiagonals; ++diagonal) {
            // loop through / diagonal tiles
            for(int ix = std::min(diagonal, MAP_SIZE_X - 1), iy = std::max(diagonal - MAP_SIZE_X, 0); ix >= 0 && iy < MAP_SIZE_Y; --ix, ++iy) {
                // position on current floor
                Position tilePos(m_centralPosition.x + (ix - PLAYER_OFFSET_X), m_centralPosition.y + (iy - PLAYER_OFFSET_Y), m_centralPosition.z);
                // adjust tilePos to the wanted floor
                tilePos.perspectiveUp(m_centralPosition.z - iz);
                // TODO: skip tiles that are behind another tile
                if(const TilePtr& tile = m_tiles[tilePos])
                    tile->draw(tilePos.to2D(m_centralPosition) - Point(walkOffsetX, walkOffsetY));
            }
        }
    }

    m_framebuffer->unbind();

    g_graphics.bindColor(Fw::white);
    m_framebuffer->draw(rect);

    // calculate stretch factor
    float horizontalStretchFactor = rect.width() / (float)(MAP_VISIBLE_WIDTH * NUM_TILE_PIXELS);
    float verticalStretchFactor = rect.height() / (float)(MAP_VISIBLE_HEIGHT * NUM_TILE_PIXELS);

    // draw player names and health bars
    //TODO: this could be cached to improve framerate
    for(int x = 0; x < MAP_VISIBLE_WIDTH; ++x) {
        for(int y = 0; y < MAP_VISIBLE_HEIGHT; ++y) {
            Position tilePos = Position(m_centralPosition.x + (x - PLAYER_OFFSET_X + 1), m_centralPosition.y + (y - PLAYER_OFFSET_Y + 1), m_centralPosition.z);
            if(const TilePtr& tile = m_tiles[tilePos]) {
                auto creatures = tile->getCreatures();

                if(creatures.size() == 0)
                    continue;

                for(const CreaturePtr& creature : creatures) {
                    int x = (7 + (tilePos.x - m_centralPosition.x))*NUM_TILE_PIXELS + 10 - tile->getDrawElevation();
                    int y = (5 + (tilePos.y - m_centralPosition.y))*NUM_TILE_PIXELS - 10 - tile->getDrawElevation();

                    if(creature != localPlayer) {
                        x += creature->getWalkOffsetX() - walkOffsetX;
                        y += creature->getWalkOffsetY() - walkOffsetY;
                    }

                    creature->drawInformation(rect.x() + x*horizontalStretchFactor, rect.y() + y*verticalStretchFactor, isCovered(tilePos, firstFloor));
                }
            }
        }
    }
}

void Map::clean()
{
    m_tiles.clear();
}

int Map::getFirstVisibleFloor()
{
    int firstFloor = 0;
    for(int ix = -1; ix <= 1 && firstFloor < m_centralPosition.z; ++ix) {
        for(int iy = -1; iy <= 1 && firstFloor < m_centralPosition.z; ++iy) {
            Position currentPos(m_centralPosition.x + ix, m_centralPosition.y + iy, m_centralPosition.z);
            if((ix == 0 && iy == 0) || isLookPossible(currentPos)) {
                Position upperPos = currentPos;
                Position perspectivePos = currentPos;
                perspectivePos.perspectiveUp();
                upperPos.up();
                while(upperPos.z >= firstFloor) {
                    if(TilePtr tile = m_tiles[upperPos]) {
                        if(ThingPtr firstThing = tile->getThing(0)) {
                            const ThingType type = firstThing->getType();
                            if((type.isGround || type.isOnBottom) && !type.isDontHide) {
                                firstFloor = upperPos.z + 1;
                                break;
                            }
                        }
                    }
                    if(TilePtr tile = m_tiles[perspectivePos]) {
                        if(ThingPtr firstThing = tile->getThing(0)) {
                            const ThingType type = firstThing->getType();
                            if((type.isGround || type.isOnBottom) && !type.isDontHide) {
                                firstFloor = perspectivePos.z + 1;
                                break;
                            }
                        }
                    }
                    perspectivePos.perspectiveUp();
                    upperPos.up();
                }
            }
        }
    }
    return firstFloor;
}

bool Map::isLookPossible(const Position& pos)
{
    TilePtr tile = m_tiles[pos];
    if(tile)
        return tile->isLookPossible();
    return true;
}

bool Map::isCovered(const Position& pos, int maxFloor)
{
    Position tilePos = pos;
    tilePos.perspectiveUp();
    while(tilePos.z >= maxFloor) {
        TilePtr tile = m_tiles[tilePos];
        if(tile && tile->isFullGround())
            return true;
        tilePos.perspectiveUp();
    }
    return false;
}

bool Map::isCompletlyCovered(const Position& pos, int maxFloor)
{
    Position tilePos = pos;
    tilePos.perspectiveUp();
    while(tilePos.z >= maxFloor) {
        bool covered = true;
        for(int x=0;x<2;++x) {
            for(int y=0;y<2;++y) {
                TilePtr tile = m_tiles[tilePos + Position(-x, -y, 0)];
                if(!tile || !tile->isFullyOpaque()) {
                    covered = false;
                    break;
                }
            }
        }
        if(covered)
            return true;
        tilePos.perspectiveUp();
    }
    return false;
}

void Map::addThing(const ThingPtr& thing, const Position& pos, int stackPos)
{
    if(!thing)
        return;

    TilePtr tile = getTile(pos);
    tile->addThing(thing, stackPos);

    if(CreaturePtr creature = thing->asCreature())
        m_creatures[creature ->getId()] = creature;
}

ThingPtr Map::getThing(const Position& pos, int stackPos)
{
    if(const TilePtr& tile = m_tiles[pos])
        return tile->getThing(stackPos);
    return nullptr;
}

void Map::removeThing(const Position& pos, int stackPos)
{
    if(TilePtr& tile = m_tiles[pos])
        tile->removeThing(stackPos);
}

void Map::removeThing(const ThingPtr& thing)
{
    if(!thing)
        return;

    if(TilePtr& tile = m_tiles[thing->getPosition()])
        tile->removeThing(thing);
}

TilePtr Map::getTile(const Position& pos)
{
    TilePtr& tile = m_tiles[pos];
    if(!tile)
        tile = TilePtr(new Tile(pos));
    return tile;
}

void Map::cleanTile(const Position& pos)
{
    if(TilePtr& tile = m_tiles[pos])
        tile->clean();
}

void Map::addCreature(const CreaturePtr& creature)
{
    m_creatures[creature->getId()] = creature;
}

CreaturePtr Map::getCreatureById(uint32 id)
{
    if(g_game.getLocalPlayer() && g_game.getLocalPlayer()->getId() == id)
        return g_game.getLocalPlayer();
    return m_creatures[id];
}

void Map::removeCreatureById(uint32 id)
{
    m_creatures.erase(id);
}

void Map::setCentralPosition(const Position& centralPosition)
{
    m_centralPosition = centralPosition;
}
