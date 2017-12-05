/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
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
#include "protocolcodes.h"
#include <framework/net/protocol.h>
#include "creature.h"

class ProtocolGame : public Protocol
{
public:
    void login(const std::string& accountName, const std::string& accountPassword, const std::string& host, uint16 port, const std::string& characterName, const std::string& authenticatorToken, const std::string& sessionKey);
    void send(const OutputMessagePtr& outputMessage);

    void sendExtendedOpcode(uint8 opcode, const std::string& buffer);
    void sendLoginPacket(uint challengeTimestamp, uint8 challengeRandom);
    void sendEnterGame();
    void sendLogout();
    void sendPing();
    void sendPingBack();
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
    void sendEquipItem(int itemId, int countOrSubType);
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
    void sendUseItemWith(const Position& fromPos, int itemId, int fromStackPos, const Position& toPos, int toThingId, int toStackPos);
    void sendUseOnCreature(const Position& pos, int thingId, int stackpos, uint creatureId);
    void sendRotateItem(const Position& pos, int thingId, int stackpos);
    void sendCloseContainer(int containerId);
    void sendUpContainer(int containerId);
    void sendEditText(uint id, const std::string& text);
    void sendEditList(uint id, int doorId, const std::string& text);
    void sendLook(const Position& position, int thingId, int stackpos);
    void sendLookCreature(uint creatureId);
    void sendTalk(Otc::MessageMode mode, int channelId, const std::string& receiver, const std::string& message);
    void sendRequestChannels();
    void sendJoinChannel(int channelId);
    void sendLeaveChannel(int channelId);
    void sendOpenPrivateChannel(const std::string& receiver);
    void sendOpenRuleViolation(const std::string& reporter);
    void sendCloseRuleViolation(const std::string& reporter);
    void sendCancelRuleViolation();
    void sendCloseNpcChannel();
    void sendChangeFightModes(Otc::FightModes fightMode, Otc::ChaseModes chaseMode, bool safeFight, Otc::PVPModes pvpMode);
    void sendAttack(uint creatureId, uint seq);
    void sendFollow(uint creatureId, uint seq);
    void sendInviteToParty(uint creatureId);
    void sendJoinParty(uint creatureId);
    void sendRevokeInvitation(uint creatureId);
    void sendPassLeadership(uint creatureId);
    void sendLeaveParty();
    void sendShareExperience(bool active);
    void sendOpenOwnChannel();
    void sendInviteToOwnChannel(const std::string& name);
    void sendExcludeFromOwnChannel(const std::string& name);
    void sendCancelAttackAndFollow();
    void sendRefreshContainer(int containerId);
    void sendRequestOutfit();
    void sendChangeOutfit(const Outfit& outfit);
    void sendMountStatus(bool mount);
    void sendAddVip(const std::string& name);
    void sendRemoveVip(uint playerId);
    void sendEditVip(uint playerId, const std::string& description, int iconId, bool notifyLogin);
    void sendBugReport(const std::string& comment);
    void sendRuleViolation(const std::string& target, int reason, int action, const std::string& comment, const std::string& statement, int statementId, bool ipBanishment);
    void sendDebugReport(const std::string& a, const std::string& b, const std::string& c, const std::string& d);
    void sendRequestQuestLog();
    void sendRequestQuestLine(int questId);
    void sendNewNewRuleViolation(int reason, int action, const std::string& characterName, const std::string& comment, const std::string& translation);
    void sendRequestItemInfo(int itemId, int subType, int index);
    void sendAnswerModalDialog(uint32 dialog, int button, int choice);
    void sendBrowseField(const Position& position);
    void sendSeekInContainer(int cid, int index);
    void sendBuyStoreOffer(int offerId, int productType, const std::string& name);
    void sendRequestTransactionHistory(int page, int entriesPerPage);
    void sendRequestStoreOffers(const std::string& categoryName, int serviceType);
    void sendOpenStore(int serviceType, const std::string &category);
    void sendTransferCoins(const std::string& recipient, int amount);
    void sendOpenTransactionHistory(int entiresPerPage);

    // otclient only
    void sendChangeMapAwareRange(int xrange, int yrange);

protected:
    void onConnect();
    void onRecv(const InputMessagePtr& inputMessage);
    void onError(const boost::system::error_code& error);

