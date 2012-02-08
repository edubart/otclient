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

Game::Game()
{
    resetGameStates();
}

void Game::resetGameStates()
{
    m_dead = false;
    m_serverBeat = 50;
    m_fightMode = Otc::FightBalanced;
    m_chaseMode = Otc::DontChase;
    m_safeFight = true;
    m_followingCreature = nullptr;
    m_attackingCreature = nullptr;
}

void Game::processConnectionError(const boost::system::error_code& error)
{
    // connection errors only have meaning if we still have a protocol
    if(m_protocolGame) {
        // eof = end of file, a clean disconnect
        if(error != asio::error::eof)
            g_lua.callGlobalField("g_game", "onConnectionError", error.message());

        processDisconnect();
    }
}

void Game::processDisconnect()
{
    if(isOnline()) {
        // only process logout event if the player is known
        if(m_localPlayer->isKnown())
            processLogout();

        m_localPlayer = nullptr;

        processGameEnd();
    }

    if(m_protocolGame) {
        m_protocolGame->disconnect();
        m_protocolGame = nullptr;
    }
}

void Game::processLoginError(const std::string& error)
{
    g_lua.callGlobalField("g_game", "onLoginError", error);
}

void Game::processLoginAdvice(const std::string& message)
{
    g_lua.callGlobalField("g_game", "onLoginAdvice", message);
}

void Game::processLoginWait(const std::string& message, int time)
{
    g_lua.callGlobalField("g_game", "onLoginWait", message, time);
}

void Game::processGameStart(const LocalPlayerPtr& localPlayer, int serverBeat)
{
    // reset the new game state
    resetGameStates();

    m_localPlayer = localPlayer;
    m_serverBeat = serverBeat;

    // synchronize fight modes with the server
    m_protocolGame->sendChangeFightModes(m_fightMode, m_chaseMode, m_safeFight);

    // NOTE: the entire map description and local player informations is not known yet
    g_lua.callGlobalField("g_game", "onGameStart");
}

void Game::processGameEnd()
{
    g_lua.callGlobalField("g_game", "onGameEnd");

    // reset game state
    resetGameStates();
}

void Game::processLogin()
{
    // the game could be offline if the login was canceled
    if(!isOnline())
        return;

    // by now the local player must be known
    g_lua.callGlobalField("g_game", "onLogin", m_localPlayer);
}

void Game::processLogout()
{
    g_lua.callGlobalField("g_game", "onLogout", m_localPlayer);
}

void Game::processDeath(int penality)
{
    m_dead = true;
    m_localPlayer->stopWalk();

    g_lua.callGlobalField("g_game","onDeath", penality);
}

void Game::processPing()
{
    m_protocolGame->sendPingResponse();
    g_lua.callGlobalField("g_game", "onPing");
}

