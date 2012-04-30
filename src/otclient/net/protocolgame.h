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

#ifndef PROTOCOLGAME_H
#define PROTOCOLGAME_H

#include "declarations.h"
#include <otclient/core/declarations.h>
#include <framework/net/protocol.h>
#include <otclient/core/creature.h>

class ProtocolGame : public Protocol
{
public:
    ProtocolGame();

public:
    void login(const std::string& accountName, const std::string& accountPassword, const std::string& host, uint16 port, const std::string& characterName);

    void onConnect();
    void onRecv(InputMessage& inputMessage);
    void onError(const boost::system::error_code& error);

    void sendLogout();
    void sendPingResponse();
    void sendAutoWalk(const std::vector<Otc::Direction>& path);
    void sendWalkNorth();
    void sendWalkEast();
    void sendWalkSouth();
    void sendWalkWest();
    void sendStop();
    void sendWalkNorthEast();
    void sendWalkSouthEast();
    void sendWalkSouthWest();
    void sendWalkNorthWest();
    void sendTurnNorth();
    void sendTurnEast();
    void sendTurnSouth();
    void sendTurnWest();
    void sendMove(const Position& fromPos, int itemId, int stackpos, const Position& toPos, int count);
    void sendInspectNpcTrade(int itemId, int count);
    void sendBuyItem(int itemId, int subType, int amount, bool ignoreCapacity, bool buyWithBackpack);
    void sendSellItem(int itemId, int subType, int amount, bool ignoreEquipped);
    void sendCloseNpcTrade();
    void sendRequestTrade(const Position& pos, int thingId, int stackpos, uint playerId);
    void sendInspectTrade(bool counterOffer, int index);
    void sendAcceptTrade();
    void sendRejectTrade();
    void sendUseItem(const Position& position, int itemId, int stackpos, int index);
    void sendUseItemWith(const Position& fromPos, int itemId, int fromStackpos, const Position& toPos, int toThingId, int toStackpos);
    void sendUseOnCreature(const Position& pos, int thingId, int stackpos, uint creatureId);
    void sendRotateItem(const Position& pos, int thingId, int stackpos);
    void sendCloseContainer(int containerId);
    void sendUpContainer(int containerId);
    void sendEditText(uint id, const std::string& text);
    void sendEditList(int listId, uint id, const std::string& text);
    void sendLook(const Position& position, int thingId, int stackpos);
    void sendTalk(Otc::SpeakType speakType, int channelId, const std::string& receiver, const std::string& message);
    void sendRequestChannels();
    void sendJoinChannel(int channelId);
    void sendLeaveChannel(int channelId);
    void sendOpenPrivateChannel(const std::string& receiver);
    void sendCloseNpcChannel();
    void sendChangeFightModes(Otc::FightModes fightMode, Otc::ChaseModes chaseMode, bool safeFight);
    void sendAttack(uint creatureId);
    void sendFollow(uint creatureId);
    void sendInviteToParty(uint creatureId);
    void sendJoinParty(uint creatureId);
    void sendRevokeInvitation(uint creatureId);
    void sendPassLeadership(uint creatureId);
    void sendLeaveParty();
    void sendShareExperience(bool active, int unknown);
    void sendOpenOwnChannel();
    void sendInviteToOwnChannel(const std::string& name);
    void sendExcludeFromOwnChannel(const std::string& name);
    void sendCancelAttackAndFollow();
    void sendRefreshContainer();
    void sendRequestOutfit();
    void sendChangeOutfit(const Outfit& outfit);
    void sendAddVip(const std::string& name);
    void sendRemoveVip(uint playerId);
    void sendBugReport(const std::string& comment);
    void sendRuleVilation(const std::string& target, int reason, int action, const std::string& comment, const std::string& statement, int statementId, bool ipBanishment);
    void sendDebugReport(const std::string& a, const std::string& b, const std::string& c, const std::string& d);
    void sendRequestQuestLog();
    void sendRequestQuestLine(int questId);

private:
    void sendLoginPacket(uint timestamp, uint8 unknown);

    // Parse Messages
    void parseMessage(InputMessage& msg);

