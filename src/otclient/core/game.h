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

typedef std::tuple<std::string, bool> Vip;

class Game
{
public:
    Game();

private:
    void resetGameStates();

protected:
    void processConnectionError(const boost::system::error_code& error);
    void processDisconnect();
    void processPing();

    void processLoginError(const std::string& error);
    void processLoginAdvice(const std::string& message);
    void processLoginWait(const std::string& message, int time);

    void processLogin();
    void processLogout();

    void processGameStart(const LocalPlayerPtr& localPlayer, int serverBeat, bool canReportBugs);
    void processGameEnd();
    void processDeath(int penality);

    void processGMActions(const std::vector<uint8>& actions);
    void processPlayerStats(double health, double maxHealth,
                            double freeCapacity, double experience,
                            double level, double levelPercent,
                            double mana, double maxMana,
                            double magicLevel, double magicLevelPercent,
                            double soul, double stamina);

    void processInventoryChange(int slot, const ItemPtr& item);
    void processCreatureMove(const CreaturePtr& creature, const Position& oldPos, const Position& newPos);
    void processCreatureTeleport(const CreaturePtr& creature);
    void processAttackCancel();
    void processWalkCancel(Otc::Direction direction);

    // message related
    void processTextMessage(const std::string& type, const std::string& message);
    void processCreatureSpeak(const std::string& name, int level, Otc::SpeakType type, const std::string& message, int channelId, const Position& creaturePos);

    // container related
    void processOpenContainer(int containerId, int itemId, const std::string& name, int capacity, bool hasParent, const std::vector<ItemPtr>& items);
    void processCloseContainer(int containerId);
    void processContainerAddItem(int containerId, const ItemPtr& item);
    void processContainerUpdateItem(int containerId, int slot, const ItemPtr& item);
    void processContainerRemoveItem(int containerId, int slot);

    // channel related
    void processChannelList(const std::vector<std::tuple<int, std::string>>& channelList);
    void processOpenChannel(int channelId, const std::string& name);
    void processOpenPrivateChannel(const std::string& name);
    void processOpenOwnPrivateChannel(int channelId, const std::string& name);
    void processCloseChannel(int channelId);

    // rule violations
    void processRuleViolationChannel(int channelId);
    void processRuleViolationRemove(const std::string& name);
    void processRuleViolationCancel(const std::string& name);
    void processRuleViolationLock();

    // vip related
    void processVipAdd(uint id, const std::string& name, bool online);
    void processVipStateChange(uint id, bool online);

    // tutorial hint
    void processTutorialHint(int id);
    void processAutomapFlag(const Position& pos, int icon, const std::string& message);

    // outfit
    void processOpenOutfitWindow(const Outfit& currentOufit, const std::vector<std::tuple<int, std::string, int>>& outfitList);

    // npc trade
    void processOpenNpcTrade(const std::vector<std::tuple<ItemPtr, std::string, int, int, int>>& items);
    void processPlayerGoods(int money, const std::vector<std::tuple<ItemPtr, int>>& goods);
    void processCloseNpcTrade();

    // player trade
    void processOpenTrade(const std::string& name, const std::vector<ItemPtr>& items);
    void processCloseTrade();

    // edit text/list
    void processEditText(int id, int itemId, int maxLength, const std::string& text, const std::string& writter, const std::string& date);
    void processEditList(int listId, int id, const std::string& text);

    // questlog
    void processQuestLog(const std::vector<std::tuple<int, std::string, bool>>& questList);
    void processQuestLine(int questId, const std::vector<std::tuple<std::string, std::string>>& questMissions);

    friend class ProtocolGame;
    friend class Map;

public:
    // login related
    void loginWorld(const std::string& account,
                        const std::string& password,
                        const std::string& worldName,
                    const std::string& worldHost, int worldPort,
                    const std::string& characterName);
    void cancelLogin();
    void forceLogout();
    void safeLogout();

    // walk related
    void walk(Otc::Direction direction);
    void autoWalk(const std::vector<Otc::Direction>& dirs);
    void forceWalk(Otc::Direction direction);
    void turn(Otc::Direction direction);
    void stop();

    // item related
    void look(const ThingPtr& thing);
    void move(const ThingPtr &thing, const Position& toPos, int count);
    void moveToParentContainer(const ThingPtr& thing, int count);
    void rotate(const ThingPtr& thing);
    void use(const ThingPtr& thing);
    void useWith(const ItemPtr& fromThing, const ThingPtr& toThing);
    void useInventoryItem(int itemId);
    void useInventoryItemWith(int itemId, const ThingPtr& toThing);