void Game::processPlayerStats(double health, double maxHealth,
                              double freeCapacity, double experience,
                              double level, double levelPercent,
                              double mana, double maxMana,
                              double magicLevel, double magicLevelPercent,
                              double soul, double stamina)
{
    if(m_localPlayer->getHealth() != health ||
       m_localPlayer->getMaxHealth() != maxHealth) {
        m_localPlayer->setStatistic(Otc::Health, health);
        m_localPlayer->setStatistic(Otc::MaxHealth, maxHealth);
        g_lua.callGlobalField("g_game", "onHealthChange", health, maxHealth);

        // cannot walk while dying
        if(health == 0) {
            if(m_localPlayer->isPreWalking())
                m_localPlayer->stopWalk();
            m_localPlayer->lockWalk();
        }
    }

    if(m_localPlayer->getStatistic(Otc::FreeCapacity) != freeCapacity) {
        m_localPlayer->setStatistic(Otc::FreeCapacity, freeCapacity);
        g_lua.callGlobalField("g_game", "onFreeCapacityChange", freeCapacity);
    }

    if(m_localPlayer->getStatistic(Otc::Experience) != experience) {
        m_localPlayer->setStatistic(Otc::Experience, experience);
        g_lua.callGlobalField("g_game", "onExperienceChange", experience);
    }

    if(m_localPlayer->getStatistic(Otc::Level) != level ||
       m_localPlayer->getStatistic(Otc::LevelPercent) != levelPercent) {
        m_localPlayer->setStatistic(Otc::Level, level);
        m_localPlayer->setStatistic(Otc::LevelPercent, levelPercent);
        g_lua.callGlobalField("g_game", "onLevelChange", level, levelPercent);
    }

    if(m_localPlayer->getStatistic(Otc::Mana) != mana ||
       m_localPlayer->getStatistic(Otc::MaxMana) != maxMana) {
        m_localPlayer->setStatistic(Otc::Mana, mana);
        m_localPlayer->setStatistic(Otc::MaxMana, maxMana);
        g_lua.callGlobalField("g_game", "onManaChange", mana, maxMana);
    }

    if(m_localPlayer->getStatistic(Otc::MagicLevel) != magicLevel ||
       m_localPlayer->getStatistic(Otc::MagicLevelPercent) != magicLevelPercent) {
        m_localPlayer->setStatistic(Otc::MagicLevel, magicLevel);
        m_localPlayer->setStatistic(Otc::MagicLevelPercent, magicLevelPercent);
        g_lua.callGlobalField("g_game", "onMagicLevelChange", magicLevel, magicLevelPercent);
    }

    if(m_localPlayer->getStatistic(Otc::Soul) != soul) {
        m_localPlayer->setStatistic(Otc::Soul, soul);
        g_lua.callGlobalField("g_game", "onSoulChange", soul);
    }

    if(m_localPlayer->getStatistic(Otc::Stamina) != stamina) {
        m_localPlayer->setStatistic(Otc::Stamina, stamina);
        g_lua.callGlobalField("g_game", "onStaminaChange", stamina);
    }
}

void Game::processTextMessage(const std::string& type, const std::string& message)
{
    g_lua.callGlobalField("g_game","onTextMessage", type, message);
}

void Game::processCreatureSpeak(const std::string& name, int level, Otc::SpeakType type, const std::string& message, int channelId, const Position& creaturePos)
{
    if(creaturePos.isValid() && (type == Otc::SpeakSay || type == Otc::SpeakWhisper || type == Otc::SpeakYell || type == Otc::SpeakMonsterSay || type == Otc::SpeakMonsterYell)) {
        StaticTextPtr staticText = StaticTextPtr(new StaticText);
        staticText->addMessage(name, type, message);
        g_map.addThing(staticText, creaturePos);
    }

    g_lua.callGlobalField("g_game", "onCreatureSpeak", name, level, type, message, channelId, creaturePos);
}

void Game::processOpenContainer(int containerId, int itemId, const std::string& name, int capacity, bool hasParent, const std::vector< ItemPtr >& items)
{
    g_lua.callGlobalField("g_game", "onOpenContainer", containerId, itemId, name, capacity, hasParent, items);
}

void Game::processContainerAddItem(int containerId, const ItemPtr& item)
{
    item->setPosition(Position(65535, containerId + 0x40, 0));
    g_lua.callGlobalField("g_game", "onContainerAddItem", containerId, item);
}

void Game::processInventoryChange(int slot, const ItemPtr& item)
{
    if(item)
        item->setPosition(Position(65535, slot, 0));

    g_lua.callGlobalField("g_game","onInventoryChange", slot, item);
}

void Game::processCreatureMove(const CreaturePtr& creature, const Position& oldPos, const Position& newPos)
{
    // animate walk
    if(oldPos.isInRange(newPos, 1, 1))
        creature->walk(oldPos, newPos);
}

void Game::processCreatureTeleport(const CreaturePtr& creature)
{
    // stop walking on creature teleports
    creature->stopWalk();

    // locks the walk for a while when teleporting
    if(creature == m_localPlayer)
        m_localPlayer->lockWalk();
}

void Game::processAttackCancel()
{
    if(isAttacking())
        setAttackingCreature(nullptr);
}

void Game::processWalkCancel(Otc::Direction direction)
{
    m_localPlayer->cancelWalk(direction);
}

