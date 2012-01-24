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

#include <framework/graphics/graphics.h>
#include <framework/graphics/framebuffer.h>
#include <framework/graphics/paintershaderprogram.h>
#include <framework/graphics/paintershadersources.h>
#include <framework/graphics/texture.h>

Map g_map;

Map::Map()
{
    setVisibleSize(Size(MAP_VISIBLE_WIDTH, MAP_VISIBLE_HEIGHT));
}

void Map::draw(const Rect& rect)
{
    if(!m_framebuffer) {
        Size fboSize(m_visibleSize.width() * NUM_TILE_PIXELS, m_visibleSize.height() * NUM_TILE_PIXELS);
        m_framebuffer = FrameBufferPtr(new FrameBuffer(fboSize));
        m_framebuffer->setClearColor(Fw::black);


        m_shaderProgram = PainterShaderProgramPtr(new PainterShaderProgram);
        m_shaderProgram->addShaderFromSourceCode(Shader::Vertex, glslMainWithTexCoordsVertexShader + glslPositionOnlyVertexShader);
        m_shaderProgram->addShaderFromSourceFile(Shader::Fragment, "/game_shaders/map.frag");
        assert(m_shaderProgram->link());
    }

    g_painter.setColor(Fw::white);
    m_framebuffer->bind();

    // draw offsets
    LocalPlayerPtr localPlayer = g_game.getLocalPlayer();
    if(localPlayer)
        m_drawOffset = localPlayer->getWalkOffset();

    //TODO: cache first/last visible floor
    // draw from bottom floors to top floors
    int firstFloor = getFirstVisibleFloor();
    const int lastFloor = MAX_Z-1;
    for(int iz = lastFloor; iz >= firstFloor; --iz) {
        // draw tiles like linus pauling's rule order
        const int numDiagonals = m_size.width() + m_size.height() - 1;
        for(int diagonal = 0; diagonal < numDiagonals; ++diagonal) {
            // loop through / diagonal tiles
            for(int ix = std::min(diagonal, m_size.width() - 1), iy = std::max(diagonal - m_size.width(), 0); ix >= 0 && iy < m_size.height(); --ix, ++iy) {
                // position on current floor
                Position tilePos(m_centralPosition.x + (ix - m_centralOffset.x), m_centralPosition.y + (iy - m_centralOffset.y), m_centralPosition.z);
                // adjust tilePos to the wanted floor
                tilePos.perspectiveUp(m_centralPosition.z - iz);
                //TODO: cache visible tiles, m_tiles[] has a high cost (50% fps decrease)
                if(const TilePtr& tile = m_tiles[tilePos]) {
                    // skip tiles that are behind another tile
                    //if(isCompletlyCovered(tilePos, firstFloor))
                    //    continue;
                    tile->draw(positionTo2D(tilePos) - m_drawOffset, rect);
                }
            }
        }

        // after drawing all tiles, draw shots
        for(const MissilePtr& shot : m_missilesAtFloor[iz]) {
            Position missilePos = shot->getPos();
            shot->draw(positionTo2D(missilePos) - m_drawOffset, rect);
        }
    }

    m_framebuffer->release();


    g_painter.setCustomProgram(m_shaderProgram);
    g_painter.setColor(Fw::white);
    m_framebuffer->draw(rect);
    g_painter.releaseCustomProgram();

    // calculate stretch factor
    float horizontalStretchFactor = rect.width() / (float)(m_visibleSize.width() * NUM_TILE_PIXELS);
    float verticalStretchFactor = rect.height() / (float)(m_visibleSize.height() * NUM_TILE_PIXELS);

    // draw player names and health bars
    //TODO: this must be cached with creature walks
    for(int x = 0; x < m_visibleSize.width(); ++x) {
        for(int y = 0; y < m_visibleSize.height(); ++y) {
            Position tilePos = Position(m_centralPosition.x + (x - m_centralOffset.x + 1), m_centralPosition.y + (y - m_centralOffset.y + 1), m_centralPosition.z);
            if(const TilePtr& tile = m_tiles[tilePos]) {
                auto creatures = tile->getCreatures();

                if(creatures.size() == 0)
                    continue;

                for(const CreaturePtr& creature : creatures) {
                    Point p((m_centralOffset.x - 1 + (tilePos.x - m_centralPosition.x))*NUM_TILE_PIXELS + 10 - tile->getDrawElevation(),
                            (m_centralOffset.y - 1 + (tilePos.y - m_centralPosition.y))*NUM_TILE_PIXELS - 10 - tile->getDrawElevation());

                    if(creature != localPlayer) {
                        p += creature->getWalkOffset() - m_drawOffset;
                    }

                    creature->drawInformation(rect.x() + p.x*horizontalStretchFactor, rect.y() + p.y*verticalStretchFactor, isCovered(tilePos, firstFloor), rect);
                }
            }
        }
    }

    // draw static text
    for(auto it = m_staticTexts.begin(), end = m_staticTexts.end(); it != end; ++it) {
        Point pos = positionTo2D((*it)->getPos()) - m_drawOffset;
        pos.x *= horizontalStretchFactor;
        pos.y *= verticalStretchFactor;
        (*it)->draw(rect.topLeft() + pos, rect);
    }

    // draw animated text
    for(auto it = m_animatedTexts.begin(), end = m_animatedTexts.end(); it != end; ++it) {
        Point pos = positionTo2D((*it)->getPos()) - m_drawOffset;
        pos.x *= horizontalStretchFactor;
        pos.y *= verticalStretchFactor;
        (*it)->draw(rect.topLeft() + pos, rect);
    }
}

