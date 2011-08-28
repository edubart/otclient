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
#include <framework/graphics/graphics.h>
#include <framework/graphics/framebuffer.h>

Map g_map;

void Map::draw(const Rect& rect)
{
    if(!m_framebuffer)
        m_framebuffer = FrameBufferPtr(new FrameBuffer(15*32, 11*32));

    g_graphics.bindColor(Color::white);
    m_framebuffer->bind();

    LocalPlayerPtr player = g_game.getLocalPlayer();
    Position playerPos = player->getPosition();
    double walkOffsetX = player->getWalkOffsetX();
    double walkOffsetY = player->getWalkOffsetY();

    // player is above 7

    int drawFloorStop = 0;
    if(playerPos.z <= 7) {

        // player pos it 8-6. check if we can draw upper floors.


        // if there is a window on north, east, south or west
        //Position direction[4] = {Position(0, -1, 0), Position(1, 0, 0), Position(0, 1, 0), Position(-1, 0, 0)};
        for(int d = 0; d < 4; ++d) {
            /*if(const TilePtr& tile = m_tiles[playerPos+direction[d]]) {
                const ThingAttributes& thingAttributes = thing->getAttributes();
                if(thingAttributes.lookThrough) {
                    drawFloorStop = playerPos.z - 1;
                    break;
                }
            }*/
        }

        // if we have something covering us, dont show floors above.
        for(int jz = 6; jz >= 0; --jz) {
            Position coverPos = Position(playerPos.x+(7-jz)-1, playerPos.y+(7-jz)-1, jz);
            if(const TilePtr& tile = m_tiles[coverPos]) {
                if(tile->getStackSize(3) > 0 && jz < playerPos.z) {
                    drawFloorStop = jz;
                    break;
                }
            }
        }

        for(int iz = 7; iz > 0; --iz) {
            if(iz == drawFloorStop)
                break;

            // +1 in draws cause 64x64 items may affect view.

            for(int step = 0; step < 2; ++step) {
                for(int ix = -8+(playerPos.z-iz); ix < + 8+7; ++ix) {
                    for(int iy = -6+(playerPos.z-iz); iy < + 6+7; ++iy) {
                        Position itemPos = Position(playerPos.x + ix, playerPos.y + iy, iz);
                        if(const TilePtr& tile = m_tiles[itemPos])
                            tile->draw((ix + 7 - (playerPos.z-iz))*32 - walkOffsetX, (iy + 5 - (playerPos.z-iz))*32 - walkOffsetY, step);
                    }
                }
            }
        }
    }

    // debug draws
    g_graphics.bindColor(Color::red);
    g_graphics.drawBoundingRect(Rect(7*32, 5*32, 32, 32));

    m_framebuffer->unbind();

    g_graphics.bindColor(Color::white);
    m_framebuffer->draw(rect);

    // calculate stretch factor
    float horizontalStretchFactor = (rect.width() - rect.x()) / (float)(15*32);
    float verticalStretchFactor = (rect.height() - rect.y()) / (float)(11*32);

    // draw player names and health bars
    for(int ix = -7; ix <= 7; ++ix) {
        for(int iy = -5; iy <= 5; ++iy) {
            Position itemPos = Position(playerPos.x + ix, playerPos.y + iy, playerPos.z);
            if(const TilePtr& tile = m_tiles[itemPos]) {
                std::vector<ThingPtr> creatures = tile->getCreatures();
                for(auto it = creatures.rbegin(), end = creatures.rend(); it != end; ++it) {
                    const ThingPtr& thing = *it;
                    const CreaturePtr& creature = thing->asCreature();

                    int x = (ix + 7)*32 + 10 - tile->getDrawNextOffset();
                    int y = (iy + 5)*32 - 10 - tile->getDrawNextOffset();

                    if(creature != player) {
                        x += creature->getWalkOffsetX() - walkOffsetX;
                        y += creature->getWalkOffsetY() - walkOffsetY;
                    }



                    // TODO: create isCovered function.
                    bool useGray = (drawFloorStop != playerPos.z-1);

                    creature->drawInformation(x*horizontalStretchFactor, y*verticalStretchFactor, useGray);
                }
            }
        }
    }
}

void Map::addThing(ThingPtr thing, int stackpos)
{
    if(!thing)
        return;

    TilePtr& tile = m_tiles[thing->getPosition()];
    if(!tile) {
        tile = TilePtr(new Tile());
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
    if(g_game.getLocalPlayer()->getId() == id)
        return g_game.getLocalPlayer();
    return m_creatures[id];
}

void Map::removeCreatureById(uint32 id)
{
    m_creatures.erase(id);
}
