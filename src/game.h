#ifndef GAME_H
#define GAME_H

#include <global.h>
#include "map.h"
#include "player.h"
#include "protocolgame.h"

class Game
{
public:
    Game();

    void setProtocol(ProtocolGamePtr protocolGame) { m_protocolGame = protocolGame; }
    ProtocolGamePtr getProtocol() { return m_protocolGame; }

    Map *getMap() { return &m_map; }
    Player *getPlayer() { return &m_player; }

    void setOnline(bool online) { m_online = online; }
    bool getOnline() { return m_online; }

private:
    Map m_map;
    Player m_player;
    ProtocolGamePtr m_protocolGame;
    bool m_online;

};

extern Game g_game;

#endif // GAME_H
