#include "game.h"
#include "localplayer.h"
#include <otclient/net/protocolgame.h>

Game g_game;

void Game::init()
{
    m_online = false;
}

void Game::terminate()
{
    if(m_online)
        logout();
}

void Game::loginWorld(const std::string& account, const std::string& password, uint32 worldIp, uint16 worldPort, const std::string& characterName)
{
    m_protocolGame = ProtocolGamePtr(new ProtocolGame);
    m_protocolGame->login(account, password, worldIp, worldPort, characterName);
}

void Game::logout()
{
    m_protocolGame->sendLogout();
    onLogout();
}

void Game::onLogin()
{
    m_localPlayer = LocalPlayerPtr(new LocalPlayer);
    m_online = true;
}

void Game::onLogout()
{
    if(m_protocolGame) {
        m_protocolGame->disconnect();
        m_protocolGame.reset();
    }
    m_localPlayer.reset();
    m_online = false;
}