void Map::clean()
{
    m_tiles.clear();
    m_creatures.clear();
    for(int i=0;i<MAX_Z-1;++i)
        m_missilesAtFloor[i].clear();
    m_animatedTexts.clear();
    m_staticTexts.clear();
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
                            ThingType *type = firstThing->getType();
                            if((type->properties[ThingType::IsGround] || type->properties[ThingType::IsOnBottom]) && !type->properties[ThingType::DontHide]) {
                                firstFloor = upperPos.z + 1;
                                break;
                            }
                        }
                    }
                    if(TilePtr tile = m_tiles[perspectivePos]) {
                        if(ThingPtr firstThing = tile->getThing(0)) {
                            ThingType *type = firstThing->getType();
                            if((type->properties[ThingType::IsGround] || type->properties[ThingType::IsOnBottom]) && !type->properties[ThingType::DontHide]) {
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

bool Map::isCovered(const Position& pos, int firstFloor)
{
    Position tilePos = pos;
    tilePos.perspectiveUp();
    while(tilePos.z >= firstFloor) {
        TilePtr tile = m_tiles[tilePos];
        if(tile && tile->isFullGround())
            return true;
        tilePos.perspectiveUp();
    }
    return false;
}

bool Map::isCompletlyCovered(const Position& pos, int firstFloor)
{
    Position tilePos = pos;
    tilePos.perspectiveUp();
    while(tilePos.z >= firstFloor) {
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

    Position oldPos = thing->getPos();
    bool teleport = false;
    if(oldPos.isValid() && !oldPos.isInRange(pos,1,1,0))
        teleport = true;

    TilePtr tile = getTile(pos);

    if(CreaturePtr creature = thing->asCreature()) {
        tile->addThing(thing, stackPos);
        m_creatures[creature->getId()] = creature;

        if(teleport)
            g_game.processCreatureTeleport(creature);
    }
    else if(MissilePtr shot = thing->asMissile()) {
        m_missilesAtFloor[shot->getPos().z].push_back(shot);
    }
    else if(AnimatedTextPtr animatedText = thing->asAnimatedText()) {
        m_animatedTexts.push_back(animatedText);
    }
    else if(StaticTextPtr staticText = thing->asStaticText()) {
        bool mustAdd = true;
        for(auto it = m_staticTexts.begin(), end = m_staticTexts.end(); it != end; ++it) {
            StaticTextPtr cStaticText = *it;
            if(cStaticText->getPos() == pos) {
                // try to combine messages
                if(cStaticText->addMessage(staticText->getName(), staticText->getMessageType(), staticText->getFirstMessage())) {
                    mustAdd = false;
                    break;
                }
                else {
                    // must add another message and rearrenge current
                }
            }

        }

        if(mustAdd)
            m_staticTexts.push_back(staticText);
    }
    else {
        tile->addThing(thing, stackPos);
    }

    thing->start();
    thing->setPos(pos);
}

ThingPtr Map::getThing(const Position& pos, int stackPos)
{
    if(const TilePtr& tile = m_tiles[pos])
        return tile->getThing(stackPos);
    return nullptr;
}

void Map::removeThingByPos(const Position& pos, int stackPos)
{
    if(TilePtr& tile = m_tiles[pos])
        tile->removeThingByStackpos(stackPos);
}

void Map::removeThing(const ThingPtr& thing)
{
    if(!thing)
        return;

    if(MissilePtr shot = thing->asMissile()) {
        auto it = std::find(m_missilesAtFloor[shot->getPos().z].begin(), m_missilesAtFloor[shot->getPos().z].end(), shot);
        if(it != m_missilesAtFloor[shot->getPos().z].end()) {
            m_missilesAtFloor[shot->getPos().z].erase(it);
        }
        return;
    }
    else if(AnimatedTextPtr animatedText = thing->asAnimatedText()) {
        auto it = std::find(m_animatedTexts.begin(), m_animatedTexts.end(), animatedText);
        if(it != m_animatedTexts.end())
            m_animatedTexts.erase(it);
        return;
    }
    else if(StaticTextPtr staticText = thing->asStaticText()) {
        auto it = std::find(m_staticTexts.begin(), m_staticTexts.end(), staticText);
        if(it != m_staticTexts.end())
            m_staticTexts.erase(it);
        return;
    }

    if(TilePtr& tile = m_tiles[thing->getPos()])
        tile->removeThing(thing);
}

TilePtr Map::getTile(const Position& pos)
{
    if(!pos.isValid())
        return nullptr;

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
    if(g_game.getLocalPlayer() && (uint32)g_game.getLocalPlayer()->getId() == id)
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

void Map::setVisibleSize(const Size& visibleSize)
{
    m_visibleSize = visibleSize;

    if(m_visibleSize.width() > MAX_WIDTH || m_visibleSize.height() > MAX_HEIGHT)
        m_visibleSize = Size(MAP_VISIBLE_WIDTH, MAP_VISIBLE_HEIGHT);

    m_centralOffset = Point(std::ceil(m_visibleSize.width() / 2.0), std::ceil(m_visibleSize.height() / 2.0));
    m_size = m_visibleSize + Size(3, 3);

    if(m_framebuffer) {
        m_framebuffer->resize(Size(m_visibleSize.width() * NUM_TILE_PIXELS, m_visibleSize.height() * NUM_TILE_PIXELS));
    }
}

Point Map::positionTo2D(const Position& position)
{
    return Point((m_centralOffset.x - 1 + (position.x - m_centralPosition.x) - (m_centralPosition.z - position.z)) * NUM_TILE_PIXELS,
                 (m_centralOffset.y - 1 + (position.y - m_centralPosition.y) - (m_centralPosition.z - position.z)) * NUM_TILE_PIXELS);
}
