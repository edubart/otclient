/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "game.h"
#include "localplayer.h"
#include "map.h"
#include "tile.h"
#include <otclient/net/protocolgame.h>
#include <framework/core/eventdispatcher.h>

Game g_game;

void Game::loginWorld(const std::string& account, const std::string& password, const std::string& worldHost, int worldPort, const std::string& characterName)
{
    m_protocolGame = ProtocolGamePtr(new ProtocolGame);
    m_protocolGame->login(account, password, worldHost, (uint16)worldPort, characterName);
}

void Game::cancelLogin()
{
    processLogout();
}

void Game::logout(bool force)
{
    if(!m_protocolGame || !m_online)
        return;

    m_protocolGame->sendLogout();

    if(force)
        processLogout();
}

void Game::processLoginError(const std::string& error)
{
    g_lua.callGlobalField("Game", "onLoginError", error);
}

void Game::processConnectionError(const boost::system::error_code& error)
{
    // connection errors only have meaning if we still have a protocol
    if(m_protocolGame) {
        if(error != asio::error::eof)
            g_lua.callGlobalField("Game", "onConnectionError", error.message());

        processLogout();
    }
}

void Game::processLogin(const LocalPlayerPtr& localPlayer)
{
    m_localPlayer = localPlayer;
    m_online = true;

    g_lua.callGlobalField("Game", "onLogin", m_localPlayer);
}

void Game::processLogout()
{
    if(m_online) {
        g_lua.callGlobalField("Game", "onLogout", m_localPlayer);

        m_localPlayer.reset();
        m_online = false;
    }

    if(m_protocolGame) {
        m_protocolGame->disconnect();
        m_protocolGame.reset();
    }
}

void Game::processTextMessage(int type, const std::string& message)
{
    g_lua.callGlobalField("Game","onTextMessage", type, message);
}

void Game::processInventoryChange(int slot, const ItemPtr& item)
{
    g_lua.callGlobalField("Game","onInventoryChange", slot, item);
}

void Game::walk(Otc::Direction direction)
{
    if(!m_online || !m_localPlayer->canWalk(direction))
        return;

    m_localPlayer->clientWalk(direction);

    switch(direction) {
    case Otc::North:
        m_protocolGame->sendWalkNorth();
        break;
    case Otc::East:
        m_protocolGame->sendWalkEast();
        break;
    case Otc::South:
        m_protocolGame->sendWalkSouth();
        break;
    case Otc::West:
        m_protocolGame->sendWalkWest();
        break;
    case Otc::NorthEast:
        m_protocolGame->sendWalkNorthEast();
        break;
    case Otc::SouthEast:
        m_protocolGame->sendWalkSouthEast();
        break;
    case Otc::SouthWest:
        m_protocolGame->sendWalkSouthWest();
        break;
    case Otc::NorthWest:
        m_protocolGame->sendWalkNorthWest();
        break;
    }
}

void Game::turn(Otc::Direction direction)
{
    if(!m_online)
        return;

    switch(direction) {
    case Otc::North:
        m_protocolGame->sendTurnNorth();
        break;
    case Otc::East:
        m_protocolGame->sendTurnEast();
        break;
    case Otc::South:
        m_protocolGame->sendTurnSouth();
        break;
    case Otc::West:
        m_protocolGame->sendTurnWest();
        break;
    }
}

void Game::look(const Position& position)
{
    const TilePtr& tile = g_map.getTile(position);
    if(tile) {
        int stackpos = tile->getLookStackpos();
        ThingPtr thing = tile->getThing(stackpos);
        if(thing)
            m_protocolGame->sendLookAt(position, thing->getId(), stackpos);
    }
}

void Game::talkChannel(int channelType, int channelId, const std::string& message)
{
    if(!m_online)
        return;

    m_protocolGame->sendTalk(channelType, channelId, "", message);
}

void Game::talkPrivate(int channelType, const std::string& receiver, const std::string& message)
{
    if(!m_online)
        return;

    m_protocolGame->sendTalk(channelType, 0, receiver, message);
}

void Game::openOutfitWindow()
{
    m_protocolGame->sendGetOutfit();
}

void Game::setOutfit(const Outfit& outfit)
{
    m_protocolGame->sendSetOutfit(outfit);
}