void Game::loginWorld(const std::string& account, const std::string& password, const std::string& worldHost, int worldPort, const std::string& characterName)
{
    m_protocolGame = ProtocolGamePtr(new ProtocolGame);
    m_protocolGame->login(account, password, worldHost, (uint16)worldPort, characterName);
}

void Game::cancelLogin()
{
    // send logout even if the game has not started yet, to make sure that the player doesn't stay logged there
    if(m_protocolGame)
        m_protocolGame->sendLogout();

    processDisconnect();
}

void Game::forceLogout()
{
    if(!isOnline())
        return;

    m_protocolGame->sendLogout();
    processDisconnect();
}

void Game::safeLogout()
{
    if(!isOnline())
        return;

    m_protocolGame->sendLogout();
}

void Game::walk(Otc::Direction direction)
{
    if(!canPerformGameAction())
        return;

    if(isFollowing())
        cancelFollow();

    if(!m_localPlayer->canWalk(direction))
        return;

    // only do prewalks to walkable tiles
    TilePtr toTile = g_map.getTile(m_localPlayer->getPosition().translatedToDirection(direction));
    if(toTile && toTile->isWalkable())
        m_localPlayer->preWalk(direction);
    else
        m_localPlayer->lockWalk();

    forceWalk(direction);
}

void Game::walkPath(const std::vector<Otc::Direction>& dir)
{
    if(!canPerformGameAction())
        return;

    if(isFollowing())
        cancelFollow();

    m_protocolGame->sendWalkPath(dir);
}

