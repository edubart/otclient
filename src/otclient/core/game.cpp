/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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
#include "creature.h"
#include "statictext.h"
#include <framework/core/eventdispatcher.h>
#include <framework/ui/uimanager.h>
#include <otclient/luascript/luavaluecasts.h>
#include <otclient/net/protocolgame.h>

Game g_game;

void Game::loginWorld(const std::string& account, const std::string& password, const std::string& worldHost, int worldPort, const std::string& characterName)
{
    m_dead = false;
    m_protocolGame = ProtocolGamePtr(new ProtocolGame);
    m_protocolGame->login(account, password, worldHost, (uint16)worldPort, characterName);
}

void Game::cancelLogin()
{
    processLogout();
}

void Game::logout(bool force)
{
    if(!m_protocolGame || !isOnline())
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

void Game::processLogin(const LocalPlayerPtr& localPlayer, int serverBeat)
{
    m_localPlayer = localPlayer;
    m_serverBeat = serverBeat;

    // NOTE: the entire map description is not known yet
    g_lua.callGlobalField("Game", "onLogin", localPlayer);
}

void Game::processLogout()
{
    if(isOnline()) {
        g_lua.callGlobalField("Game", "onLogout", m_localPlayer);

        m_localPlayer = nullptr;
    }

    if(m_protocolGame) {
        m_protocolGame->disconnect();
        m_protocolGame = nullptr;
    }

    g_map.clean();
}

void Game::processDeath()
{
    m_dead = true;
    g_lua.callGlobalField("Game","onDeath");
}

void Game::processCreatureSpeak(const std::string& name, int level, const std::string& type, const std::string& message, int channelId, const Position& creaturePos)
{
    if(creaturePos.isValid() && (type == "say" || type == "whisper" || type == "yell" || type == "monsterSay" || type == "monsterYell")) {
        StaticTextPtr staticText = StaticTextPtr(new StaticText);
        staticText->addMessage(name, type, message);
        g_map.addThing(staticText, creaturePos);
    }

    g_lua.callGlobalField("Game", "onCreatureSpeak", name, level, type, message, channelId, creaturePos);
}

void Game::processTextMessage(const std::string& type, const std::string& message)
{
    g_lua.callGlobalField("Game","onTextMessage", type, message);
}

void Game::processContainerAddItem(int containerId, const ItemPtr& item)
{
    if(item)
        item->setPos(Position(65535, containerId + 0x40, 0));

    g_lua.callGlobalField("Game", "onContainerAddItem", containerId, item);
}

void Game::processInventoryChange(int slot, const ItemPtr& item)
{
    if(item)
        item->setPos(Position(65535, slot, 0));

    g_lua.callGlobalField("Game","onInventoryChange", slot, item);
}

void Game::processCreatureMove(const CreaturePtr& creature, const Position& oldPos, const Position& newPos)
{
    if(!oldPos.isInRange(newPos, 1, 1, 0))
        logError("unexpected creature move");

    // animate walk
    creature->walk(oldPos, newPos);
}

void Game::processCreatureTeleport(const CreaturePtr& creature)
{
    // stop walking on creature teleports
    creature->stopWalk();
}

void Game::processAttackCancel()
{
    if(m_localPlayer->isAttacking())
        m_localPlayer->setAttackingCreature(nullptr);
}

void Game::processWalkCancel(Otc::Direction direction)
{
    m_localPlayer->cancelWalk(direction);
}

void Game::walk(Otc::Direction direction)
{
    if(!isOnline() || !m_localPlayer->isKnown() || isDead() || !checkBotProtection())
        return;

    if(m_localPlayer->isFollowing())
        cancelFollow();

    if(!m_localPlayer->canWalk(direction))
        return;


    // TODO: restore check for blockable tiles
    /*
    if(toTile && !toTile->isWalkable() && !fromTile->getElevation() >= 3) {
        g_game.processTextMessage("statusSmall", "Sorry, not possible.");
        return false;
    }*/

    // only do prewalk to walkable tiles
    TilePtr toTile = g_map.getTile(m_localPlayer->getPos() + Position::getPosFromDirection(direction));
    if(toTile && toTile->isWalkable())
        m_localPlayer->preWalk(direction);
    else
        m_localPlayer->lockWalk();

    forceWalk(direction);
}

void Game::forceWalk(Otc::Direction direction)
{
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
    if(!isOnline())
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

void Game::look(const ThingPtr& thing)
{
    if(!isOnline() || !thing || !checkBotProtection())
        return;

    int stackpos = getThingStackpos(thing);
    if(stackpos != -1)
        m_protocolGame->sendLookAt(thing->getPos(), thing->getId(), stackpos);
}

void Game::open(const ThingPtr& thing, int containerId)
{
    if(!isOnline() || !thing || !checkBotProtection())
        return;

    int stackpos = getThingStackpos(thing);
    if(stackpos != -1)
        m_protocolGame->sendUseItem(thing->getPos(), thing->getId(), stackpos, containerId);
}

void Game::use(const ThingPtr& thing)
{
    if(!isOnline() || !thing || !checkBotProtection())
        return;

    m_localPlayer->lockWalk();

    int stackpos = getThingStackpos(thing);
    if(stackpos != -1)
        m_protocolGame->sendUseItem(thing->getPos(), thing->getId(), stackpos, 0);
}

void Game::useWith(const ThingPtr& fromThing, const ThingPtr& toThing)
{
    if(!isOnline() || !fromThing || !toThing || !checkBotProtection())
        return;

    Position pos = fromThing->getPos();
    int fromStackpos = getThingStackpos(fromThing);
    if(fromStackpos == -1)
        return;

    m_localPlayer->lockWalk();

    if(CreaturePtr creature = toThing->asCreature()) {
        m_protocolGame->sendUseOnCreature(pos, fromThing->getId(), fromStackpos, creature->getId());
    } else {
        int toStackpos = getThingStackpos(toThing);
        if(toStackpos == -1)
            return;

        m_protocolGame->sendUseItemEx(pos, fromThing->getId(), fromStackpos, toThing->getPos(), toThing->getId(), toStackpos);
    }
}

void Game::useInventoryItem(int itemId, const ThingPtr& toThing)
{
    if(!isOnline() || !toThing || !checkBotProtection())
        return;

    m_localPlayer->lockWalk();

    Position pos = Position(0xFFFF, 0, 0); // means that is a item in inventory
    int toStackpos = getThingStackpos(toThing);
    if(toStackpos == -1)
        return;

    if(CreaturePtr creature = toThing->asCreature()) {
        m_protocolGame->sendUseOnCreature(pos, itemId, 0, creature->getId());
    } else {
        m_protocolGame->sendUseItemEx(pos, itemId, 0, toThing->getPos(), toThing->getId(), toStackpos);
    }
}

void Game::move(const ThingPtr& thing, const Position& toPos, int count)
{
    if(!isOnline() || !thing || !checkBotProtection() || thing->getPos() == toPos || count <= 0)
        return;

    m_localPlayer->lockWalk();

    int stackpos = getThingStackpos(thing);
    if(stackpos == -1)
        return;

    m_protocolGame->sendThrow(thing->getPos(), thing->getId(), stackpos, toPos, count);
}

void Game::attack(const CreaturePtr& creature)
{
    if(!isOnline() || !creature || !checkBotProtection())
        return;

    m_localPlayer->lockWalk();

    if(m_localPlayer->isFollowing())
        cancelFollow();

    m_localPlayer->setAttackingCreature(creature);
    m_protocolGame->sendAttack(creature->getId());
}

void Game::cancelAttack()
{
    m_localPlayer->lockWalk();

    m_localPlayer->setAttackingCreature(nullptr);
    m_protocolGame->sendAttack(0);
}

void Game::follow(const CreaturePtr& creature)
{
    if(!isOnline() || !creature || !checkBotProtection())
        return;

    if(m_localPlayer->isAttacking())
        cancelAttack();

    m_localPlayer->setFollowingCreature(creature);
    m_protocolGame->sendFollow(creature->getId());
}

void Game::cancelFollow()
{
    m_localPlayer->setFollowingCreature(nullptr);
    m_protocolGame->sendFollow(0);
}

void Game::rotate(const ThingPtr& thing)
{
    if(!isOnline() || !thing || !checkBotProtection())
        return;

    int stackpos = getThingStackpos(thing);
    if(stackpos != -1)
        m_protocolGame->sendRotateItem(thing->getPos(), thing->getId(), stackpos);
}

//TODO: move this to Thing class
int Game::getThingStackpos(const ThingPtr& thing)
{
    // thing is at map
    if(thing->getPos().x != 65535) {
        TilePtr tile = g_map.getTile(thing->getPos());
        if(tile)
            return tile->getThingStackpos(thing);
        else {
            logError("could not get tile");
            return -1;
        }
    }

    // thing is at container or inventory
    return 0;
}

void Game::talk(const std::string& message)
{
    talkChannel("say", 0, message);
}

void Game::talkChannel(const std::string& speakTypeDesc, int channelId, const std::string& message)
{
    if(!isOnline() || !checkBotProtection())
        return;
    m_protocolGame->sendTalk(speakTypeDesc, channelId, "", message);
}

void Game::talkPrivate(const std::string& speakTypeDesc, const std::string& receiver, const std::string& message)
{
    if(!isOnline() || !checkBotProtection())
        return;
    m_protocolGame->sendTalk(speakTypeDesc, 0, receiver, message);
}

void Game::requestChannels()
{
    if(!isOnline() || !checkBotProtection())
        return;
    m_protocolGame->sendGetChannels();
}

void Game::joinChannel(int channelId)
{
    if(!isOnline() || !checkBotProtection())
        return;
    m_protocolGame->sendJoinChannel(channelId);
}

void Game::leaveChannel(int channelId)
{
    if(!isOnline() || !checkBotProtection())
        return;
    m_protocolGame->sendLeaveChannel(channelId);
}

void Game::closeNpcChannel()
{
    if(!isOnline() || !checkBotProtection())
        return;
    m_protocolGame->sendCloseNpcChannel();
}


void Game::partyInvite(int creatureId)
{
    if(!isOnline() || !checkBotProtection())
        return;
    m_protocolGame->sendInviteToParty(creatureId);
}

void Game::partyJoin(int creatureId)
{
    if(!isOnline() || !checkBotProtection())
        return;
    m_protocolGame->sendJoinParty(creatureId);
}

void Game::partyRevokeInvitation(int creatureId)
{
    if(!isOnline() || !checkBotProtection())
        return;
    m_protocolGame->sendRevokeInvitation(creatureId);
}


void Game::partyPassLeadership(int creatureId)
{
    if(!isOnline() || !checkBotProtection())
        return;
    m_protocolGame->sendPassLeadership(creatureId);
}

void Game::partyLeave()
{
    if(!isOnline() || !checkBotProtection())
        return;
    m_protocolGame->sendLeaveParty();
}

void Game::partyShareExperience(bool active)
{
    if(!isOnline() || !checkBotProtection())
        return;
    m_protocolGame->sendShareExperience(active, 0);
}

void Game::requestOutfit()
{
    if(!isOnline() || !checkBotProtection())
        return;
    m_protocolGame->sendGetOutfit();
}

void Game::setOutfit(const Outfit& outfit)
{
    if(!isOnline() || !checkBotProtection())
        return;
    m_protocolGame->sendSetOutfit(outfit);
}

void Game::addVip(const std::string& name)
{
    if(!isOnline() || name.empty() || !checkBotProtection())
        return;
    m_protocolGame->sendAddVip(name);
}

void Game::removeVip(int playerId)
{
    if(!isOnline() || !checkBotProtection())
        return;
    m_protocolGame->sendRemoveVip(playerId);
}

bool Game::checkBotProtection()
{
#ifndef NO_BOT_PROTECTION
    if(g_lua.isInCppCallback() && !g_ui.isOnInputEvent()) {
        logError("caught a lua call to a bot protected game function, the call was canceled");
        return false;
    }
#endif
    return true;
}
