#include "map.h"
#include "game.h"
#include "localplayer.h"
#include <framework/graphics/graphics.h>
#include <framework/graphics/framebuffer.h>

Map g_map;

void Map::draw(int x, int y)
{
    if(!m_framebuffer)
        m_framebuffer = FrameBufferPtr(new FrameBuffer(15*32, 11*32));

    g_graphics.bindColor(Color::white);
    m_framebuffer->bind();

    Position playerPos = g_game.getLocalPlayer()->getPosition();

    // player is above 7
    if(playerPos.z <= 7) {

        // player pos it 8-6. check if we can draw upper floors.
        int drawFloorStop = 0;
        for(int jz = 6; jz >= 0; --jz) {
            Position coverPos = Position(playerPos.x+(7-jz)-1, playerPos.y+(7-jz)-1, jz);
            if(const TilePtr& tile = m_tiles[coverPos]) {
                if(tile->getStackSize() > 0 && jz < playerPos.z) {
                    drawFloorStop = jz;
                    break;
                }
            }
        }

        for(int iz = 7; iz > 0; --iz) {
            if(iz == drawFloorStop)
                break;

            // +1 in draws cause 64x64 items may affect view.

            for(int ix = -7+(playerPos.z-iz); ix < + 8+7; ++ix) {
                for(int iy = -5+(playerPos.z-iz); iy < + 6+7; ++iy) {
                    Position itemPos = Position(playerPos.x + ix, playerPos.y + iy, iz);
                    if(const TilePtr& tile = m_tiles[itemPos])
                        tile->draw((ix + 7 - (playerPos.z-iz))*32, (iy + 5 - (playerPos.z-iz))*32);
                }
            }
        }
    }

    // debug draws
    g_graphics.drawBoundingRect(Rect(7*32, 5*32, 32, 32), Color::red);

    m_framebuffer->unbind();
    m_framebuffer->draw(Rect(x, y, g_graphics.getScreenSize()));
}

void Map::addThing(ThingPtr thing, uint8 stackpos)
{
    TilePtr& tile = m_tiles[thing->getPosition()];
    if(!tile) {
        tile = TilePtr(new Tile());
    }

    tile->addThing(thing, stackpos);

    if(const CreaturePtr& creature = thing->asCreature())
        m_creatures[thing->getId()] = creature;
}

ThingPtr Map::getThing(const Position& pos, uint8 stackpos)
{
    if(const TilePtr& tile = m_tiles[pos]) {
        return tile->getThing(stackpos);
    }
    return ThingPtr();
}

void Map::removeThing(const Position& pos, uint8 stackpos)
{
    if(TilePtr& tile = m_tiles[pos]) {
        tile->removeThing(stackpos);
    }
}

void Map::clean()
{
    m_tiles.clear();
    m_creatures.clear();
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
