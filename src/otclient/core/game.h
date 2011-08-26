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
                    const std::string& worldHost, int worldPort,
                    const std::string& characterName);
    void logout();

    void onLogin();
    void onLogout();

    void walk(Direction direction);
    void turn(Direction direction);

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
