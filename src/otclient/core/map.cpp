#include "map.h"
#include "game.h"
#include <framework/graphics/graphics.h>
#include <framework/graphics/framebuffer.h>

void Map::draw(int x, int y)
{
    if(!m_framebuffer)
        m_framebuffer = FrameBufferPtr(new FrameBuffer(15*32, 11*32));

    g_graphics.bindColor(Color::white);
    m_framebuffer->bind();

    Position *playerPos = g_game.getPlayer()->getPosition();

    // player is above 7
    if(playerPos->z <= 7) {

        // player pos it 8-6. check if we can draw upper floors.
        bool draw = true;
        for(int jz = 6; jz >= 0; --jz) {
            Position coverPos = Position(playerPos->x-(6-jz), playerPos->y-(6-jz), jz);
            if(m_tiles[coverPos]) {
                if(m_tiles[coverPos]->getStackSize() > 0 && jz < playerPos->z) {
                    draw = false;
                }
            }
        }

        for(int iz = 7; iz > 0; --iz) {

            // +1 in draws cause 64x64 items may affect view.
            for(int ix = -7; ix < + 8+7; ++ix) {
                for(int iy = -5; iy < + 6+7; ++iy) {
                    Position itemPos = Position(playerPos->x + ix, playerPos->y + iy, iz);
                    //Position drawPos = Position(ix + 8, iy - playerPos->y + 6, iz);
                    //logDebug("x: ", relativePos.x, " y: ", relativePos.y, " z: ", (int)relativePos.z);
                    if(m_tiles[itemPos])
                        m_tiles[itemPos]->draw((ix + 7 - (7-iz))*32, (iy + 5 - (7-iz))*32);
                }
            }

            if(!draw)
                break;
        }

    }

    // draw effects
    for(auto it = m_effects.begin(), end = m_effects.end(); it != end; ++it) {
        Position *effectPos = (*it)->getPosition();
        (*it)->draw((effectPos->x - playerPos->x + 7) * 32, (effectPos->y - playerPos->y + 5) * 32);
    }

    // debug draws
    g_graphics.drawBoundingRect(Rect(7*32, 5*32, 32, 32), Color::red);

    m_framebuffer->unbind();
    m_framebuffer->draw(Rect(x, y, g_graphics.getScreenSize()));
}

void Map::addThing(ThingPtr thing, uint8 stackpos)
{
    if(thing->getType() == Thing::TYPE_ITEM || thing->getType() == Thing::TYPE_CREATURE) {
        if(!m_tiles[*thing->getPosition()]) {
            m_tiles[*thing->getPosition()] = TilePtr(new Tile());
        }

        m_tiles[*thing->getPosition()]->addThing(thing, stackpos);
    }
    else if(thing->getType() == Thing::TYPE_EFFECT) {
        m_effects.push_back(thing);
    }
}