    // container related
    void open(const ItemPtr& item, const ContainerPtr& previousContainer);
    void openParent(const ContainerPtr& container);
    void close(const ContainerPtr& container);
    void refreshContainer();

    // attack/follow related
    void attack(const CreaturePtr& creature);
    void cancelAttack() { attack(nullptr); }
    void follow(const CreaturePtr& creature);
    void cancelFollow() { follow(nullptr); }
    void cancelAttackAndFollow();

    // talk related
    void talk(const std::string& message);
    void talkChannel(Otc::SpeakType speakType, int channelId, const std::string& message);
    void talkPrivate(Otc::SpeakType speakType, const std::string& receiver, const std::string& message);

    // channel related
    void openPrivateChannel(const std::string& receiver);
    void requestChannels();
    void joinChannel(int channelId);
    void leaveChannel(int channelId);
    void closeNpcChannel();
    void openOwnChannel();
    void inviteToOwnChannel(const std::string& name);
    void excludeFromOwnChannel(const std::string& name);

    // party related
    void partyInvite(int creatureId);
    void partyJoin(int creatureId);
    void partyRevokeInvitation(int creatureId);
    void partyPassLeadership(int creatureId);
    void partyLeave();
    void partyShareExperience(bool active);

    // outfit related
    void requestOutfit();
    void changeOutfit(const Outfit& outfit);

    // vip related
    void addVip(const std::string& name);
    void removeVip(int playerId);

    // fight modes related
    void setChaseMode(Otc::ChaseModes chaseMode);
    void setFightMode(Otc::FightModes fightMode);
    void setSafeFight(bool on);
    Otc::ChaseModes getChaseMode() { return m_chaseMode; }
    Otc::FightModes getFightMode() { return m_fightMode; }
    bool isSafeFight() { return m_safeFight; }

    // npc trade related
    void inspectNpcTrade(const ItemPtr& item);
    void buyItem(const ItemPtr& item, int amount, bool ignoreCapacity, bool buyWithBackpack);
    void sellItem(const ItemPtr& item, int amount, bool ignoreEquipped);
    void closeNpcTrade();

    // player trade related
    void requestTrade(const ItemPtr& item, const CreaturePtr& creature);
    void inspectTrade(bool counterOffer, int index);
    void acceptTrade();
    void rejectTrade();

    // house window and editable items related
    void editText(uint id, const std::string& text);
    void editList(int listId, uint id, const std::string& text);

    // reports
    void reportBug(const std::string& comment);
    void reportRuleVilation(const std::string& target, int reason, int action, const std::string& comment, const std::string& statement, int statementId, bool ipBanishment);
    void debugReport(const std::string& a, const std::string& b, const std::string& c, const std::string& d);

    // questlog related
    void requestQuestLog();
    void requestQuestLine(int questId);

    bool canPerformGameAction();
    bool canReportBugs() { return m_canReportBugs; }
    bool checkBotProtection();

    bool isOnline() { return !!m_localPlayer; }
    bool isDead() { return m_dead; }
    bool isAttacking() { return !!m_attackingCreature; }
    bool isFollowing() { return !!m_followingCreature; }

    ContainerPtr getContainer(int index) { return m_containers[index]; }
    std::map<int, ContainerPtr> getContainers() { return m_containers; }
    std::map<int, Vip> getVips() { return m_vips; }
    CreaturePtr getAttackingCreature() { return m_attackingCreature; }
    CreaturePtr getFollowingCreature() { return m_followingCreature; }
    int getServerBeat() { return m_serverBeat; }
    LocalPlayerPtr getLocalPlayer() { return m_localPlayer; }
    ProtocolGamePtr getProtocolGame() { return m_protocolGame; }
    int getProtocolVersion() { return PROTOCOL; }
    std::string getWorldName() { return m_worldName; }
    std::vector<uint8> getGMActions() { return m_gmActions; }

private:
    void setAttackingCreature(const CreaturePtr& creature);
    void setFollowingCreature(const CreaturePtr& creature);

    LocalPlayerPtr m_localPlayer;
    CreaturePtr m_attackingCreature;
    CreaturePtr m_followingCreature;
    ProtocolGamePtr m_protocolGame;
    std::map<int, ContainerPtr> m_containers;
    std::map<int, Vip> m_vips;

    bool m_dead;
    int m_serverBeat;
    Otc::FightModes m_fightMode;
    Otc::ChaseModes m_chaseMode;
    bool m_safeFight;
    bool m_canReportBugs;
    std::vector<uint8> m_gmActions;
    std::string m_worldName;
};

extern Game g_game;

#endif
