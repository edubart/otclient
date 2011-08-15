#include "game.h"
#include "localplayer.h"

Game g_game;

Game::Game()
{
    m_online = false;
}

void Game::onLogin()
{
    m_localPlayer = LocalPlayerPtr(new LocalPlayer);
    m_online = true;
}

void Game::onLogout()
{
    m_localPlayer.reset();
    m_online = false;
}
