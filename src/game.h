#ifndef GAME_H
#define GAME_H

#include <global.h>
#include "map.h"
#include "player.h"
#include "protocolgame.h"

class Game
{
public:
    void setProtocol(ProtocolGame *protocolGame) { m_protocolGame = protocolGame; }
    ProtocolGame *getProtocol() { return m_protocolGame; }

    Map *getMap() { return &m_map; }
    Player *getPlayer() { return &m_player; }

private:
    Map m_map;
    Player m_player;
    ProtocolGame *m_protocolGame;

};

extern Game g_game;

#endif // GAME_H
