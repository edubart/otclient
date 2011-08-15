#ifndef GAME_H
#define GAME_H

#include "declarations.h"
#include <otclient/net/declarations.h>

class Game
{
public:
    Game();

    void onLogin();
    void onLogout();

    void setProtocol(ProtocolGamePtr protocolGame) { m_protocolGame = protocolGame; }

    bool isOnline() { return m_online; }

    ProtocolGamePtr getProtocol() { return m_protocolGame; }
    LocalPlayerPtr getLocalPlayer() { return m_localPlayer; }

private:
    LocalPlayerPtr m_localPlayer;
    ProtocolGamePtr m_protocolGame;
    bool m_online;
};

extern Game g_game;

#endif
