#include "map.h"
#include "game.h"
#include <graphics/graphics.h>
#include <graphics/framebuffer.h>

void Map::draw(int x, int y)
{
    if(!m_framebuffer)
        m_framebuffer = FrameBufferPtr(new FrameBuffer(15*32, 11*32));

    m_framebuffer->bind();

    Position *playerPos = g_game.getPlayer()->getPosition();

    // player is above 7
    if(playerPos->z <= 7) {

        // player pos it 8-6. check if we can draw upper floors.
        /*bool draw = true;
        for(int jz = 6; jz >= 0; --jz) {
            if(m_tiles[Position(8+(6-jz), 6+(6-jz), jz)]->getStackSize() > 0) {
                draw = false;
            }
        }*/

        for(int iz = 7; iz > 0; --iz) {

            // +1 in draws cause 64x64 items may affect view.
            for(int ix = - 8; ix < + 8; ++ix) {
                for(int iy = - 6; iy < + 6; ++iy) {
                    Position relativePos = Position(playerPos->x + ix, playerPos->y + iy, iz);
                    //Position drawPos = Position(ix + 8, iy - playerPos->y + 6, iz);
                    //logDebug("x: ", relativePos.x, " y: ", relativePos.y, " z: ", (int)relativePos.z);
                    if(m_tiles.find(relativePos) != m_tiles.end())
                        m_tiles[relativePos]->draw(ix + 8, iy + 6, iz);
                }
            }

            //if(!draw)
                //break;
        }

    }

    m_framebuffer->unbind();

    m_framebuffer->draw(x, y, g_graphics.getScreenSize().width(), g_graphics.getScreenSize().height());
}

void Map::addThing(Thing *thing, const Position& pos)
{
    if(m_tiles.find(pos) == m_tiles.end()) {
        m_tiles[pos] = TilePtr(new Tile());
    }
    m_tiles[pos]->addThing(thing);
}