void Game::forceWalk(Otc::Direction direction)
{
    if(!canPerformGameAction())
        return;

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
    if(!canPerformGameAction())
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

void Game::stop()
{
    if(!canPerformGameAction())
        return;

    if(isFollowing())
        cancelFollow();

    m_protocolGame->sendStop();
}

void Game::look(const ThingPtr& thing)
{
    if(!canPerformGameAction() || !thing)
        return;

    m_protocolGame->sendLook(thing->getPosition(), thing->getId(), thing->getStackpos());
}

void Game::move(const ThingPtr& thing, const Position& toPos, int count)
{
    if(!canPerformGameAction() || !thing || thing->getPosition() == toPos || count <= 0)
        return;

    m_localPlayer->lockWalk();

    m_protocolGame->sendMove(thing->getPosition(), thing->getId(), thing->getStackpos(), toPos, count);
}

void Game::rotate(const ThingPtr& thing)
{
    if(!canPerformGameAction() || !thing)
        return;

    m_protocolGame->sendRotateItem(thing->getPosition(), thing->getId(), thing->getStackpos());
}

void Game::use(const ThingPtr& thing)
{
    if(!canPerformGameAction() || !thing)
        return;

    Position pos = thing->getPosition();
    if(!pos.isValid()) // virtual item
        pos = Position(0xFFFF, 0, 0); // means that is a item in inventory

    m_localPlayer->lockWalk();

    m_protocolGame->sendUseItem(pos, thing->getId(), thing->getStackpos(), 0);
}

void Game::useInventoryItem(int itemId)
{
    if(!canPerformGameAction() || !g_thingsType.isValidItemId(itemId))
        return;

    Position pos = Position(0xFFFF, 0, 0); // means that is a item in inventory
    m_localPlayer->lockWalk();

    m_protocolGame->sendUseItem(pos, itemId, 0, 0);
}

void Game::useWith(const ItemPtr& item, const ThingPtr& toThing)
{
    if(!canPerformGameAction() || !item || !toThing)
        return;

    Position pos = item->getPosition();
    if(!pos.isValid()) // virtual item
        pos = Position(0xFFFF, 0, 0); // means that is a item in inventory

    m_localPlayer->lockWalk();

    if(CreaturePtr creature = toThing->asCreature())
        m_protocolGame->sendUseOnCreature(pos, item->getId(), item->getStackpos(), creature->getId());
    else
        m_protocolGame->sendUseItemWith(pos, item->getId(), item->getStackpos(), toThing->getPosition(), toThing->getId(), toThing->getStackpos());
}

void Game::useInventoryItemWith(int itemId, const ThingPtr& toThing)
{
    if(!canPerformGameAction() || !toThing)
        return;

    m_localPlayer->lockWalk();

    Position pos = Position(0xFFFF, 0, 0); // means that is a item in inventory

    if(CreaturePtr creature = toThing->asCreature())
        m_protocolGame->sendUseOnCreature(pos, itemId, 0, creature->getId());
    else
        m_protocolGame->sendUseItemWith(pos, itemId, 0, toThing->getPosition(), toThing->getId(), toThing->getStackpos());
}

void Game::open(const ItemPtr& item, int containerId)
{
    if(!canPerformGameAction() || !item)
        return;

    m_protocolGame->sendUseItem(item->getPosition(), item->getId(), item->getStackpos(), containerId);
}

void Game::upContainer(int containerId)
{
    if(!canPerformGameAction())
        return;
    m_protocolGame->sendUpContainer(containerId);
}

void Game::refreshContainer()
{
    if(!canPerformGameAction())
        return;
    m_protocolGame->sendRefreshContainer();
}

void Game::attack(const CreaturePtr& creature)
{
    if(!canPerformGameAction() || creature == m_localPlayer || creature == m_attackingCreature)
        return;

    m_localPlayer->lockWalk();

    if(creature && isFollowing())
        cancelFollow();

    setAttackingCreature(creature);
    m_protocolGame->sendAttack(creature ? creature->getId() : 0);
}

void Game::follow(const CreaturePtr& creature)
{
    if(!canPerformGameAction() || creature == m_localPlayer || creature == m_followingCreature)
        return;

    m_localPlayer->lockWalk();

    if(creature && isAttacking())
        cancelAttack();

    setFollowingCreature(creature);
    m_protocolGame->sendFollow(creature ? creature->getId() : 0);
}

void Game::cancelAttackAndFollow()
{
    if(!canPerformGameAction())
        return;

    m_localPlayer->lockWalk();

    setAttackingCreature(nullptr);
    setFollowingCreature(nullptr);
    m_protocolGame->sendCancelAttackAndFollow();
}

void Game::talk(const std::string& message)
{
    if(!canPerformGameAction() || message.empty())
        return;
    talkChannel(Otc::SpeakSay, 0, message);
}

void Game::talkChannel(Otc::SpeakType speakType, int channelId, const std::string& message)
{
    if(!canPerformGameAction() || message.empty())
        return;
    m_protocolGame->sendTalk(speakType, channelId, "", message);
}

void Game::talkPrivate(Otc::SpeakType speakType, const std::string& receiver, const std::string& message)
{
    if(!canPerformGameAction() || receiver.empty() || message.empty())
        return;
    m_protocolGame->sendTalk(speakType, 0, receiver, message);
}

void Game::openPrivateChannel(const std::string& receiver)
{
    if(!canPerformGameAction() || receiver.empty())
        return;
    m_protocolGame->sendOpenPrivateChannel(receiver);
}

void Game::requestChannels()
{
    if(!canPerformGameAction())
        return;
    m_protocolGame->sendRequestChannels();
}

void Game::joinChannel(int channelId)
{
    if(!canPerformGameAction())
        return;
    m_protocolGame->sendJoinChannel(channelId);
}

void Game::leaveChannel(int channelId)
{
    if(!canPerformGameAction())
        return;
    m_protocolGame->sendLeaveChannel(channelId);
}

void Game::closeNpcChannel()
{
    if(!canPerformGameAction())
        return;
    m_protocolGame->sendCloseNpcChannel();
}

void Game::openOwnChannel()
{
    if(!canPerformGameAction())
        return;
    m_protocolGame->sendOpenOwnChannel();
}

void Game::inviteToOwnChannel(const std::string& name)
{
    if(!canPerformGameAction() || name.empty())
        return;
    m_protocolGame->sendInviteToOwnChannel(name);
}

void Game::excludeFromOwnChannel(const std::string& name)
{
    if(!canPerformGameAction() || name.empty())
        return;
    m_protocolGame->sendExcludeFromOwnChannel(name);
}

void Game::partyInvite(int creatureId)
{
    if(!canPerformGameAction())
        return;
    m_protocolGame->sendInviteToParty(creatureId);
}

void Game::partyJoin(int creatureId)
{
    if(!canPerformGameAction())
        return;
    m_protocolGame->sendJoinParty(creatureId);
}

void Game::partyRevokeInvitation(int creatureId)
{
    if(!canPerformGameAction())
        return;
    m_protocolGame->sendRevokeInvitation(creatureId);
}

void Game::partyPassLeadership(int creatureId)
{
    if(!canPerformGameAction())
        return;
    m_protocolGame->sendPassLeadership(creatureId);
}

void Game::partyLeave()
{
    if(!canPerformGameAction())
        return;
    m_protocolGame->sendLeaveParty();
}

void Game::partyShareExperience(bool active)
{
    if(!canPerformGameAction())
        return;
    m_protocolGame->sendShareExperience(active, 0);
}

void Game::requestOutfit()
{
    if(!canPerformGameAction())
        return;
    m_protocolGame->sendRequestOutfit();
}

void Game::changeOutfit(const Outfit& outfit)
{
    if(!canPerformGameAction())
        return;
    m_protocolGame->sendChangeOutfit(outfit);
}

void Game::addVip(const std::string& name)
{
    if(!canPerformGameAction() || name.empty())
        return;
    m_protocolGame->sendAddVip(name);
}

void Game::removeVip(int playerId)
{
    if(!isOnline() || !checkBotProtection())
        return;
    m_protocolGame->sendRemoveVip(playerId);
}

void Game::setChaseMode(Otc::ChaseModes chaseMode)
{
    if(!canPerformGameAction())
        return;

    m_chaseMode = chaseMode;
    m_protocolGame->sendChangeFightModes(m_fightMode, m_chaseMode, m_safeFight);
}

void Game::setFightMode(Otc::FightModes fightMode)
{
    if(!canPerformGameAction())
        return;

    m_fightMode = fightMode;
    m_protocolGame->sendChangeFightModes(m_fightMode, m_chaseMode, m_safeFight);
}

void Game::setSafeFight(bool on)
{
    if(!canPerformGameAction())
        return;

    m_safeFight = on;
    m_protocolGame->sendChangeFightModes(m_fightMode, m_chaseMode, m_safeFight);
}

bool Game::checkBotProtection()
{
#ifdef BOT_PROTECTION
    // accepts calls comming from a stacktrace containing only C++ functions,
    // if the stacktrace contains a lua function, then only accept if the engine is processing an input event
    if(g_lua.isInCppCallback() && !g_ui.isOnInputEvent()) {
        logError("caught a lua call to a bot protected game function, the call was canceled");
        return false;
    }
#endif
    return true;
}

bool Game::canPerformGameAction()
{
    // we can only perform game actions if we meet these conditions:
    // - the local player exists
    // - the local player is known
    // - the local player is not dead
    // - we have a game protocol
    // - the game protocol is connected
    // - its not a bot action
    return m_localPlayer && m_localPlayer->isKnown() && !m_dead && m_protocolGame && m_protocolGame->isConnected() && checkBotProtection();
}

void Game::setAttackingCreature(const CreaturePtr& creature)
{
    if(m_attackingCreature) {
        m_attackingCreature->hideStaticSquare();
        m_attackingCreature = nullptr;
    }

    if(creature) {
        creature->showStaticSquare(Fw::red);
        m_attackingCreature = creature;
    }

    g_lua.callGlobalField("g_game", "onAttackingCreatureChange", creature);
}

void Game::setFollowingCreature(const CreaturePtr& creature)
{
    if(m_followingCreature) {
        m_followingCreature->hideStaticSquare();
        m_followingCreature = nullptr;
    }

    if(creature) {
        creature->showStaticSquare(Fw::green);
        m_followingCreature = creature;
    }

    g_lua.callGlobalField("g_game", "onFollowingCreatureChange", creature);
}
