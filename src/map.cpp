#include "map.h"
#include "game.h"
#include <graphics/graphics.h>

void Map::draw(int x, int y)
{
    if(!m_framebuffer)
        m_framebuffer = FrameBufferPtr(new FrameBuffer(15*32, 11*32));

    m_framebuffer->bind();

    Position playerPos = g_game.getPlayer()->getPosition();

    // player is above 7
    if(playerPos.getZ() <= 7) {

        // player pos it 8-6. check if we can draw upper floors.
        bool draw = true;
        for(int jz = 6; jz >= 0; --jz) {
            if(m_map[8+(6-jz)][6+(6-jz)][jz].getStackSize() > 0) {
                draw = false;
            }
        }

        for(int iz = 7; iz > 0; --iz) {

            // +1 in draws cause 64x64 items may affect view.
            for(int ix = 0; ix < 24; ++ix) {
                for(int iy = 0; iy < 20; ++iy) {
                    m_map[ix+1][iy+1][iz].draw(ix, iy, iz);
                }
            }

            if(!draw)
                break;
        }

    }

    m_framebuffer->unbind();

    m_framebuffer->draw(x, y, g_graphics.getScreenSize().width(), g_graphics.getScreenSize().height());
}

void Map::addThing(Thing *thing, const Position& pos)
{
    Position playerPos = g_game.getPlayer()->getPosition();
    Position relativePos = Position(pos.getX() - playerPos.getX() + 8, pos.getY() - playerPos.getY() + 6, pos.getZ());

    if(relativePos.getX() >= 25 || relativePos.getY() >= 21 || relativePos.getZ() >= 15) {
        logDebug("relativePos is invalid.");
        return;
    }

    //logDebug("x: ", (int)relativePos.getX(), " y: ", (int)relativePos.getY(), " z: ", (int)relativePos.getZ());
    m_map[relativePos.getX()][relativePos.getY()][relativePos.getZ()].addThing(thing);
}
