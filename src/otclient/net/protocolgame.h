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
    void sendPing();
    void sendWalkNorth();
    void sendWalkEast();
    void sendWalkSouth();
    void sendWalkWest();
    void sendStopAutowalk();
    void sendWalkNorthEast();
    void sendWalkSouthEast();
    void sendWalkSouthWest();
    void sendWalkNorthWest();
    void sendTurnNorth();
    void sendTurnEast();
    void sendTurnSouth();
    void sendTurnWest();
    void sendThrow(const Position& fromPos, int thingId, int stackpos, const Position& toPos, int count);
    void sendLookInShop(int thingId, int count);
    void sendPlayerPurchase(int thingId, int count, int amount, bool ignoreCapacity, bool buyWithBackpack);
    void sendPlayerSale(int thingId, int count, int amount, bool ignoreEquipped);
    void sendCloseShop();
    void sendRequestTrade(const Position& pos, int thingId, int stackpos, uint playerId);
    void sendLookInTrade(bool counterOffer, int index);
    void sendAcceptTrade();
    void sendRejectTrade();
    void sendUseItem(const Position& position, int itemId, int stackpos, int index);
    void sendUseItemEx(const Position& fromPos, int fromThingId, int fromStackpos, const Position& toPos, int toThingId, int toStackpos);
    void sendUseOnCreature(const Position& pos, int thingId, int stackpos, uint creatureId);
    void sendRotateItem(const Position& pos, int thingId, int stackpos);
    void sendCloseContainer(int containerId);
    void sendUpContainer(int containerId);
    void sendTextWindow(uint windowTextId, const std::string& text);
    void sendHouseWindow(int doorId, uint id, const std::string& text);
    void sendLookAt(const Position& position, int thingId, int stackpos);
    void sendTalk(const std::string& speakTypeDesc, int channelId, const std::string& receiver, const std::string& message);
    void sendGetChannels();
    void sendJoinChannel(int channelId);
    void sendLeaveChannel(int channelId);
    void sendPrivateChannel(const std::string& receiver);
    void sendCloseNpcChannel();
    void sendFightTatics(Otc::FightModes fightMode, Otc::ChaseModes chaseMode, bool safeFight);
    void sendAttack(uint creatureId);
    void sendFollow(uint creatureId);
    void sendInviteToParty(uint creatureId);
    void sendJoinParty(uint creatureId);
    void sendRevokeInvitation(uint creatureId);
    void sendPassLeadership(uint creatureId);
    void sendLeaveParty();
    void sendShareExperience(bool active, int unknown);
    void sendOpenChannel(int channelId);
    void sendInviteToChannel(const std::string& name);
    void sendExcludeFromChannel(const std::string& name);
    void sendCancel();
    void sendUpdateContainer();
    void sendGetOutfit();
    void sendSetOutfit(const Outfit& outfit);
    void sendAddVip(const std::string& name);
    void sendRemoveVip(uint playerId);
    void sendGetQuestLog();
    void sendGetQuestLine(int questId);

private:
    void sendLoginPacket(uint timestamp, uint8 unknown);

    // Parse Messages
    void parseMessage(InputMessage& msg);

    void parsePlayerLogin(InputMessage& msg);
    void parseGMActions(InputMessage& msg);
    void parseLoginError(InputMessage& msg);
    void parseFYIMessage(InputMessage& msg);
    void parseWaitList(InputMessage& msg);
    void parsePing(InputMessage&);
    void parseDeath(InputMessage& msg);
    void parseCanReportBugs(InputMessage& msg);
    void parseMapDescription(InputMessage& msg);
    void parseMoveNorth(InputMessage& msg);
    void parseMoveEast(InputMessage& msg);
    void parseMoveSouth(InputMessage& msg);
    void parseMoveWest(InputMessage& msg);
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
    void parseOpenShopWindow(InputMessage& msg);
    void parsePlayerCash(InputMessage& msg);
    void parseCloseShopWindow(InputMessage&);
    void parseWorldLight(InputMessage& msg);
    void parseMagicEffect(InputMessage& msg);
    void parseAnimatedText(InputMessage& msg);
    void parseDistanceMissile(InputMessage& msg);
    void parseCreatureSquare(InputMessage& msg);
    void parseCreatureHealth(InputMessage& msg);
    void parseCreatureLight(InputMessage& msg);
    void parseCreatureOutfit(InputMessage& msg);
    void parseCreatureSpeed(InputMessage& msg);
    void parseCreatureSkulls(InputMessage& msg);
    void parseCreatureShields(InputMessage& msg);
    void parseCreatureTurn(InputMessage& msg);
    void parseItemTextWindow(InputMessage& msg);
    void parseHouseTextWindow(InputMessage& msg);
    void parsePlayerStats(InputMessage& msg);
    void parsePlayerSkills(InputMessage& msg);
    void parsePlayerIcons(InputMessage& msg);
    void parsePlayerCancelAttack(InputMessage& msg);
    void parseCreatureSpeak(InputMessage& msg);
    void parseChannelList(InputMessage& msg);
    void parseOpenChannel(InputMessage& msg);
    void parseOpenPrivatePlayerChat(InputMessage& msg);
    void parseCreatePrivateChannel(InputMessage& msg);
    void parseClosePrivateChannel(InputMessage& msg);
    void parseSafeTradeRequest(InputMessage& msg);
    void parseSafeTradeClose(InputMessage&);
    void parseTextMessage(InputMessage& msg);
    void parseCancelWalk(InputMessage& msg);
    void parseFloorChangeUp(InputMessage& msg);
    void parseFloorChangeDown(InputMessage& msg);
    void parseOutfitWindow(InputMessage& msg);
    void parseVipState(InputMessage& msg);
    void parseVipLogin(InputMessage& msg);
    void parseVipLogout(InputMessage& msg);
    void parseShowTutorial(InputMessage& msg);
    void parseAddMarker(InputMessage& msg);
    void parseQuestList(InputMessage& msg);
    void parseQuestPartList(InputMessage& msg);

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
