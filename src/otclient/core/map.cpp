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
#include <framework/graphics/graphics.h>
#include <framework/graphics/framebuffer.h>

Map g_map;

void Map::draw(const Rect& rect)
{
    if(!m_framebuffer)
        m_framebuffer = FrameBufferPtr(new FrameBuffer(MAP_VISIBLE_WIDTH * NUM_TILE_PIXELS, MAP_VISIBLE_HEIGHT * NUM_TILE_PIXELS));

    g_graphics.bindColor(Fw::white);
    m_framebuffer->bind();

    LocalPlayerPtr localPlayer = g_game.getLocalPlayer();
    int walkOffsetX = localPlayer->getWalkOffsetX();
    int walkOffsetY = localPlayer->getWalkOffsetY();

    int maxFloor = getMaxVisibleFloor();
    for(int z = MAX_Z-1; z >= maxFloor; --z) {
        if(z < maxFloor)
            continue;

        int zdif = m_centralPosition.z - z;
        for(int y = 0; y < MAP_SIZE_Y; ++y) {
            for(int x = 0; x < MAP_SIZE_X; ++x) {
                Position tilePos(m_centralPosition.x + (x - PLAYER_OFFSET_X), m_centralPosition.y + (y - PLAYER_OFFSET_Y), m_centralPosition.z);
                tilePos.coveredUp(m_centralPosition.z - z);
                if(const TilePtr& tile = m_tiles[tilePos]) {
                    // skip tiles that are behind another tile
                    if(z > maxFloor && isCompletlyCovered(tilePos, maxFloor))
                        continue;

                    int x = (7 + (tile->getPosition().x - m_centralPosition.x) - zdif) * NUM_TILE_PIXELS;
                    int y = (5 + (tile->getPosition().y - m_centralPosition.y) - zdif) * NUM_TILE_PIXELS;
                    tile->draw(x - walkOffsetX, y - walkOffsetY);
                }
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
    for(int x = 0; x < MAP_VISIBLE_WIDTH; ++x) {
        for(int y = 0; y < MAP_VISIBLE_HEIGHT; ++y) {
            Position tilePos = Position(m_centralPosition.x + (x - PLAYER_OFFSET_X - 1), m_centralPosition.y + (y - PLAYER_OFFSET_Y - 1), m_centralPosition.z);
            if(const TilePtr& tile = m_tiles[tilePos]) {
                auto& creatures = tile->getCreatures();

                if(creatures.size() == 0)
                    continue;

                for(auto it = creatures.rbegin(), end = creatures.rend(); it != end; ++it) {
                    CreaturePtr creature = (*it)->asCreature();

                    int x = (7 + (tilePos.x - m_centralPosition.x))*NUM_TILE_PIXELS + 10 - tile->getDrawNextOffset();
                    int y = (5 + (tilePos.y - m_centralPosition.y))*NUM_TILE_PIXELS - 10 - tile->getDrawNextOffset();

                    if(creature != localPlayer) {
                        x += creature->getWalkOffsetX() - walkOffsetX;
                        y += creature->getWalkOffsetY() - walkOffsetY;
                    }

                    creature->drawInformation(rect.x() + x*horizontalStretchFactor, rect.y() + y*verticalStretchFactor, isCovered(tilePos, maxFloor));
                }
            }
        }
    }
}

int Map::getMaxVisibleFloor()
{
    Position tilePos = m_centralPosition;
    tilePos.up();
    TilePtr tile = m_tiles[tilePos];
    if(tile)
        return m_centralPosition.z;

    tilePos = Position(m_centralPosition);
    while(tilePos.z >= 0) {
        tilePos.coveredUp();
        tile = m_tiles[tilePos];
        if(tile)
            return tilePos.z + 1;
    }
    /*
    int maxZ = MAP_SIZE_Z - 1;
    int currentZ = m_centralPosition.z;
    while(maxZ > currentZ) {
        TilePtr tile = m_tiles[tilePos];
        if(tilç
        maxZ--;
    };

    for(int x = PLAYER_OFFSET_X-1; x <= PLAYER_OFFSET_X+1; ++x) {
        for(int y = PLAYER_OFFSET_Y-1; y <= PLAYER_OFFSET_Y+1; ++y) {
            if(x == PLAYER_OFFSET_X && y == PLAYER_OFFSET_Y)
                continue;
            for(int z = currentZ + 1; z <
        }
    }
    */

    return 0;
}

bool Map::isCovered(const Position& pos, int maxFloor)
{
    Position tilePos = pos;
    tilePos.coveredUp();
    while(tilePos.z >= maxFloor) {
        TilePtr tile = m_tiles[tilePos];
        if(tile)
            return true;
        tilePos.coveredUp();
    }
    return false;
}

bool Map::isCompletlyCovered(const Position& pos, int maxFloor)
{
    Position tilePos = pos;
    tilePos.coveredUp();
    while(tilePos.z >= maxFloor) {
        bool covered = true;
        for(int x=0;x<2;++x) {
            for(int y=0;y<2;++y) {
                TilePtr tile = m_tiles[tilePos + Position(-x, -y, 0)];
                if(!tile || !tile->isOpaque()) {
                    covered = false;
                    break;
                }
            }
        }
        if(covered)
            return true;
        tilePos.coveredUp();
    }
    return false;
}

void Map::addThing(ThingPtr thing, int stackpos)
{
    if(!thing)
        return;

    TilePtr& tile = m_tiles[thing->getPosition()];
    if(!tile) {
        tile = TilePtr(new Tile(thing->getPosition()));
    }

    tile->addThing(thing, stackpos);

    if(const CreaturePtr& creature = thing->asCreature())
        m_creatures[thing->getId()] = creature;
}

ThingPtr Map::getThing(const Position& pos, int stackpos)
{
    if(const TilePtr& tile = m_tiles[pos]) {
        return tile->getThing(stackpos);
    }
    return ThingPtr();
}

void Map::removeThing(const Position& pos, int stackpos)
{
    if(TilePtr& tile = m_tiles[pos]) {
        tile->removeThing(stackpos);
    }
}

void Map::removeThingByPtr(ThingPtr thing)
{
    if(!thing)
        return;

    if(TilePtr& tile = m_tiles[thing->getPosition()]) {
        tile->removeThingByPtr(thing);
    }
}

void Map::clean()
{
    m_tiles.clear();
}

void Map::cleanTile(const Position& pos)
{
    if(TilePtr& tile = m_tiles[pos])
        tile->clean();
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
