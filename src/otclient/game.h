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
#include "item.h"
#include "outfit.h"
#include <framework/core/timer.h>

typedef std::tuple<std::string, bool> Vip;

//@bindsingleton g_game
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

    void processGameStart();
    void processGameEnd();
    void processDeath(int penality);

    void processGMActions(const std::vector<uint8>& actions);
    void processInventoryChange(int slot, const ItemPtr& item);
    void processCreatureMove(const CreaturePtr& creature, const Position& oldPos, const Position& newPos);
    void processCreatureTeleport(const CreaturePtr& creature);
    void processAttackCancel(uint seq);
    void processWalkCancel(Otc::Direction direction);

    // message related
    void processTextMessage(Otc::MessageMode mode, const std::string& text);
    void processTalk(const std::string& name, int level, Otc::MessageMode mode, const std::string& text, int channelId, const Position& pos);

    // container related
    void processOpenContainer(int containerId, const ItemPtr& containerItem, const std::string& name, int capacity, bool hasParent, const std::vector<ItemPtr>& items);
    void processCloseContainer(int containerId);
    void processContainerAddItem(int containerId, const ItemPtr& item);
    void processContainerUpdateItem(int containerId, int slot, const ItemPtr& item);
    void processContainerRemoveItem(int containerId, int slot);

    // channel related
    void processChannelList(const std::vector<std::tuple<int, std::string> >& channelList);
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
    void processOpenOutfitWindow(const Outfit& currentOufit, const std::vector<std::tuple<int, std::string, int> >& outfitList,
                                 const std::vector<std::tuple<int, std::string> >& mountList);

    // npc trade
    void processOpenNpcTrade(const std::vector<std::tuple<ItemPtr, std::string, int, int, int> >& items);
    void processPlayerGoods(int money, const std::vector<std::tuple<ItemPtr, int> >& goods);
    void processCloseNpcTrade();

    // player trade
    void processOwnTrade(const std::string& name, const std::vector<ItemPtr>& items);
    void processCounterTrade(const std::string& name, const std::vector<ItemPtr>& items);
    void processCloseTrade();

    // edit text/list
    void processEditText(uint id, int itemId, int maxLength, const std::string& text, const std::string& writter, const std::string& date);
    void processEditList(uint id, int doorId, const std::string& text);

    // questlog
    void processQuestLog(const std::vector<std::tuple<int, std::string, bool> >& questList);
    void processQuestLine(int questId, const std::vector<std::tuple<std::string, std::string> >& questMissions);

    friend class ProtocolGame;
    friend class Map;

public:
    // login related
    void loginWorld(const std::string& account, const std::string& password, const std::string& worldName, const std::string& worldHost, int worldPort, const std::string& characterName);
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
    void talkChannel(Otc::MessageMode mode, int channelId, const std::string& message);
    void talkPrivate(Otc::MessageMode mode, const std::string& receiver, const std::string& message);

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
    void editList(uint id, int doorId, const std::string& text);

    // reports
    void reportBug(const std::string& comment);
    void reportRuleVilation(const std::string& target, int reason, int action, const std::string& comment, const std::string& statement, int statementId, bool ipBanishment);
    void debugReport(const std::string& a, const std::string& b, const std::string& c, const std::string& d);

    // questlog related
    void requestQuestLog();
    void requestQuestLine(int questId);

    // 870 only
    void equipItem(const ItemPtr& item);
    void mount(bool mount);

    // 910 only
    //void requestItemInfo();
    //void reportRuleViolation2();
    // TODO: market related

    // dynamic support for game features
    void enableFeature(Otc::GameFeature feature) { m_features.set(feature, true); }
    void disableFeature(Otc::GameFeature feature) { m_features.set(feature, false); }
    void setFeature(Otc::GameFeature feature, bool enabled) { m_features.set(feature, enabled); }
    bool getFeature(Otc::GameFeature feature) { return m_features.test(feature); }

    void setClientVersion(int version);
    int getClientVersion() { return m_protocolVersion; }

    void setRSA(const std::string& rsa);
    std::string getRSA() { return m_rsa; }

    bool canPerformGameAction();
    bool checkBotProtection();

    bool isOnline() { return m_online; }
    bool isDead() { return m_dead; }
    bool isAttacking() { return !!m_attackingCreature; }
    bool isFollowing() { return !!m_followingCreature; }
    bool isMounted() { return m_mounted; }

    ContainerPtr getContainer(int index) { return m_containers[index]; }
    std::map<int, ContainerPtr> getContainers() { return m_containers; }
    std::map<int, Vip> getVips() { return m_vips; }
    CreaturePtr getAttackingCreature() { return m_attackingCreature; }
    CreaturePtr getFollowingCreature() { return m_followingCreature; }
    void setServerBeat(int beat) { m_serverBeat = beat; }
    int getServerBeat() { return m_serverBeat; }
    void setCanReportBugs(bool enable) { m_canReportBugs = enable; }
    bool canReportBugs() { return m_canReportBugs; }
    LocalPlayerPtr getLocalPlayer() { return m_localPlayer; }
    ProtocolGamePtr getProtocolGame() { return m_protocolGame; }
    std::string getCharacterName() { return m_characterName; }
    std::string getWorldName() { return m_worldName; }
    std::vector<uint8> getGMActions() { return m_gmActions; }

protected:
    void enableBotCall() { m_denyBotCall = false; }
    void disableBotCall() { m_denyBotCall = true; }

private:
    void setAttackingCreature(const CreaturePtr& creature);
    void setFollowingCreature(const CreaturePtr& creature);

    LocalPlayerPtr m_localPlayer;
    CreaturePtr m_attackingCreature;
    CreaturePtr m_followingCreature;
    ProtocolGamePtr m_protocolGame;
    std::map<int, ContainerPtr> m_containers;
    std::map<int, Vip> m_vips;

    bool m_online;
    bool m_denyBotCall;
    bool m_dead;
    bool m_mounted;
    int m_serverBeat;
    uint m_seq;
    Otc::FightModes m_fightMode;
    Otc::ChaseModes m_chaseMode;
    bool m_safeFight;
    bool m_canReportBugs;
    std::vector<uint8> m_gmActions;
    std::string m_characterName;
    std::string m_worldName;
    std::bitset<Otc::LastGameFeature> m_features;
    int m_protocolVersion;
    std::string m_rsa;
};

extern Game g_game;

#endif