    friend class Game;

public:
    void addPosition(const OutputMessagePtr& msg, const Position& position);

private:
    void parseStoreButtonIndicators(const InputMessagePtr& msg);
    void parseSetStoreDeepLink(const InputMessagePtr& msg);
    void parseStore(const InputMessagePtr& msg);
    void parseStoreError(const InputMessagePtr& msg);
    void parseStoreTransactionHistory(const InputMessagePtr& msg);
    void parseStoreOffers(const InputMessagePtr& msg);
    void parseCompleteStorePurchase(const InputMessagePtr& msg);
    void parseRequestPurchaseData(const InputMessagePtr& msg);
    void parseCoinBalance(const InputMessagePtr& msg);
    void parseCoinBalanceUpdating(const InputMessagePtr& msg);
    void parseBlessings(const InputMessagePtr& msg);
    void parseUnjustifiedStats(const InputMessagePtr& msg);
    void parsePvpSituations(const InputMessagePtr& msg);
    void parsePreset(const InputMessagePtr& msg);
    void parseCreatureType(const InputMessagePtr& msg);
    void parsePlayerHelpers(const InputMessagePtr& msg);
    void parseMessage(const InputMessagePtr& msg);
    void parsePendingGame(const InputMessagePtr& msg);
    void parseEnterGame(const InputMessagePtr& msg);
    void parseLogin(const InputMessagePtr& msg);
    void parseGMActions(const InputMessagePtr& msg);
    void parseUpdateNeeded(const InputMessagePtr& msg);
    void parseLoginError(const InputMessagePtr& msg);
    void parseLoginAdvice(const InputMessagePtr& msg);
    void parseLoginWait(const InputMessagePtr& msg);
    void parseLoginToken(const InputMessagePtr& msg);
    void parsePing(const InputMessagePtr& msg);
    void parsePingBack(const InputMessagePtr& msg);
    void parseChallenge(const InputMessagePtr& msg);
    void parseDeath(const InputMessagePtr& msg);
    void parseMapDescription(const InputMessagePtr& msg);
    void parseMapMoveNorth(const InputMessagePtr& msg);
    void parseMapMoveEast(const InputMessagePtr& msg);
    void parseMapMoveSouth(const InputMessagePtr& msg);
    void parseMapMoveWest(const InputMessagePtr& msg);
    void parseUpdateTile(const InputMessagePtr& msg);
    void parseTileAddThing(const InputMessagePtr& msg);
    void parseTileTransformThing(const InputMessagePtr& msg);
    void parseTileRemoveThing(const InputMessagePtr& msg);
    void parseCreatureMove(const InputMessagePtr& msg);
    void parseOpenContainer(const InputMessagePtr& msg);
    void parseCloseContainer(const InputMessagePtr& msg);
    void parseContainerAddItem(const InputMessagePtr& msg);
    void parseContainerUpdateItem(const InputMessagePtr& msg);
    void parseContainerRemoveItem(const InputMessagePtr& msg);
    void parseAddInventoryItem(const InputMessagePtr& msg);
    void parseRemoveInventoryItem(const InputMessagePtr& msg);
    void parseOpenNpcTrade(const InputMessagePtr& msg);
    void parsePlayerGoods(const InputMessagePtr& msg);
    void parseCloseNpcTrade(const InputMessagePtr&);
    void parseWorldLight(const InputMessagePtr& msg);
    void parseMagicEffect(const InputMessagePtr& msg);
    void parseAnimatedText(const InputMessagePtr& msg);
    void parseDistanceMissile(const InputMessagePtr& msg);
    void parseCreatureMark(const InputMessagePtr& msg);
    void parseTrappers(const InputMessagePtr& msg);
    void parseCreatureHealth(const InputMessagePtr& msg);
    void parseCreatureLight(const InputMessagePtr& msg);
    void parseCreatureOutfit(const InputMessagePtr& msg);
    void parseCreatureSpeed(const InputMessagePtr& msg);
    void parseCreatureSkulls(const InputMessagePtr& msg);
    void parseCreatureShields(const InputMessagePtr& msg);
    void parseCreatureUnpass(const InputMessagePtr& msg);
    void parseEditText(const InputMessagePtr& msg);
    void parseEditList(const InputMessagePtr& msg);
    void parsePremiumTrigger(const InputMessagePtr& msg);
    void parsePlayerInfo(const InputMessagePtr& msg);
    void parsePlayerStats(const InputMessagePtr& msg);
    void parsePlayerSkills(const InputMessagePtr& msg);
    void parsePlayerState(const InputMessagePtr& msg);
    void parsePlayerCancelAttack(const InputMessagePtr& msg);
    void parsePlayerModes(const InputMessagePtr& msg);
    void parseSpellCooldown(const InputMessagePtr& msg);
    void parseSpellGroupCooldown(const InputMessagePtr& msg);
    void parseMultiUseCooldown(const InputMessagePtr& msg);
    void parseTalk(const InputMessagePtr& msg);
    void parseChannelList(const InputMessagePtr& msg);
    void parseOpenChannel(const InputMessagePtr& msg);
    void parseOpenPrivateChannel(const InputMessagePtr& msg);
    void parseOpenOwnPrivateChannel(const InputMessagePtr& msg);
    void parseCloseChannel(const InputMessagePtr& msg);
    void parseRuleViolationChannel(const InputMessagePtr& msg);
    void parseRuleViolationRemove(const InputMessagePtr& msg);
    void parseRuleViolationCancel(const InputMessagePtr& msg);
    void parseRuleViolationLock(const InputMessagePtr& msg);
    void parseOwnTrade(const InputMessagePtr& msg);
    void parseCounterTrade(const InputMessagePtr& msg);
    void parseCloseTrade(const InputMessagePtr&);
    void parseTextMessage(const InputMessagePtr& msg);
    void parseCancelWalk(const InputMessagePtr& msg);
    void parseWalkWait(const InputMessagePtr& msg);
    void parseFloorChangeUp(const InputMessagePtr& msg);
    void parseFloorChangeDown(const InputMessagePtr& msg);
    void parseOpenOutfitWindow(const InputMessagePtr& msg);
    void parseVipAdd(const InputMessagePtr& msg);
    void parseVipState(const InputMessagePtr& msg);
    void parseVipLogout(const InputMessagePtr& msg);
    void parseTutorialHint(const InputMessagePtr& msg);
    void parseAutomapFlag(const InputMessagePtr& msg);
    void parseQuestLog(const InputMessagePtr& msg);
    void parseQuestLine(const InputMessagePtr& msg);
    void parseChannelEvent(const InputMessagePtr& msg);
    void parseItemInfo(const InputMessagePtr& msg);
    void parsePlayerInventory(const InputMessagePtr& msg);
    void parseModalDialog(const InputMessagePtr& msg);
    void parseExtendedOpcode(const InputMessagePtr& msg);
    void parseChangeMapAwareRange(const InputMessagePtr& msg);
    void parseCreaturesMark(const InputMessagePtr& msg);

public:
    void setMapDescription(const InputMessagePtr& msg, int x, int y, int z, int width, int height);
    int setFloorDescription(const InputMessagePtr& msg, int x, int y, int z, int width, int height, int offset, int skip);
    int setTileDescription(const InputMessagePtr& msg, Position position);

    Outfit getOutfit(const InputMessagePtr& msg);
    ThingPtr getThing(const InputMessagePtr& msg);
    ThingPtr getMappedThing(const InputMessagePtr& msg);
    CreaturePtr getCreature(const InputMessagePtr& msg, int type = 0);
    StaticTextPtr getStaticText(const InputMessagePtr& msg, int type = 0);
    ItemPtr getItem(const InputMessagePtr& msg, int id = 0);
    Position getPosition(const InputMessagePtr& msg);

private:
    stdext::boolean<false> m_enableSendExtendedOpcode;
    stdext::boolean<false> m_gameInitialized;
    stdext::boolean<false> m_mapKnown;
    stdext::boolean<true> m_firstRecv;
    std::string m_accountName;
    std::string m_accountPassword;
    std::string m_authenticatorToken;
    std::string m_sessionKey;
    std::string m_characterName;
    LocalPlayerPtr m_localPlayer;
};

#endif
