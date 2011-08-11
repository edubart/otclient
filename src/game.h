#ifndef GAME_H
#define GAME_H

#include <global.h>
#include "map.h"
#include "player.h"

class Game
{
public:
    Map *getMap() { return &m_map; }
    Player *getPlayer() { return &m_player; }

private:
    Map m_map;
    Player m_player;

};

extern Game g_game;

#endif // GAME_H