    void parseInitGame(InputMessage& msg);
    void parseGMActions(InputMessage& msg);
    void parseLoginError(InputMessage& msg);
    void parseLoginAdvice(InputMessage& msg);
    void parseLoginWait(InputMessage& msg);
    void parsePing(InputMessage&);
    void parseDeath(InputMessage& msg);
    void parseMapDescription(InputMessage& msg);
    void parseMapMoveNorth(InputMessage& msg);
    void parseMapMoveEast(InputMessage& msg);
    void parseMapMoveSouth(InputMessage& msg);
    void parseMapMoveWest(InputMessage& msg);
    void parseUpdateTile(InputMessage& msg);
    void parseTileAddThing(InputMessage& msg);
    void parseTileTransformThing(InputMessage& msg);
    void parseTileRemoveThing(InputMessage& msg);
    void parseCreatureMove(InputMessage& msg);
    void parseOpenContainer(InputMessage& msg);
    void parseCloseContainer(InputMessage& msg);
    void parseContainerAddItem(InputMessage& msg);
    void parseContainerUpdateItem(InputMessage& msg);
    void parseContainerRemoveItem(InputMessage& msg);
    void parseAddInventoryItem(InputMessage& msg);
    void parseRemoveInventoryItem(InputMessage& msg);
    void parseOpenNpcTrade(InputMessage& msg);
    void parsePlayerGoods(InputMessage& msg);
    void parseCloseNpcTrade(InputMessage&);
    void parseWorldLight(InputMessage& msg);
    void parseMagicEffect(InputMessage& msg);
    void parseAnimatedText(InputMessage& msg);
    void parseDistanceMissile(InputMessage& msg);
    void parseCreatureMark(InputMessage& msg);
    void parseCreatureHealth(InputMessage& msg);
    void parseCreatureLight(InputMessage& msg);
    void parseCreatureOutfit(InputMessage& msg);
    void parseCreatureSpeed(InputMessage& msg);
    void parseCreatureSkulls(InputMessage& msg);
    void parseCreatureShields(InputMessage& msg);
    void parseCreatureUnpass(InputMessage& msg);
    void parseCreatureTurn(InputMessage& msg);
    void parseEditText(InputMessage& msg);
    void parseEditList(InputMessage& msg);
    void parsePlayerStats(InputMessage& msg);
    void parsePlayerSkills(InputMessage& msg);
    void parsePlayerState(InputMessage& msg);
    void parsePlayerCancelAttack(InputMessage& msg);
    void parseSpellDelay(InputMessage& msg);
    void parseSpellGroupDelay(InputMessage& msg);
    void parseCreatureSpeak(InputMessage& msg);
    void parseChannelList(InputMessage& msg);
    void parseOpenChannel(InputMessage& msg);
    void parseOpenPrivateChannel(InputMessage& msg);
    void parseOpenOwnPrivateChannel(InputMessage& msg);
    void parseCloseChannel(InputMessage& msg);
    void parseRuleViolationChannel(InputMessage& msg);
    void parseRuleViolationRemove(InputMessage& msg);
    void parseRuleViolationCancel(InputMessage& msg);
    void parseRuleViolationLock(InputMessage& msg);
    void parseOpenTrade(InputMessage& msg);
    void parseCloseTrade(InputMessage&);
    void parseTextMessage(InputMessage& msg);
    void parseCancelWalk(InputMessage& msg);
    void parseFloorChangeUp(InputMessage& msg);
    void parseFloorChangeDown(InputMessage& msg);
    void parseOpenOutfitWindow(InputMessage& msg);
    void parseVipAdd(InputMessage& msg);
    void parseVipLogin(InputMessage& msg);
    void parseVipLogout(InputMessage& msg);
    void parseTutorialHint(InputMessage& msg);
    void parseAutomapFlag(InputMessage& msg);
    void parseQuestLog(InputMessage& msg);
    void parseQuestLine(InputMessage& msg);

    void setMapDescription(InputMessage& msg, int x, int y, int z, int width, int height);
    void setFloorDescription(InputMessage& msg, int x, int y, int z, int width, int height, int offset, int* skipTiles);
    void setTileDescription(InputMessage& msg, Position position);

    Outfit internalGetOutfit(InputMessage& msg);
    ThingPtr internalGetThing(InputMessage& msg);
    ItemPtr internalGetItem(InputMessage& msg, int id = 0);

    void addPosition(OutputMessage& msg, const Position& position);
    Position parsePosition(InputMessage& msg);

private:
    bool m_waitingLoginPacket;
    std::string m_accountName;
    std::string m_accountPassword;
    std::string m_characterName;
    LocalPlayerPtr m_localPlayer;
};

#endif
