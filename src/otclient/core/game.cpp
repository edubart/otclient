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

void Game::loginWorld(const std::string& account, const std::string& password, const std::string& worldHost, int worldPort, const std::string& characterName)
{
    m_protocolGame = ProtocolGamePtr(new ProtocolGame);
    m_protocolGame->login(account, password, worldHost, (uint16)worldPort, characterName);
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

void Game::walk(Direction direction)
{
    if(!m_online)
        return;

    // TODO: check if we can walk.

    m_localPlayer->setDirection(direction);

    switch(direction) {
    case DIRECTION_NORTH:
        m_protocolGame->sendWalkNorth();
        break;
    case DIRECTION_EAST:
        m_protocolGame->sendWalkEast();
        break;
    case DIRECTION_SOUTH:
        m_protocolGame->sendWalkSouth();
        break;
    case DIRECTION_WEST:
        m_protocolGame->sendWalkWest();
        break;
    }
}

void Game::turn(Direction direction)
{
    if(!m_online)
        return;

    switch(direction) {
    case DIRECTION_NORTH:
        m_protocolGame->sendTurnNorth();
        break;
    case DIRECTION_EAST:
        m_protocolGame->sendTurnEast();
        break;
    case DIRECTION_SOUTH:
        m_protocolGame->sendTurnSouth();
        break;
    case DIRECTION_WEST:
        m_protocolGame->sendTurnWest();
        break;
    }
}
