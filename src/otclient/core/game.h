#ifndef GAME_H
#define GAME_H

#include "declarations.h"
#include <otclient/net/declarations.h>

class Game
{
public:
    void init();
    void terminate();

    void loginWorld(const std::string& account,
                    const std::string& password,
                    uint32 worldIp, uint16 worldPort,
                    const std::string& characterName);
    void logout();

    void onLogin();
    void onLogout();

    bool isOnline() { return m_online; }

    LocalPlayerPtr getLocalPlayer() { return m_localPlayer; }
    ProtocolGamePtr getProtocolGame() { return m_protocolGame; }

private:
    LocalPlayerPtr m_localPlayer;
    ProtocolGamePtr m_protocolGame;
    bool m_online;
};

extern Game g_game;

#endif
