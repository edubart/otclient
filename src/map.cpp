#include "map.h"
#include "game.h"

void Map::draw(int x, int y, int width, int heigth)
{
    Position playerPos = g_game.getPlayer()->getPosition();
    for(int ix = 0; ix < 19; ++ix) {
        for(int iy = 0; iy < 15; ++iy) {
            if(playerPos.getZ() >= 7) {
                for(int iz = 7; iz > 0; --iz) {
                    m_map[ix][iy][iz].draw(x+ix*32-(7-iz)*32, y+iy*32-(7-iz)*32);
                }
            }
        }
    }
}

void Map::addThing(Thing *thing, const Position& pos)
{
    Position playerPos = g_game.getPlayer()->getPosition();
    Position relativePos = Position(pos.getX() - playerPos.getX() + 8, pos.getY() - playerPos.getY() + 6, pos.getZ());

    if(relativePos.getX() >= 18)
        logDebug("relativePos is invalid.");
    if(relativePos.getY() >= 14)
        logDebug("relativePos is invalid.");
    if(relativePos.getZ() >= 15)
        logDebug("relativePos is invalid.");

    logDebug("x: ", (int)relativePos.getX(), " y: ", (int)relativePos.getY(), " z: ", (int)relativePos.getZ());
    m_map[relativePos.getX()][relativePos.getY()][relativePos.getZ()].addThing(thing);
}
