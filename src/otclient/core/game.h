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

#ifndef GAME_H
#define GAME_H

#include "declarations.h"
#include <otclient/net/declarations.h>
#include <otclient/core/item.h>
#include <otclient/core/outfit.h>
#include <framework/core/timer.h>

class Game
{
public:
    void loginWorld(const std::string& account,
                    const std::string& password,
                    const std::string& worldHost, int worldPort,
                    const std::string& characterName);
    void cancelLogin();
    void logout(bool force);
    void forceLogout() { logout(true); }
    void cleanLogout() { logout(false); }
    void processLoginError(const std::string& error);
    void processConnectionError(const boost::system::error_code& error);
    void processLogin(const LocalPlayerPtr& localPlayer, int serverBeat);
    void processLogout();
    void processDeath();

    void processTextMessage(const std::string& type, const std::string& message);
    void processCreatureSpeak(const std::string& name, int level, const std::string& type, const std::string& message, int channelId, const Position& creaturePos);
    void processContainerAddItem(int containerId, const ItemPtr& item);
    void processInventoryChange(int slot, const ItemPtr& item);
    void processCreatureMove(const CreaturePtr& creature, const Position& oldPos, const Position& newPos);
    void processAttackCancel();
    void processWalkCancel(Otc::Direction direction);

    // walk related
    void walk(Otc::Direction direction);
    void forceWalk(Otc::Direction direction);
    void turn(Otc::Direction direction);

    // item related
    void look(const ThingPtr& thing);
    void open(const ThingPtr& thing, int containerId);
    void use(const ThingPtr& thing);
    void useWith(const ThingPtr& fromThing, const ThingPtr& toThing);
    void useHotkey(int itemId, const ThingPtr& toThing);

    // attack/follow related
    void attack(const CreaturePtr& creature);
    void cancelAttack();
    void follow(const CreaturePtr& creature);
    void cancelFollow();
    void rotate(const ThingPtr& thing);

    // talk related
    void talk(const std::string& message);
    void talkChannel(const std::string& speakTypeDesc, int channelId, const std::string& message);
    void talkPrivate(const std::string& speakTypeDesc, const std::string& receiver, const std::string& message);
    void requestChannels();
    void joinChannel(int channelId);
    void leaveChannel(int channelId);
    void closeNpcChannel();

    // party related
    void partyInvite(int creatureId);
    void partyJoin(int creatureId);
    void partyRevokeInvitation(int creatureId);
    void partyPassLeadership(int creatureId);
    void partyLeave();
    void partyShareExperience(bool active);

    // outfit related
    void requestOutfit();
    void setOutfit(const Outfit& outfit);

    // vip related
    void addVip(const std::string& name);
    void removeVip(int playerId);

    int getThingStackpos(const ThingPtr& thing);

    bool checkBotProtection();

    bool isOnline() { return m_online; }
    bool isDead() { return m_dead; }

    void setSelectedThing(const ThingPtr& thing) { m_selectedThing = thing; }
    ThingPtr getSelectedThing() { return m_selectedThing; }

    void setServerBeat(int serverBeat) { m_serverBeat = serverBeat; }
    int getServerBeat() { return m_serverBeat; }

    LocalPlayerPtr getLocalPlayer() { return m_localPlayer; }
    ProtocolGamePtr getProtocolGame() { return m_protocolGame; }
    int getProtocolVersion() { return PROTOCOL; }

private:
    LocalPlayerPtr m_localPlayer;
    ProtocolGamePtr m_protocolGame;
    bool m_online;
    bool m_dead;
    int m_serverBeat;
    ThingPtr m_selectedThing;
};

extern Game g_game;

#endif
