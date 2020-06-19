/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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

#include "protocolgame.h"

#include "localplayer.h"
#include "thingtypemanager.h"
#include "game.h"
#include "map.h"
#include "item.h"
#include "effect.h"
#include "missile.h"
#include "tile.h"
#include "luavaluecasts.h"
#include <framework/core/eventdispatcher.h>

void ProtocolGame::parseMessage(const InputMessagePtr &msg)
{
    int opcode = -1;
    int prevOpcode = -1;

    try
    {
        while (!msg->eof())
        {
            opcode = msg->getU8();

            // must be > so extended will be enabled before GameStart.
            if (!g_game.getFeature(Otc::GameLoginPending))
            {
                if (!m_gameInitialized && opcode > Proto::GameServerFirstGameOpcode)
                {
                    g_game.processGameStart();
                    m_gameInitialized = true;
                }
            }

            // try to parse in lua first
            int readPos = msg->getReadPos();
            if (callLuaField<bool>("onOpcode", opcode, msg))
                continue;
            else
                msg->setReadPos(readPos); // restore read pos

            switch (opcode)
            {
            case Proto::GameServerLoginOrPendingState:
                parsePendingGame(msg);
                break;
            case Proto::GameServerGMActions:
                parseGMActions(msg);
                break;
            case Proto::GameServerUpdateNeeded:
                parseUpdateNeeded(msg);
                break;
            case Proto::GameServerLoginError:
                parseLoginError(msg);
                break;
            case Proto::GameServerLoginAdvice:
                parseLoginAdvice(msg);
                break;
            case Proto::GameServerLoginWait:
                parseLoginWait(msg);
                break;
            case Proto::GameServerLoginToken:
                parseLoginToken(msg);
                break;
            case Proto::GameServerPing:
            case Proto::GameServerPingBack:
                if ((opcode == Proto::GameServerPing && g_game.getFeature(Otc::GameClientPing)) ||
                    (opcode == Proto::GameServerPingBack && !g_game.getFeature(Otc::GameClientPing)))
                    parsePingBack(msg);
                else
                    parsePing(msg);
                break;
            case Proto::GameServerChallenge:
                parseChallenge(msg);
                break;
            case Proto::GameServerDeath:
                parseDeath(msg);
                break;
            case Proto::GameServerFullMap:
                parseMapDescription(msg);
                break;
            case Proto::GameServerMapTopRow:
                parseMapMoveNorth(msg);
                break;
            case Proto::GameServerMapRightRow:
                parseMapMoveEast(msg);
                break;
            case Proto::GameServerMapBottomRow:
                parseMapMoveSouth(msg);
                break;
            case Proto::GameServerMapLeftRow:
                parseMapMoveWest(msg);
                break;
            case Proto::GameServerUpdateTile:
                parseUpdateTile(msg);
                break;
            case Proto::GameServerCreateOnMap:
                parseTileAddThing(msg);
                break;
            case Proto::GameServerChangeOnMap:
                parseTileTransformThing(msg);
                break;
            case Proto::GameServerDeleteOnMap:
                parseTileRemoveThing(msg);
                break;
            case Proto::GameServerMoveCreature:
                parseCreatureMove(msg);
                break;
            case Proto::GameServerOpenContainer:
                parseOpenContainer(msg);
                break;
            case Proto::GameServerCloseContainer:
                parseCloseContainer(msg);
                break;
            case Proto::GameServerCreateContainer:
                parseContainerAddItem(msg);
                break;
            case Proto::GameServerChangeInContainer:
                parseContainerUpdateItem(msg);
                break;
            case Proto::GameServerDeleteInContainer:
                parseContainerRemoveItem(msg);
                break;
            case Proto::GameServerSetInventory:
                parseAddInventoryItem(msg);
                break;
            case Proto::GameServerDeleteInventory:
                parseRemoveInventoryItem(msg);
                break;
            case Proto::GameServerOpenNpcTrade:
                parseOpenNpcTrade(msg);
                break;
            case Proto::GameServerPlayerGoods:
                parsePlayerGoods(msg);
                break;
            case Proto::GameServerCloseNpcTrade:
                parseCloseNpcTrade(msg);
                break;
            case Proto::GameServerOwnTrade:
                parseOwnTrade(msg);
                break;
            case Proto::GameServerCounterTrade:
                parseCounterTrade(msg);
                break;
            case Proto::GameServerCloseTrade:
                parseCloseTrade(msg);
                break;
            case Proto::GameServerAmbient:
                parseWorldLight(msg);
                break;
            case Proto::GameServerGraphicalEffect:
                parseMagicEffect(msg);
                break;
            case Proto::GameServerTextEffect:
                parseAnimatedText(msg);
                break;
            case Proto::GameServerMissleEffect:
                parseDistanceMissile(msg);
                break;
            case Proto::GameServerMarkCreature:
                parseCreatureMark(msg);
                break;
            case Proto::GameServerTrappers:
                parseTrappers(msg);
                break;
            case Proto::GameServerCreatureHealth:
                parseCreatureHealth(msg);
                break;
            case Proto::GameServerCreatureLight:
                parseCreatureLight(msg);
                break;
            case Proto::GameServerCreatureOutfit:
                parseCreatureOutfit(msg);
                break;
            case Proto::GameServerCreatureSpeed:
                parseCreatureSpeed(msg);
                break;
            case Proto::GameServerCreatureSkull:
                parseCreatureSkulls(msg);
                break;
            case Proto::GameServerCreatureParty:
                parseCreatureShields(msg);
                break;
            case Proto::GameServerCreatureUnpass:
                parseCreatureUnpass(msg);
                break;
            case Proto::GameServerEditText:
                parseEditText(msg);
                break;
            case Proto::GameServerEditList:
                parseEditList(msg);
                break;
            // PROTOCOL>=1038
            case Proto::GameServerPremiumTrigger:
                parsePremiumTrigger(msg);
                break;
            case Proto::GameServerPlayerData:
                parsePlayerStats(msg);
                break;
            case Proto::GameServerPlayerSkills:
                parsePlayerSkills(msg);
                break;
            case Proto::GameServerPlayerState:
                parsePlayerState(msg);
                break;
            case Proto::GameServerClearTarget:
                parsePlayerCancelAttack(msg);
                break;
            case Proto::GameServerPlayerModes:
                parsePlayerModes(msg);
                break;
            case Proto::GameServerTalk:
                parseTalk(msg);
                break;
            case Proto::GameServerChannels:
                parseChannelList(msg);
                break;
            case Proto::GameServerOpenChannel:
                parseOpenChannel(msg);
                break;
            case Proto::GameServerOpenPrivateChannel:
                parseOpenPrivateChannel(msg);
                break;
            case Proto::GameServerRuleViolationChannel:
                parseRuleViolationChannel(msg);
                break;
            case Proto::GameServerRuleViolationRemove:
                parseRuleViolationRemove(msg);
                break;
            case Proto::GameServerRuleViolationCancel:
                parseRuleViolationCancel(msg);
                break;
            case Proto::GameServerRuleViolationLock:
                parseRuleViolationLock(msg);
                break;
            case Proto::GameServerOpenOwnChannel:
                parseOpenOwnPrivateChannel(msg);
                break;
            case Proto::GameServerCloseChannel:
                parseCloseChannel(msg);
                break;
            case Proto::GameServerTextMessage:
                parseTextMessage(msg);
                break;
            case Proto::GameServerCancelWalk:
                parseCancelWalk(msg);
                break;
            case Proto::GameServerWalkWait:
                parseWalkWait(msg);
                break;
            case Proto::GameServerFloorChangeUp:
                parseFloorChangeUp(msg);
                break;
            case Proto::GameServerFloorChangeDown:
                parseFloorChangeDown(msg);
                break;
            case Proto::GameServerChooseOutfit:
                parseOpenOutfitWindow(msg);
                break;
            case Proto::GameServerVipAdd:
                parseVipAdd(msg);
                break;
            case Proto::GameServerVipState:
                parseVipState(msg);
                break;
            case Proto::GameServerVipLogout:
                parseVipLogout(msg);
                break;
            case Proto::GameServerTutorialHint:
                parseTutorialHint(msg);
                break;
            case Proto::GameServerAutomapFlag:
                parseAutomapFlag(msg);
                break;
            case Proto::GameServerQuestLog:
                parseQuestLog(msg);
                break;
            case Proto::GameServerQuestLine:
                parseQuestLine(msg);
                break;
            // PROTOCOL>=870
            case Proto::GameServerSpellDelay:
                parseSpellCooldown(msg);
                break;
            case Proto::GameServerSpellGroupDelay:
                parseSpellGroupCooldown(msg);
                break;
            case Proto::GameServerMultiUseDelay:
                parseMultiUseCooldown(msg);
                break;
            // PROTOCOL>=910
            case Proto::GameServerChannelEvent:
                parseChannelEvent(msg);
                break;
            case Proto::GameServerItemInfo:
                parseItemInfo(msg);
                break;
            case Proto::GameServerPlayerInventory:
                parsePlayerInventory(msg);
                break;
            // PROTOCOL>=950
            case Proto::GameServerPlayerDataBasic:
                parsePlayerInfo(msg);
                break;
            // PROTOCOL>=970
            case Proto::GameServerModalDialog:
                parseModalDialog(msg);
                break;
            // PROTOCOL>=980
            case Proto::GameServerLoginSuccess:
                parseLogin(msg);
                break;
            case Proto::GameServerEnterGame:
                parseEnterGame(msg);
                break;
            // PROTOCOL>=1000
            case Proto::GameServerCreatureMarks:
                parseCreaturesMark(msg);
                break;
            case Proto::GameServerCreatureType:
                parseCreatureType(msg);
                break;
            // PROTOCOL>=1055
            case Proto::GameServerBlessings:
                parseBlessings(msg);
                break;
            case Proto::GameServerUnjustifiedStats:
                parseUnjustifiedStats(msg);
                break;
            case Proto::GameServerPvpSituations:
                parsePvpSituations(msg);
                break;
            case Proto::GameServerPreset:
                parsePreset(msg);
                break;
            // PROTOCOL>=1080
            case Proto::GameServerCoinBalanceUpdating:
                parseCoinBalanceUpdating(msg);
                break;
            case Proto::GameServerCoinBalance:
                parseCoinBalance(msg);
                break;
            case Proto::GameServerRequestPurchaseData:
                parseRequestPurchaseData(msg);
                break;
            case Proto::GameServerSendShowDescription:
                parseShowDescription(msg);
                break;
            case Proto::GameServerStoreCompletePurchase:
                parseCompleteStorePurchase(msg);
                break;
            case Proto::GameServerStoreOffers:
                parseStoreOffers(msg);
                break;
            case Proto::GameServerStoreTransactionHistory:
                parseStoreTransactionHistory(msg);
                break;
            case Proto::GameServerStoreError:
                parseStoreError(msg);
                break;
            case Proto::GameServerStore:
                parseStore(msg);
                break;
            // PROTOCOL>=1097
            case Proto::GameServerStoreButtonIndicators:
                parseStoreButtonIndicators(msg);
                break;
            case Proto::GameServerSetStoreDeepLink:
                parseSetStoreDeepLink(msg);
                break;
            // otclient ONLY
            case Proto::GameServerExtendedOpcode:
                parseExtendedOpcode(msg);
                break;
            case Proto::GameServerChangeMapAwareRange:
                parseChangeMapAwareRange(msg);
                break;
            // 12.x +
            case Proto::GameServerSendClientCheck:
                parseClientCheck(msg);
                break;
            case Proto::GameServerSendGameNews:
                parseGameNews(msg);
                break;
            case Proto::GameServerSendBlessDialog:
                parseBlessDialog(msg);
                break;
            case Proto::GameServerSendRestingAreaState:
                parseRestingAreaState(msg);
                break;
            case Proto::GameServerSendUpdateImpactTracker:
                parseUpdateImpactTracker(msg);
                break;
            case Proto::GameServerSendItemsPrice:
                parseItemsPrice(msg);
                break;
            case Proto::GameServerSendUpdateSupplyTracker:
                parseUpdateSupplyTracker(msg);
                break;
            case Proto::GameServerSendUpdateLootTracker:
                parseUpdateLootTracker(msg);
                break;
            case Proto::GameServerSendKillTrackerUpdate:
                parseKillTrackerUpdate(msg);
                break;
            case Proto::GameServerSendBestiaryEntryChanged:
                parseBestiaryEntryChanged(msg);
                break;
            case Proto::GameServerSendDailyRewardCollectionState:
                parseDailyRewardCollectionState(msg);
                break;
            case Proto::GameServerSendOpenRewardWall:
                parseOpenRewardWall(msg);
                break;
            case Proto::GameServerSendDailyReward:
                parseDailyReward(msg);
                break;
            case Proto::GameServerSendRewardHistory:
                parseRewardHistory(msg);
                break;
            case Proto::GameServerSendPreyTimeLeft:
                parsePreyTimeLeft(msg);
                break;
            case Proto::GameServerSendPreyData:
                parsePreyData(msg);
                break;
            case Proto::GameServerSendPreyRerollPrice:
                parsePreyRerollPrice(msg);
                break;
            case Proto::GameServerSendImbuementWindow:
                parseImbuementWindow(msg);
                break;
            case Proto::GameServerSendCloseImbuementWindow:
                parseCloseImbuementWindow(msg);
                break;
            case Proto::GameServerSendError:
                parseError(msg);
                break;
            case Proto::GameServerSendCollectionResource:
                parseCollectionResource(msg);
                break;
            case Proto::GameServerSendTibiaTime:
                parseTibiaTime(msg);
                break;
            default:
                stdext::throw_exception(stdext::format("unhandled opcode %d", (int)opcode));
                break;
            }
            prevOpcode = opcode;
        }
    }
    catch (stdext::exception &e)
    {
        g_logger.error(stdext::format("ProtocolGame parse message exception (%d bytes unread, last opcode is 0x%02x (%d), prev opcode is 0x%02x(%d)): %s",
                                      msg->getUnreadSize(), opcode, opcode, prevOpcode, prevOpcode, e.what()));
    }
}

void ProtocolGame::parseLogin(const InputMessagePtr &msg)
{
    uint playerId = msg->getU32();
    int serverBeat = msg->getU16();

    if (g_game.getFeature(Otc::GameNewSpeedLaw))
    {
        double speedA = msg->getDouble();
        double speedB = msg->getDouble();
        double speedC = msg->getDouble();
        m_localPlayer->setSpeedFormula(speedA, speedB, speedC);
    }
    bool canReportBugs = msg->getU8();

    if (g_game.getClientVersion() >= 1054)
        msg->getU8(); // can change pvp frame option

    if (g_game.getClientVersion() >= 1058)
    {
        int expertModeEnabled = msg->getU8();
        g_game.setExpertPvpMode(expertModeEnabled);
    }

    if (g_game.getFeature(Otc::GameIngameStore))
    {
        // URL to ingame store images
        msg->getString();

        // premium coin package size
        // e.g you can only buy packs of 25, 50, 75, .. coins in the market
        msg->getU16();
    }

    // TODO: Implement exiva button usage
    msg->getU8();
    // TODO: Implement tournament button usage
    // msg->getU8();

    m_localPlayer->setId(playerId);
    g_game.setServerBeat(serverBeat);
    g_game.setCanReportBugs(canReportBugs);

    g_game.processLogin();
}

void ProtocolGame::parsePendingGame(const InputMessagePtr &)
{
    //set player to pending game state
    g_game.processPendingGame();
}

void ProtocolGame::parseEnterGame(const InputMessagePtr &)
{
    //set player to entered game state
    g_game.processEnterGame();

    if (!m_gameInitialized)
    {
        g_game.processGameStart();
        m_gameInitialized = true;
    }
}

void ProtocolGame::parseStoreButtonIndicators(const InputMessagePtr &msg)
{
    msg->getU8(); // have sale item
    msg->getU8(); // have new item
}

void ProtocolGame::parseSetStoreDeepLink(const InputMessagePtr &msg)
{
    msg->getU8(); // currentlyFeaturedServiceType
}

void ProtocolGame::parseBlessings(const InputMessagePtr &msg)
{
    uint16 blessings = msg->getU16(); // bless flag
    uint8 blessStatus = msg->getU8(); // TODO: add usage to blessStatus - 1 = Disabled | 2 = normal | 3 = green

    // msg->getU16(); // extra weird u16 if bless > 5 = 1 else 0

    m_localPlayer->setBlessings(blessings);
}

void ProtocolGame::parsePreset(const InputMessagePtr &msg)
{
    msg->getU32(); // preset
}

void ProtocolGame::parseRequestPurchaseData(const InputMessagePtr &msg)
{
    msg->getU32(); // transactionId
    msg->getU8();  // productType
}

void ProtocolGame::parseShowDescription(const InputMessagePtr &msg)
{
    msg->getU32(); // offerId
    msg->getString();  // offer description
}

void ProtocolGame::parseStore(const InputMessagePtr &msg)
{
    parseCoinBalance(msg);

    int categories = msg->getU16();
    for (int i = 0; i < categories; i++)
    {
        std::string category = msg->getString();
        std::string description = msg->getString();

        int highlightState = 0;
        if (g_game.getFeature(Otc::GameIngameStoreHighlights))
            highlightState = msg->getU8();

        std::vector<std::string> icons;
        int iconCount = msg->getU8();
        for (int j = 0; j < iconCount; j++)
        {
            std::string icon = msg->getString();
            icons.push_back(icon);
        }

        // If this is a valid category name then
        // the category we just parsed is a child of that
        msg->getString();
    }
}

void ProtocolGame::parseCoinBalance(const InputMessagePtr &msg)
{
    bool update = msg->getU8() == 1;
    int coins = -1;
    int transferableCoins = -1;
    if (update)
    {
        // amount of coins that can be used to buy prodcuts
        // in the ingame store
        coins = msg->getU32();

        // amount of coins that can be sold in market
        // or be transfered to another player
        transferableCoins = msg->getU32();
    }
}

void ProtocolGame::parseCoinBalanceUpdating(const InputMessagePtr &msg)
{
    // coin balance can be updating and might not be accurate
    bool isUpdating = msg->getU8() == 1;
}

void ProtocolGame::parseCompleteStorePurchase(const InputMessagePtr &msg)
{
    // not used
    msg->getU8();

    std::string message = msg->getString();
    int coins = msg->getU32();
    int transferableCoins = msg->getU32();

    g_logger.info(stdext::format("Purchase Complete: %s\nAvailable coins: %d (transferable: %d)", message, coins, transferableCoins));
}

void ProtocolGame::parseStoreTransactionHistory(const InputMessagePtr &msg)
{
    int currentPage = msg->getU32();
    msg->getU32(); // pageCount

    int entries = msg->getU8();
    for (int i = 0; i < entries; i++)
    {
        int time = msg->getU32();
        int productType = msg->getU8();
        int coinChange = msg->getU32();
        msg->getU8(); // 0 = transferable tibia coin, 1 = normal tibia coin
        std::string productName = msg->getString();
        g_logger.error(stdext::format("Time %i, type %i, change %i, product name %s", time, productType, coinChange, productName));
    }
}

void ProtocolGame::parseStoreOffers(const InputMessagePtr &msg)
{
    msg->getString(); // categoryName

    int offers = msg->getU16();
    for (int i = 0; i < offers; i++)
    {
        msg->getU32();    // offerId
        msg->getString(); // offerName
        msg->getString(); // offerDescription

        msg->getU32(); // price
        int highlightState = msg->getU8();
        if (highlightState == 2 && g_game.getFeature(Otc::GameIngameStoreHighlights) && g_game.getClientVersion() >= 1097)
        {
            msg->getU32(); // saleValidUntilTimestamp
            msg->getU32(); // basePrice
        }

        int disabledState = msg->getU8();
        if (g_game.getFeature(Otc::GameIngameStoreHighlights) && disabledState == 1)
        {
            msg->getString(); // disabledReason
        }

        std::vector<std::string> icons;
        int iconCount = msg->getU8();
        for (int j = 0; j < iconCount; j++)
        {
            std::string icon = msg->getString();
            icons.push_back(icon);
        }

        int subOffers = msg->getU16();
        for (int j = 0; j < subOffers; j++)
        {
            msg->getString(); // name
            msg->getString(); // description

            int subIcons = msg->getU8();
            for (int k = 0; k < subIcons; k++)
            {
                msg->getString(); // icon
            }
            msg->getString(); // serviceType
        }
    }
}

void ProtocolGame::parseStoreError(const InputMessagePtr &msg)
{
    int errorType = msg->getU8();
    std::string message = msg->getString();
    g_logger.error(stdext::format("Store Error: %s [%i]", message, errorType));
}

void ProtocolGame::parseUnjustifiedStats(const InputMessagePtr &msg)
{
    UnjustifiedPoints unjustifiedPoints;
    unjustifiedPoints.killsDay = msg->getU8();
    unjustifiedPoints.killsDayRemaining = msg->getU8();
    unjustifiedPoints.killsWeek = msg->getU8();
    unjustifiedPoints.killsWeekRemaining = msg->getU8();
    unjustifiedPoints.killsMonth = msg->getU8();
    unjustifiedPoints.killsMonthRemaining = msg->getU8();
    unjustifiedPoints.skullTime = msg->getU8();

    g_game.setUnjustifiedPoints(unjustifiedPoints);
}

void ProtocolGame::parsePvpSituations(const InputMessagePtr &msg)
{
    uint8 openPvpSituations = msg->getU8();

    g_game.setOpenPvpSituations(openPvpSituations);
}

void ProtocolGame::parseGMActions(const InputMessagePtr &msg)
{
    std::vector<uint8> actions;

    int numViolationReasons;

    if (g_game.getClientVersion() >= 850)
        numViolationReasons = 20;
    else if (g_game.getClientVersion() >= 840)
        numViolationReasons = 23;
    else
        numViolationReasons = 32;

    for (int i = 0; i < numViolationReasons; ++i)
        actions.push_back(msg->getU8());
    g_game.processGMActions(actions);
}

void ProtocolGame::parseUpdateNeeded(const InputMessagePtr &msg)
{
    std::string signature = msg->getString();
    g_game.processUpdateNeeded(signature);
}

void ProtocolGame::parseLoginError(const InputMessagePtr &msg)
{
    std::string error = msg->getString();

    g_game.processLoginError(error);
}

void ProtocolGame::parseLoginAdvice(const InputMessagePtr &msg)
{
    std::string message = msg->getString();

    g_game.processLoginAdvice(message);
}

void ProtocolGame::parseLoginWait(const InputMessagePtr &msg)
{
    std::string message = msg->getString();
    int time = msg->getU8();

    g_game.processLoginWait(message, time);
}

void ProtocolGame::parseLoginToken(const InputMessagePtr &msg)
{
    bool unknown = (msg->getU8() == 0);
    g_game.processLoginToken(unknown);
}

void ProtocolGame::parsePing(const InputMessagePtr &msg)
{
    g_game.processPing();
}

void ProtocolGame::parsePingBack(const InputMessagePtr &msg)
{
    g_game.processPingBack();
}

void ProtocolGame::parseChallenge(const InputMessagePtr &msg)
{
    uint timestamp = msg->getU32();
    uint8 random = msg->getU8();

    sendLoginPacket(timestamp, random);
}

void ProtocolGame::parseDeath(const InputMessagePtr &msg)
{
    int penality = 100;
    int deathType = Otc::DeathRegular;
    bool deathRedemption = false;

    deathType = msg->getU8();
    penality = msg->getU8();
    deathRedemption = msg->getU8(); // TODO: apply deathRedemption value

    g_game.processDeath(deathType, penality);
}

void ProtocolGame::parseMapDescription(const InputMessagePtr &msg)
{
    Position pos = getPosition(msg);

    if (!m_mapKnown)
        m_localPlayer->setPosition(pos);

    g_map.setCentralPosition(pos);

    AwareRange range = g_map.getAwareRange();
    setMapDescription(msg, pos.x - range.left, pos.y - range.top, pos.z, range.horizontal(), range.vertical());

    if (!m_mapKnown)
    {
        g_dispatcher.addEvent([] { g_lua.callGlobalField("g_game", "onMapKnown"); });
        m_mapKnown = true;
    }

    g_dispatcher.addEvent([] { g_lua.callGlobalField("g_game", "onMapDescription"); });
}

void ProtocolGame::parseMapMoveNorth(const InputMessagePtr &msg)
{
    Position pos;
    if (g_game.getFeature(Otc::GameMapMovePosition))
        pos = getPosition(msg);
    else
        pos = g_map.getCentralPosition();
    pos.y--;

    AwareRange range = g_map.getAwareRange();
    setMapDescription(msg, pos.x - range.left, pos.y - range.top, pos.z, range.horizontal(), 1);
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseMapMoveEast(const InputMessagePtr &msg)
{
    Position pos;
    if (g_game.getFeature(Otc::GameMapMovePosition))
        pos = getPosition(msg);
    else
        pos = g_map.getCentralPosition();
    pos.x++;

    AwareRange range = g_map.getAwareRange();
    setMapDescription(msg, pos.x + range.right, pos.y - range.top, pos.z, 1, range.vertical());
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseMapMoveSouth(const InputMessagePtr &msg)
{
    Position pos;
    if (g_game.getFeature(Otc::GameMapMovePosition))
        pos = getPosition(msg);
    else
        pos = g_map.getCentralPosition();
    pos.y++;

    AwareRange range = g_map.getAwareRange();
    setMapDescription(msg, pos.x - range.left, pos.y + range.bottom, pos.z, range.horizontal(), 1);
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseMapMoveWest(const InputMessagePtr &msg)
{
    Position pos;
    if (g_game.getFeature(Otc::GameMapMovePosition))
        pos = getPosition(msg);
    else
        pos = g_map.getCentralPosition();
    pos.x--;

    AwareRange range = g_map.getAwareRange();
    setMapDescription(msg, pos.x - range.left, pos.y - range.top, pos.z, 1, range.vertical());
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseUpdateTile(const InputMessagePtr &msg)
{
    Position tilePos = getPosition(msg);
    setTileDescription(msg, tilePos);
}

void ProtocolGame::parseTileAddThing(const InputMessagePtr &msg)
{
    Position pos = getPosition(msg);
    int stackPos = -1;

    if (g_game.getClientVersion() >= 841)
        stackPos = msg->getU8();

    ThingPtr thing = getThing(msg);
    g_map.addThing(thing, pos, stackPos);
}

void ProtocolGame::parseTileTransformThing(const InputMessagePtr &msg)
{
    ThingPtr thing = getMappedThing(msg);
    ThingPtr newThing = getThing(msg);

    if (!thing)
    {
        g_logger.traceError("no thing");
        return;
    }

    Position pos = thing->getPosition();
    int stackpos = thing->getStackPos();

    if (!g_map.removeThing(thing))
    {
        g_logger.traceError("unable to remove thing");
        return;
    }

    g_map.addThing(newThing, pos, stackpos);
}

void ProtocolGame::parseTileRemoveThing(const InputMessagePtr &msg)
{
    ThingPtr thing = getMappedThing(msg);
    if (!thing)
    {
        g_logger.traceError("no thing");
        return;
    }

    if (!g_map.removeThing(thing))
        g_logger.traceError("unable to remove thing");
}

void ProtocolGame::parseCreatureMove(const InputMessagePtr &msg)
{
    ThingPtr thing = getMappedThing(msg);
    Position newPos = getPosition(msg);

    if (!thing || !thing->isCreature())
    {
        g_logger.traceError("no creature found to move");
        return;
    }

    if (!g_map.removeThing(thing))
    {
        g_logger.traceError("unable to remove creature");
        return;
    }

    CreaturePtr creature = thing->static_self_cast<Creature>();
    creature->allowAppearWalk();

    g_map.addThing(thing, newPos, -1);
}

void ProtocolGame::parseOpenContainer(const InputMessagePtr &msg)
{
    int containerId = msg->getU8();
    ItemPtr containerItem = getItem(msg);
    std::string name = msg->getString();
    int capacity = msg->getU8();
    bool hasParent = (msg->getU8() != 0);

    bool isUnlocked = true;
    bool hasPages = false;
    int containerSize = 0;
    int firstIndex = 0;

    if (g_game.getFeature(Otc::GameContainerPagination))
    {
        isUnlocked = (msg->getU8() != 0); // drag and drop
        hasPages = (msg->getU8() != 0);   // pagination
        containerSize = msg->getU16();    // container size
        firstIndex = msg->getU16();       // first index
    }

    int itemCount = msg->getU8();

    std::vector<ItemPtr> items(itemCount);
    for (int i = 0; i < itemCount; i++)
        items[i] = getItem(msg);

    g_game.processOpenContainer(containerId, containerItem, name, capacity, hasParent, items, isUnlocked, hasPages, containerSize, firstIndex);
}

void ProtocolGame::parseCloseContainer(const InputMessagePtr &msg)
{
    int containerId = msg->getU8();
    g_game.processCloseContainer(containerId);
}

void ProtocolGame::parseContainerAddItem(const InputMessagePtr &msg)
{
    int containerId = msg->getU8();
    int slot = 0;
    if (g_game.getFeature(Otc::GameContainerPagination))
    {
        slot = msg->getU16(); // slot
    }
    ItemPtr item = getItem(msg);
    g_game.processContainerAddItem(containerId, item, slot);
}

void ProtocolGame::parseContainerUpdateItem(const InputMessagePtr &msg)
{
    int containerId = msg->getU8();
    int slot;
    if (g_game.getFeature(Otc::GameContainerPagination))
    {
        slot = msg->getU16();
    }
    else
    {
        slot = msg->getU8();
    }
    ItemPtr item = getItem(msg);
    g_game.processContainerUpdateItem(containerId, slot, item);
}

void ProtocolGame::parseContainerRemoveItem(const InputMessagePtr &msg)
{
    int containerId = msg->getU8();
    int slot;
    ItemPtr lastItem;
    if (g_game.getFeature(Otc::GameContainerPagination))
    {
        slot = msg->getU16();

        int itemId = msg->getU16();
        if (itemId != 0)
            lastItem = getItem(msg, itemId);
    }
    else
    {
        slot = msg->getU8();
    }
    g_game.processContainerRemoveItem(containerId, slot, lastItem);
}

void ProtocolGame::parseAddInventoryItem(const InputMessagePtr &msg)
{
    int slot = msg->getU8();
    ItemPtr item = getItem(msg);
    g_game.processInventoryChange(slot, item);
}

void ProtocolGame::parseRemoveInventoryItem(const InputMessagePtr &msg)
{
    int slot = msg->getU8();
    g_game.processInventoryChange(slot, ItemPtr());
}

void ProtocolGame::parseOpenNpcTrade(const InputMessagePtr &msg)
{
    std::vector<std::tuple<ItemPtr, std::string, int, int, int>> items;
    std::string npcName;

    if (g_game.getFeature(Otc::GameNameOnNpcTrade))
        npcName = msg->getString();

    int listCount;

    if (g_game.getClientVersion() >= 900)
        listCount = msg->getU16();
    else
        listCount = msg->getU8();

    for (int i = 0; i < listCount; ++i)
    {
        uint16 itemId = msg->getU16();
        uint8 count = msg->getU8();

        ItemPtr item = Item::create(itemId);
        item->setCountOrSubType(count);

        std::string name = msg->getString();
        int weight = msg->getU32();
        int buyPrice = msg->getU32();
        int sellPrice = msg->getU32();
        items.emplace_back(item, name, weight, buyPrice, sellPrice);
    }

    g_game.processOpenNpcTrade(items);
}

void ProtocolGame::parsePlayerGoods(const InputMessagePtr &msg)
{
    std::vector<std::tuple<ItemPtr, int>> goods;

    int money;
    if (g_game.getClientVersion() >= 973)
        money = msg->getU64();
    else
        money = msg->getU32();

    int size = msg->getU8();
    for (int i = 0; i < size; i++)
    {
        int itemId = msg->getU16();
        int amount;

        if (g_game.getFeature(Otc::GameDoubleShopSellAmount))
            amount = msg->getU16();
        else
            amount = msg->getU8();

        goods.emplace_back(Item::create(itemId), amount);
    }

    g_game.processPlayerGoods(money, goods);
}

void ProtocolGame::parseCloseNpcTrade(const InputMessagePtr &)
{
    g_game.processCloseNpcTrade();
}

void ProtocolGame::parseOwnTrade(const InputMessagePtr &msg)
{
    std::string name = g_game.formatCreatureName(msg->getString());
    int count = msg->getU8();

    std::vector<ItemPtr> items(count);
    for (int i = 0; i < count; i++)
        items[i] = getItem(msg);

    g_game.processOwnTrade(name, items);
}

void ProtocolGame::parseCounterTrade(const InputMessagePtr &msg)
{
    std::string name = g_game.formatCreatureName(msg->getString());
    int count = msg->getU8();

    std::vector<ItemPtr> items(count);
    for (int i = 0; i < count; i++)
        items[i] = getItem(msg);

    g_game.processCounterTrade(name, items);
}

void ProtocolGame::parseCloseTrade(const InputMessagePtr &)
{
    g_game.processCloseTrade();
}

void ProtocolGame::parseWorldLight(const InputMessagePtr &msg)
{
    Light light;
    light.intensity = msg->getU8();
    light.color = msg->getU8();

    g_map.setLight(light);
}

void ProtocolGame::parseMagicEffect(const InputMessagePtr &msg)
{
    Position pos = getPosition(msg);
    int effectId;
    if (g_game.getFeature(Otc::GameMagicEffectU16))
        effectId = msg->getU16();
    else
        effectId = msg->getU8();

    if (!g_things.isValidDatId(effectId, ThingCategoryEffect))
    {
        g_logger.traceError(stdext::format("invalid effect id %d", effectId));
        return;
    }

    EffectPtr effect = EffectPtr(new Effect());
    effect->setId(effectId);
    g_map.addThing(effect, pos);
}

void ProtocolGame::parseAnimatedText(const InputMessagePtr &msg)
{
    Position position = getPosition(msg);
    int color = msg->getU8();
    std::string text = msg->getString();

    AnimatedTextPtr animatedText = AnimatedTextPtr(new AnimatedText);
    animatedText->setColor(color);
    animatedText->setText(text);
    g_map.addThing(animatedText, position);
}

void ProtocolGame::parseDistanceMissile(const InputMessagePtr &msg)
{
    Position fromPos = getPosition(msg);
    Position toPos = getPosition(msg);
    int shotId = msg->getU8();

    if (!g_things.isValidDatId(shotId, ThingCategoryMissile))
    {
        g_logger.traceError(stdext::format("invalid missile id %d", shotId));
        return;
    }

    MissilePtr missile = MissilePtr(new Missile());
    missile->setId(shotId);
    missile->setPath(fromPos, toPos);
    g_map.addThing(missile, fromPos);
}

void ProtocolGame::parseCreatureMark(const InputMessagePtr &msg)
{
    uint id = msg->getU32();
    int color = msg->getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if (creature)
        creature->addTimedSquare(color);
    else
        g_logger.traceError("could not get creature");
}

void ProtocolGame::parseTrappers(const InputMessagePtr &msg)
{
    int numTrappers = msg->getU8();

    if (numTrappers > 8)
        g_logger.traceError("too many trappers");

    for (int i = 0; i < numTrappers; ++i)
    {
        uint id = msg->getU32();
        CreaturePtr creature = g_map.getCreatureById(id);
        if (creature)
        {
            //TODO: set creature as trapper
        }
        else
            g_logger.traceError("could not get creature");
    }
}

void ProtocolGame::parseCreatureHealth(const InputMessagePtr &msg)
{
    uint id = msg->getU32();
    int healthPercent = msg->getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if (creature)
        creature->setHealthPercent(healthPercent);

    // some servers has a bug in get spectators and sends unknown creatures updates
    // so this code is disabled
    /*
    else
        g_logger.traceError("could not get creature");
    */
}

void ProtocolGame::parseCreatureLight(const InputMessagePtr &msg)
{
    uint id = msg->getU32();

    Light light;
    light.intensity = msg->getU8();
    light.color = msg->getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if (creature)
        creature->setLight(light);
    else
        g_logger.traceError("could not get creature");
}

void ProtocolGame::parseCreatureOutfit(const InputMessagePtr &msg)
{
    uint id = msg->getU32();
    Outfit outfit = getOutfit(msg);

    CreaturePtr creature = g_map.getCreatureById(id);
    if (creature)
        creature->setOutfit(outfit);
    else
        g_logger.traceError("could not get creature");
}

void ProtocolGame::parseCreatureSpeed(const InputMessagePtr &msg)
{
    uint id = msg->getU32();

    int baseSpeed = -1;
    if (g_game.getClientVersion() >= 1059)
        baseSpeed = msg->getU16();

    int speed = msg->getU16();

    CreaturePtr creature = g_map.getCreatureById(id);
    if (creature)
    {
        creature->setSpeed(speed);
        if (baseSpeed != -1)
            creature->setBaseSpeed(baseSpeed);
    }

    // some servers has a bug in get spectators and sends unknown creatures updates
    // so this code is disabled
    /*
    else
        g_logger.traceError("could not get creature");
    */
}

void ProtocolGame::parseCreatureSkulls(const InputMessagePtr &msg)
{
    uint id = msg->getU32();
    int skull = msg->getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if (creature)
        creature->setSkull(skull);
    else
        g_logger.traceError("could not get creature");
}

void ProtocolGame::parseCreatureShields(const InputMessagePtr &msg)
{
    uint id = msg->getU32();
    int shield = msg->getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if (creature)
        creature->setShield(shield);
    else
        g_logger.traceError("could not get creature");
}

void ProtocolGame::parseCreatureUnpass(const InputMessagePtr &msg)
{
    uint id = msg->getU32();
    bool unpass = msg->getU8();

    CreaturePtr creature = g_map.getCreatureById(id);
    if (creature)
        creature->setPassable(!unpass);
    else
        g_logger.traceError("could not get creature");
}

void ProtocolGame::parseEditText(const InputMessagePtr &msg)
{
    uint id = msg->getU32();

    int itemId;
    if (g_game.getClientVersion() >= 1010)
    {
        // TODO: processEditText with ItemPtr as parameter
        ItemPtr item = getItem(msg);
        itemId = item->getId();
    }
    else
        itemId = msg->getU16();

    int maxLength = msg->getU16();
    std::string text = msg->getString();

    std::string writer = msg->getString();
    std::string date = "";
    if (g_game.getFeature(Otc::GameWritableDate))
        date = msg->getString();

    g_game.processEditText(id, itemId, maxLength, text, writer, date);
}

void ProtocolGame::parseEditList(const InputMessagePtr &msg)
{
    int doorId = msg->getU8();
    uint id = msg->getU32();
    const std::string &text = msg->getString();

    g_game.processEditList(id, doorId, text);
}

void ProtocolGame::parsePremiumTrigger(const InputMessagePtr &msg)
{
    int triggerCount = msg->getU8();
    std::vector<int> triggers;
    for (int i = 0; i < triggerCount; ++i)
    {
        triggers.push_back(msg->getU8());
    }

    if (g_game.getClientVersion() <= 1096)
    {
        bool something = msg->getU8() == 1;
    }
}

void ProtocolGame::parsePlayerInfo(const InputMessagePtr &msg)
{
    bool premium = msg->getU8(); // premium
    msg->getU32();           // premium expiration used for premium advertisement
    int vocation = msg->getU8(); // vocation

    msg->getU8(); // prey window byte

    int spellCount = msg->getU16();
    std::vector<int> spells;
    for (int i = 0; i < spellCount; ++i)
        spells.push_back(msg->getU8()); // spell id

    m_localPlayer->setPremium(premium);
    m_localPlayer->setVocation(vocation);
    m_localPlayer->setSpells(spells);
}

void ProtocolGame::parsePlayerStats(const InputMessagePtr &msg)
{
    double health;
    double maxHealth;

    if (g_game.getFeature(Otc::GameDoubleHealth))
    {
        health = msg->getU32();
        maxHealth = msg->getU32();
    }
    else
    {
        health = msg->getU16();
        maxHealth = msg->getU16();
    }

    double freeCapacity = msg->getU32() / 100.0;
    double experience = msg->getU64();
    double level = msg->getU16();
    double levelPercent = msg->getU8();

    msg->getU16();    // baseXpGain
    msg->getU16();    // grindingAddend
    msg->getU16();    // storeBoostAddend
    msg->getU16();    // huntingBoostFactor

    double mana;
    double maxMana;

    if (g_game.getFeature(Otc::GameDoubleHealth))
    {
        mana = msg->getU32();
        maxMana = msg->getU32();
    }
    else
    {
        mana = msg->getU16();
        maxMana = msg->getU16();
    }

    double soul = msg->getU8();
    double stamina = msg->getU16();
    double baseSpeed = msg->getU16();
    double regeneration = msg->getU16();
    double training = msg->getU16();
    int remainingStoreXpBoostSeconds = msg->getU16();
    bool canBuyMoreStoreXpBoosts = msg->getU8();

    m_localPlayer->setHealth(health, maxHealth);
    m_localPlayer->setFreeCapacity(freeCapacity);

    m_localPlayer->setExperience(experience);
    m_localPlayer->setLevel(level, levelPercent);
    m_localPlayer->setMana(mana, maxMana);

    m_localPlayer->setStamina(stamina);
    m_localPlayer->setSoul(soul);
    m_localPlayer->setBaseSpeed(baseSpeed);
    m_localPlayer->setRegenerationTime(regeneration);
    m_localPlayer->setOfflineTrainingTime(training);
}

void ProtocolGame::parsePlayerSkills(const InputMessagePtr &msg)
{
    uint16_t magicLevel = msg->getU16(); // magic level
    uint16_t baseMagicLevel = msg->getU16(); // base magic level
    uint16_t magicLevelPercent = msg->getU16(); // magic level percent

    m_localPlayer->setMagicLevel(magicLevel, magicLevelPercent);
    m_localPlayer->setBaseMagicLevel(baseMagicLevel);

    for (int skill = 0; skill < Otc::LastSkill; skill++) {
        uint16_t level = msg->getU16();
        uint16_t baseLevel = msg->getU16();

        // Critical, Life Leech and Mana Leech have no level percent
        uint16_t levelPercent = skill <= Otc::Fishing ? msg->getU16() : 0;

        m_localPlayer->setSkill((Otc::Skill)skill, level, levelPercent);
        m_localPlayer->setBaseSkill((Otc::Skill)skill, baseLevel);
    }
    
    uint32_t totalCapacity = msg->getU32(); // total capacity
    m_localPlayer->setTotalCapacity(totalCapacity);

    // TODO: apply total base capacity usage
    msg->getU32(); // total base capacity
}

void ProtocolGame::parsePlayerState(const InputMessagePtr &msg)
{
    m_localPlayer->setStates(msg->getU32());
}

void ProtocolGame::parsePlayerCancelAttack(const InputMessagePtr &msg)
{
    uint seq = 0;
    if (g_game.getFeature(Otc::GameAttackSeq))
        seq = msg->getU32();

    g_game.processAttackCancel(seq);
}

void ProtocolGame::parsePlayerModes(const InputMessagePtr &msg)
{
    int fightMode = msg->getU8();
    int chaseMode = msg->getU8();
    bool safeMode = msg->getU8();

    int pvpMode = 0;
    if (g_game.getFeature(Otc::GamePVPMode))
        pvpMode = msg->getU8();

    g_game.processPlayerModes((Otc::FightModes)fightMode, (Otc::ChaseModes)chaseMode, safeMode, (Otc::PVPModes)pvpMode);
}

void ProtocolGame::parseSpellCooldown(const InputMessagePtr &msg)
{
    int spellId = msg->getU8();
    int delay = msg->getU32();

    // TODO: verify if there are icons for spells id 170+ and remove the ternary check (if id >170 => 150)
    g_lua.callGlobalField("g_game", "onSpellCooldown", (spellId >= 170) ? 150 : spellId, delay);
}

void ProtocolGame::parseSpellGroupCooldown(const InputMessagePtr &msg)
{
    int groupId = msg->getU8();
    int delay = msg->getU32();

    g_lua.callGlobalField("g_game", "onSpellGroupCooldown", groupId, delay);
}

void ProtocolGame::parseMultiUseCooldown(const InputMessagePtr &msg)
{
    int delay = msg->getU32();

    g_lua.callGlobalField("g_game", "onMultiUseCooldown", delay);
}

void ProtocolGame::parseTalk(const InputMessagePtr &msg)
{
    if (g_game.getFeature(Otc::GameMessageStatements))
        msg->getU32(); // channel statement guid

    std::string name = g_game.formatCreatureName(msg->getString());

    int level = 0;
    if (g_game.getFeature(Otc::GameMessageLevel))
        level = msg->getU16();

    Otc::MessageMode mode = Proto::translateMessageModeFromServer(msg->getU8());
    int channelId = 0;
    Position pos;

    switch (mode)
    {
    case Otc::MessageSay:
    case Otc::MessageWhisper:
    case Otc::MessageYell:
    case Otc::MessageMonsterSay:
    case Otc::MessageMonsterYell:
    case Otc::MessageNpcTo:
    case Otc::MessageBarkLow:
    case Otc::MessageBarkLoud:
    case Otc::MessageSpell:
    case Otc::MessageNpcFromStartBlock:
        pos = getPosition(msg);
        break;
    case Otc::MessageChannel:
    case Otc::MessageChannelManagement:
    case Otc::MessageChannelHighlight:
    case Otc::MessageGamemasterChannel:
        channelId = msg->getU16();
        break;
    case Otc::MessageNpcFrom:
    case Otc::MessagePrivateFrom:
    case Otc::MessageGamemasterBroadcast:
    case Otc::MessageGamemasterPrivateFrom:
    case Otc::MessageRVRAnswer:
    case Otc::MessageRVRContinue:
        break;
    case Otc::MessageRVRChannel:
        msg->getU32();
        break;
    default:
        stdext::throw_exception(stdext::format("unknown message mode %d", mode));
        break;
    }

    std::string text = msg->getString();

    g_game.processTalk(name, level, mode, text, channelId, pos);
}

void ProtocolGame::parseChannelList(const InputMessagePtr &msg)
{
    int count = msg->getU8();
    std::vector<std::tuple<int, std::string>> channelList;
    for (int i = 0; i < count; i++)
    {
        int id = msg->getU16();
        std::string name = msg->getString();
        channelList.emplace_back(id, name);
    }

    g_game.processChannelList(channelList);
}

void ProtocolGame::parseOpenChannel(const InputMessagePtr &msg)
{
    int channelId = msg->getU16();
    std::string name = msg->getString();

    if (g_game.getFeature(Otc::GameChannelPlayerList))
    {
        int joinedPlayers = msg->getU16();
        for (int i = 0; i < joinedPlayers; ++i)
            g_game.formatCreatureName(msg->getString()); // player name
        int invitedPlayers = msg->getU16();
        for (int i = 0; i < invitedPlayers; ++i)
            g_game.formatCreatureName(msg->getString()); // player name
    }

    g_game.processOpenChannel(channelId, name);
}

void ProtocolGame::parseOpenPrivateChannel(const InputMessagePtr &msg)
{
    std::string name = g_game.formatCreatureName(msg->getString());

    g_game.processOpenPrivateChannel(name);
}

void ProtocolGame::parseOpenOwnPrivateChannel(const InputMessagePtr &msg)
{
    int channelId = msg->getU16();
    std::string name = msg->getString();

    g_game.processOpenOwnPrivateChannel(channelId, name);
}

void ProtocolGame::parseCloseChannel(const InputMessagePtr &msg)
{
    int channelId = msg->getU16();

    g_game.processCloseChannel(channelId);
}

void ProtocolGame::parseRuleViolationChannel(const InputMessagePtr &msg)
{
    int channelId = msg->getU16();

    g_game.processRuleViolationChannel(channelId);
}

void ProtocolGame::parseRuleViolationRemove(const InputMessagePtr &msg)
{
    std::string name = msg->getString();

    g_game.processRuleViolationRemove(name);
}

void ProtocolGame::parseRuleViolationCancel(const InputMessagePtr &msg)
{
    std::string name = msg->getString();

    g_game.processRuleViolationCancel(name);
}

void ProtocolGame::parseRuleViolationLock(const InputMessagePtr &msg)
{
    g_game.processRuleViolationLock();
}

void ProtocolGame::parseTextMessage(const InputMessagePtr &msg)
{
    int code = msg->getU8();
    Otc::MessageMode mode = Proto::translateMessageModeFromServer(code);
    std::string text;

    switch (mode)
    {
    case Otc::MessageChannelManagement:
    {
        msg->getU16(); // channelId
        text = msg->getString();
        break;
    }
    case Otc::MessageGuild:
    case Otc::MessagePartyManagement:
    case Otc::MessageParty:
    {
        msg->getU16(); // channelId
        text = msg->getString();
        break;
    }
    case Otc::MessageDamageDealed:
    case Otc::MessageDamageReceived:
    case Otc::MessageDamageOthers:
    {
        Position pos = getPosition(msg);
        uint value[2];
        int color[2];

        // physical damage
        value[0] = msg->getU32();
        color[0] = msg->getU8();

        // magic damage
        value[1] = msg->getU32();
        color[1] = msg->getU8();
        text = msg->getString();

        for (int i = 0; i < 2; ++i)
        {
            if (value[i] == 0)
                continue;
            AnimatedTextPtr animatedText = AnimatedTextPtr(new AnimatedText);
            animatedText->setColor(color[i]);
            animatedText->setText(stdext::to_string(value[i]));
            g_map.addThing(animatedText, pos);
        }
        break;
    }
    case Otc::MessageHeal:
    case Otc::MessageMana:
    case Otc::MessageExp:
    case Otc::MessageHealOthers:
    case Otc::MessageExpOthers:
    {
        Position pos = getPosition(msg);
        uint value = msg->getU32();
        int color = msg->getU8();
        text = msg->getString();

        AnimatedTextPtr animatedText = AnimatedTextPtr(new AnimatedText);
        animatedText->setColor(color);
        animatedText->setText(stdext::to_string(value));
        g_map.addThing(animatedText, pos);
        break;
    }
    case Otc::MessageInvalid:
        stdext::throw_exception(stdext::format("unknown message mode %d", mode));
        break;
    default:
        text = msg->getString();
        break;
    }

    g_game.processTextMessage(mode, text);
}

void ProtocolGame::parseCancelWalk(const InputMessagePtr &msg)
{
    Otc::Direction direction = (Otc::Direction)msg->getU8();

    g_game.processWalkCancel(direction);
}

void ProtocolGame::parseWalkWait(const InputMessagePtr &msg)
{
    int millis = msg->getU16();
    m_localPlayer->lockWalk(millis);
}

void ProtocolGame::parseFloorChangeUp(const InputMessagePtr &msg)
{
    Position pos;
    if (g_game.getFeature(Otc::GameMapMovePosition))
        pos = getPosition(msg);
    else
        pos = g_map.getCentralPosition();
    AwareRange range = g_map.getAwareRange();
    pos.z--;

    int skip = 0;
    if (pos.z == Otc::SEA_FLOOR)
        for (int i = Otc::SEA_FLOOR - Otc::AWARE_UNDEGROUND_FLOOR_RANGE; i >= 0; i--)
            skip = setFloorDescription(msg, pos.x - range.left, pos.y - range.top, i, range.horizontal(), range.vertical(), 8 - i, skip);
    else if (pos.z > Otc::SEA_FLOOR)
        skip = setFloorDescription(msg, pos.x - range.left, pos.y - range.top, pos.z - Otc::AWARE_UNDEGROUND_FLOOR_RANGE, range.horizontal(), range.vertical(), 3, skip);

    pos.x++;
    pos.y++;
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseFloorChangeDown(const InputMessagePtr &msg)
{
    Position pos;
    if (g_game.getFeature(Otc::GameMapMovePosition))
        pos = getPosition(msg);
    else
        pos = g_map.getCentralPosition();
    AwareRange range = g_map.getAwareRange();
    pos.z++;

    int skip = 0;
    if (pos.z == Otc::UNDERGROUND_FLOOR)
    {
        int j, i;
        for (i = pos.z, j = -1; i <= pos.z + Otc::AWARE_UNDEGROUND_FLOOR_RANGE; ++i, --j)
            skip = setFloorDescription(msg, pos.x - range.left, pos.y - range.top, i, range.horizontal(), range.vertical(), j, skip);
    }
    else if (pos.z > Otc::UNDERGROUND_FLOOR && pos.z < Otc::MAX_Z - 1)
        skip = setFloorDescription(msg, pos.x - range.left, pos.y - range.top, pos.z + Otc::AWARE_UNDEGROUND_FLOOR_RANGE, range.horizontal(), range.vertical(), -3, skip);

    pos.x--;
    pos.y--;
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseOpenOutfitWindow(const InputMessagePtr &msg)
{
    Outfit currentOutfit = getOutfit(msg);
    std::vector<std::tuple<int, std::string, int>> outfitList;

    // new outfit model size is uint_16
    uint16_t outfitCount = msg->getU16();
    for (int i = 0; i < outfitCount; i++)
    {
        int outfitId = msg->getU16();
        std::string outfitName = msg->getString();
        int outfitAddons = msg->getU8();

        // TODO: identify extra null byte usage 12.x
        msg->getU8();

        outfitList.emplace_back(outfitId, outfitName, outfitAddons);
    }

    std::vector<std::tuple<int, std::string>> mountList;
    if (g_game.getFeature(Otc::GamePlayerMounts))
    {
        // new mounts mondel size is uint_16
        int mountCount = msg->getU16();
        for (int i = 0; i < mountCount; ++i)
        {
            int mountId = msg->getU16();              // mount type
            std::string mountName = msg->getString(); // mount name

            // TODO: identify extra null byte usage 12.x
            msg->getU8();

            mountList.emplace_back(mountId, mountName);
        }
    }

    // TODO: identify extra null byte usage 12.x
    msg->getU8();
    msg->getU8();

    g_game.processOpenOutfitWindow(currentOutfit, outfitList, mountList);
}

void ProtocolGame::parseVipAdd(const InputMessagePtr &msg)
{
    uint id, iconId = 0, status;
    std::string name, desc = "";
    bool notifyLogin = false;

    id = msg->getU32();
    name = g_game.formatCreatureName(msg->getString());
    if (g_game.getFeature(Otc::GameAdditionalVipInfo))
    {
        desc = msg->getString();
        iconId = msg->getU32();
        notifyLogin = msg->getU8();
    }
    status = msg->getU8();

    // TODO: implement vipGroups usage
    msg->getU8(); // vipGroup by id

    g_game.processVipAdd(id, name, status, desc, iconId, notifyLogin);
}

void ProtocolGame::parseVipState(const InputMessagePtr &msg)
{
    uint id = msg->getU32();
    if (g_game.getFeature(Otc::GameLoginPending))
    {
        uint status = msg->getU8();
        g_game.processVipStateChange(id, status);
    }
    else
    {
        g_game.processVipStateChange(id, 1);
    }
}

void ProtocolGame::parseVipLogout(const InputMessagePtr &msg)
{
    uint id = msg->getU32();
    g_game.processVipStateChange(id, 0);
}

void ProtocolGame::parseTutorialHint(const InputMessagePtr &msg)
{
    int id = msg->getU8();
    g_game.processTutorialHint(id);
}

void ProtocolGame::parseAutomapFlag(const InputMessagePtr &msg)
{
    msg->skipBytes(1); // TODO: understand and apply 12.x extra unknown byte
    Position pos = getPosition(msg);
    int icon = msg->getU8();
    std::string description = msg->getString();

    bool remove = false;
    if (g_game.getFeature(Otc::GameMinimapRemove))
        remove = msg->getU8() != 0;

    if (!remove)
        g_game.processAddAutomapFlag(pos, icon, description);
    else
        g_game.processRemoveAutomapFlag(pos, icon, description);
}

void ProtocolGame::parseQuestLog(const InputMessagePtr &msg)
{
    std::vector<std::tuple<int, std::string, bool>> questList;
    int questsCount = msg->getU16();
    for (int i = 0; i < questsCount; i++)
    {
        int id = msg->getU16();
        std::string name = msg->getString();
        bool completed = msg->getU8();
        questList.emplace_back(id, name, completed);
    }

    g_game.processQuestLog(questList);
}

void ProtocolGame::parseQuestLine(const InputMessagePtr &msg)
{
    std::vector<std::tuple<std::string, std::string>> questMissions;
    int questId = msg->getU16();
    int missionCount = msg->getU8();
    for (int i = 0; i < missionCount; i++)
    {
        msg->getU16(); // repeated quest ID
        std::string missionName = msg->getString();
        std::string missionDescrition = msg->getString();
        questMissions.emplace_back(missionName, missionDescrition);
    }

    g_game.processQuestLine(questId, questMissions);
}

void ProtocolGame::parseChannelEvent(const InputMessagePtr &msg)
{
    uint16 channelId = msg->getU16();
    std::string name = g_game.formatCreatureName(msg->getString());
    uint8 type = msg->getU8();

    g_lua.callGlobalField("g_game", "onChannelEvent", channelId, name, type);
}

void ProtocolGame::parseItemInfo(const InputMessagePtr &msg)
{
    std::vector<std::tuple<ItemPtr, std::string>> list;
    int size = msg->getU8();
    for (int i = 0; i < size; ++i)
    {
        ItemPtr item(new Item);
        item->setId(msg->getU16());
        item->setCountOrSubType(msg->getU8());

        std::string desc = msg->getString();
        list.emplace_back(item, desc);
    }

    g_lua.callGlobalField("g_game", "onItemInfo", list);
}

void ProtocolGame::parsePlayerInventory(const InputMessagePtr &msg)
{
    int size = msg->getU16();
    for (int i = 0; i < size; ++i)
    {
        msg->getU16(); // id
        msg->getU8();  // subtype
        msg->getU16(); // count
    }
}

void ProtocolGame::parseModalDialog(const InputMessagePtr &msg)
{
    uint32 windowId = msg->getU32();
    std::string title = msg->getString();
    std::string message = msg->getString();

    int sizeButtons = msg->getU8();
    std::vector<std::tuple<int, std::string>> buttonList;
    for (int i = 0; i < sizeButtons; ++i)
    {
        std::string value = msg->getString();
        int buttonId = msg->getU8();
        buttonList.push_back(std::make_tuple(buttonId, value));
    }

    int sizeChoices = msg->getU8();
    std::vector<std::tuple<int, std::string>> choiceList;
    for (int i = 0; i < sizeChoices; ++i)
    {
        std::string value = msg->getString();
        int choideId = msg->getU8();
        choiceList.push_back(std::make_tuple(choideId, value));
    }

    int enterButton, escapeButton;
    if (g_game.getClientVersion() > 970)
    {
        escapeButton = msg->getU8();
        enterButton = msg->getU8();
    }
    else
    {
        enterButton = msg->getU8();
        escapeButton = msg->getU8();
    }

    bool priority = msg->getU8() == 0x01;

    g_game.processModalDialog(windowId, title, message, buttonList, enterButton, escapeButton, choiceList, priority);
}

void ProtocolGame::parseExtendedOpcode(const InputMessagePtr &msg)
{
    int opcode = msg->getU8();
    std::string buffer = msg->getString();

    if (opcode == 0)
        m_enableSendExtendedOpcode = true;
    else if (opcode == 2)
        parsePingBack(msg);
    else
        callLuaField("onExtendedOpcode", opcode, buffer);
}

void ProtocolGame::parseChangeMapAwareRange(const InputMessagePtr &msg)
{
    int xrange = msg->getU8();
    int yrange = msg->getU8();

    AwareRange range;
    range.left = xrange / 2 - ((xrange + 1) % 2);
    range.right = xrange / 2;
    range.top = yrange / 2 - ((yrange + 1) % 2);
    range.bottom = yrange / 2;

    g_map.setAwareRange(range);
    g_lua.callGlobalField("g_game", "onMapChangeAwareRange", xrange, yrange);
}

void ProtocolGame::parseCreaturesMark(const InputMessagePtr &msg)
{
    int len;
    if (g_game.getClientVersion() >= 1035)
    {
        len = 1;
    }
    else
    {
        len = msg->getU8();
    }

    for (int i = 0; i < len; ++i)
    {
        uint32 id = msg->getU32();
        bool isPermanent = msg->getU8() != 1;
        uint8 markType = msg->getU8();

        CreaturePtr creature = g_map.getCreatureById(id);
        if (creature)
        {
            if (isPermanent)
            {
                if (markType == 0xff)
                    creature->hideStaticSquare();
                else
                    creature->showStaticSquare(Color::from8bit(markType));
            }
            else
                creature->addTimedSquare(markType);
        }
        else
            g_logger.traceError("could not get creature");
    }
}

void ProtocolGame::parseCreatureType(const InputMessagePtr &msg)
{
    uint32 id = msg->getU32();
    uint8 type = msg->getU8();

    if (type == Proto::CreatureTypeSummonOwn)
    {
        // TODO: implement player summon type specific features
        msg->getU32(); // master id
    }

    CreaturePtr creature = g_map.getCreatureById(id);
    if (creature)
        creature->setType(type);
    else
        g_logger.traceError("could not get creature");
}

void ProtocolGame::setMapDescription(const InputMessagePtr &msg, int x, int y, int z, int width, int height)
{
    int startz, endz, zstep;

    if (z > Otc::SEA_FLOOR)
    {
        startz = z - Otc::AWARE_UNDEGROUND_FLOOR_RANGE;
        endz = std::min<int>(z + Otc::AWARE_UNDEGROUND_FLOOR_RANGE, (int)Otc::MAX_Z);
        zstep = 1;
    }
    else
    {
        startz = Otc::SEA_FLOOR;
        endz = 0;
        zstep = -1;
    }

    int skip = 0;
    for (int nz = startz; nz != endz + zstep; nz += zstep)
        skip = setFloorDescription(msg, x, y, nz, width, height, z - nz, skip);
}

int ProtocolGame::setFloorDescription(const InputMessagePtr &msg, int x, int y, int z, int width, int height, int offset, int skip)
{
    for (int nx = 0; nx < width; nx++)
    {
        for (int ny = 0; ny < height; ny++)
        {
            Position tilePos(x + nx + offset, y + ny + offset, z);
            if (skip == 0)
                skip = setTileDescription(msg, tilePos);
            else
            {
                g_map.cleanTile(tilePos);
                skip--;
            }
        }
    }
    return skip;
}

int ProtocolGame::setTileDescription(const InputMessagePtr &msg, Position position)
{
    g_map.cleanTile(position);

    bool gotEffect = false;
    for (int stackPos = 0; stackPos < 256; stackPos++)
    {
        if (msg->peekU16() >= 0xff00)
            return msg->getU16() & 0xff;

        if (g_game.getFeature(Otc::GameEnvironmentEffect) && !gotEffect)
        {
            gotEffect = true;
            continue;
        }

        if (stackPos > 10)
            g_logger.traceError(stdext::format("too many things, pos=%s, stackpos=%d", stdext::to_string(position), stackPos));

        ThingPtr thing = getThing(msg);
        g_map.addThing(thing, position, stackPos);
    }

    return 0;
}
Outfit ProtocolGame::getOutfit(const InputMessagePtr &msg)
{
    Outfit outfit;

    uint16_t lookType = msg->getU16();

    if (lookType != 0)
    {
        outfit.setCategory(ThingCategoryCreature);
        uint8_t head = msg->getU8();
        uint8_t body = msg->getU8();
        uint8_t legs = msg->getU8();
        uint8_t feet = msg->getU8();
        uint8_t addons = msg->getU8();

        if (!g_things.isValidDatId(lookType, ThingCategoryCreature))
        {
            g_logger.traceError(stdext::format("invalid outfit looktype %d", lookType));
            lookType = 0;
        }

        outfit.setId(lookType);
        outfit.setHead(head);
        outfit.setBody(body);
        outfit.setLegs(legs);
        outfit.setFeet(feet);
        outfit.setAddons(addons);
    }
    else
    {
        int lookTypeEx = msg->getU16();
        if (lookTypeEx == 0)
        {
            outfit.setCategory(ThingCategoryEffect);
            outfit.setAuxId(13); // invisible effect id
        }
        else
        {
            if (!g_things.isValidDatId(lookTypeEx, ThingCategoryItem))
            {
                g_logger.traceError(stdext::format("invalid outfit looktypeex %d", lookTypeEx));
                lookTypeEx = 0;
            }
            outfit.setCategory(ThingCategoryItem);
            outfit.setAuxId(lookTypeEx);
        }
    }

    outfit.setMount(msg->getU16());

    return outfit;
}

ThingPtr ProtocolGame::getThing(const InputMessagePtr &msg)
{
    ThingPtr thing;

    int id = msg->getU16();

    if (id == 0)
        stdext::throw_exception("invalid thing id");
    else if (id == Proto::UnknownCreature || id == Proto::OutdatedCreature || id == Proto::Creature)
        thing = getCreature(msg, id);
    else if (id == Proto::StaticText) // otclient only
        thing = getStaticText(msg, id);
    else // item
        thing = getItem(msg, id);

    return thing;
}

ThingPtr ProtocolGame::getMappedThing(const InputMessagePtr &msg)
{
    ThingPtr thing;
    uint16 x = msg->getU16();

    if (x != 0xffff)
    {
        Position pos;
        pos.x = x;
        pos.y = msg->getU16();
        pos.z = msg->getU8();
        uint8 stackpos = msg->getU8();
        assert(stackpos != 255);
        thing = g_map.getThing(pos, stackpos);
        if (!thing)
            g_logger.traceError(stdext::format("no thing at pos:%s, stackpos:%d", stdext::to_string(pos), stackpos));
    }
    else
    {
        uint32 id = msg->getU32();
        thing = g_map.getCreatureById(id);
        if (!thing)
            g_logger.traceError(stdext::format("no creature with id %u", id));
    }

    return thing;
}

CreaturePtr ProtocolGame::getCreature(const InputMessagePtr &msg, int type)
{
    if (type == 0)
        type = msg->getU16();

    CreaturePtr creature;
    bool known = (type != Proto::UnknownCreature);
    if (type == Proto::OutdatedCreature || type == Proto::UnknownCreature)
    {
        if (known)
        {
            uint id = msg->getU32();
            creature = g_map.getCreatureById(id);
            if (!creature)
                g_logger.traceError("server said that a creature is known, but it's not");
        }
        else
        {
            uint removeId = msg->getU32();
            g_map.removeCreatureById(removeId);

            uint id = msg->getU32();

            int creatureType = msg->getU8();
            
            if (creatureType == Proto::CreatureTypeSummonOwn)
            {
                // TODO: Implement player summon creature type specific features
                msg->getU32(); // master id
                creature = MonsterPtr(new Monster);
            }

            std::string name = g_game.formatCreatureName(msg->getString());

            if (id == m_localPlayer->getId())
                creature = m_localPlayer;
            else if (creatureType == Proto::CreatureTypePlayer)
            {
                // fixes a bug server side bug where GameInit is not sent and local player id is unknown
                if (m_localPlayer->getId() == 0 && name == m_localPlayer->getName())
                    creature = m_localPlayer;
                else
                    creature = PlayerPtr(new Player);
            }
            else if (creatureType == Proto::CreatureTypeMonster)
                creature = MonsterPtr(new Monster);
            else if (creatureType == Proto::CreatureTypeNpc)
                creature = NpcPtr(new Npc);
            else if (creatureType == Proto::CreatureTypeHidden)
            {
                // TODO: Implement hidden creature type specific features
                creature = CreaturePtr(new Creature);
            }
            else
                g_logger.traceError("creature type is invalid");

            if (creature)
            {
                creature->setId(id);
                creature->setName(name);

                g_map.addCreature(creature);
            }
        }

        int healthPercent = msg->getU8();
        Otc::Direction direction = (Otc::Direction)msg->getU8();
        Outfit outfit = getOutfit(msg);

        Light light;
        light.intensity = msg->getU8();
        light.color = msg->getU8();

        int speed = msg->getU16();
        int skull = msg->getU8();
        int shield = msg->getU8();

        // emblem is sent only when the creature is not known
        int8 emblem = !known ? msg->getU8() : -1;
        int8 creatureType = msg->getU8();

        if (creatureType == Proto::CreatureTypeSummonOwn)
        {
            // TODO: Implement player summon specific features
            msg->getU32(); // master id
        }
        else if (creatureType == Proto::CreatureTypeHidden)
        {
            // TODO: Implement hidden creature type specific features
        }
        else if (creatureType == Proto::CreatureTypePlayer)
        {
            int8 vocationId = msg->getU8();
        }

        int8 icon = msg->getU8();
        uint8 mark = msg->getU8();
        msg->getU8(); // inspection byte
        bool unpass = msg->getU8();

        if (creature)
        {
            if (mark == 0xff)
                creature->hideStaticSquare();
            else
                creature->showStaticSquare(Color::from8bit(mark));
        }

        if (creature)
        {
            creature->setHealthPercent(healthPercent);
            creature->setDirection(direction);
            creature->setOutfit(outfit);
            creature->setSpeed(speed);
            creature->setSkull(skull);
            creature->setShield(shield);
            creature->setPassable(!unpass);
            creature->setLight(light);

            if (emblem != -1)
                creature->setEmblem(emblem);

            if (creatureType != -1)
                creature->setType(creatureType);

            if (icon != -1)
                creature->setIcon(icon);

            if (creature == m_localPlayer && !m_localPlayer->isKnown())
                m_localPlayer->setKnown(true);
        }
    }
    else if (type == Proto::Creature)
    {
        uint id = msg->getU32();
        creature = g_map.getCreatureById(id);

        if (!creature)
            g_logger.traceError("invalid creature");

        Otc::Direction direction = (Otc::Direction)msg->getU8();
        bool unpass = msg->getU8();
        if (creature) {
            creature->turn(direction);
            creature->setPassable(!unpass);
        }
    }
    else
    {
        stdext::throw_exception("invalid creature opcode");
    }

    return creature;
}

ItemPtr ProtocolGame::getItem(const InputMessagePtr &msg, int id)
{
    if (id == 0)
        id = msg->getU16();

    ItemPtr item = Item::create(id);
    if (item->getId() == 0)
        stdext::throw_exception(stdext::format("unable to create item with invalid id %d", id));

    if (item->isStackable() || item->isFluidContainer() || item->isSplash() || item->isChargeable())
        item->setCountOrSubType(msg->getU8());

    if (item->getAnimationPhases() > 1)
    {
        // 0x00 => automatic phase
        // 0xFE => random phase
        // 0xFF => async phase
        msg->getU8();
        //item->setPhase(msg->getU8());
    }

    return item;
}

StaticTextPtr ProtocolGame::getStaticText(const InputMessagePtr &msg, int id)
{
    int colorByte = msg->getU8();
    Color color = Color::from8bit(colorByte);
    std::string fontName = msg->getString();
    std::string text = msg->getString();
    StaticTextPtr staticText = StaticTextPtr(new StaticText);
    staticText->setText(text);
    staticText->setFont(fontName);
    staticText->setColor(color);
    return staticText;
}

Position ProtocolGame::getPosition(const InputMessagePtr &msg)
{
    uint16 x = msg->getU16();
    uint16 y = msg->getU16();
    uint8 z = msg->getU8();

    return Position(x, y, z);
}

// 12.x +
void ProtocolGame::parseClientCheck(const InputMessagePtr& msg) {
    msg->getU32(); // 1
    msg->getU8(); // 1
}

void ProtocolGame::parseGameNews(const InputMessagePtr& msg) {
    msg->getU32(); // 1
    msg->getU8(); // 1

    // TODO: implement game news usage
}

void ProtocolGame::parseBlessDialog(const InputMessagePtr& msg) {
    // parse bless amount
    uint8_t totalBless = msg->getU8(); // total bless

    // parse each bless
    for (int i = 0; i < totalBless; i++) {
        msg->getU16(); // bless bit wise
        msg->getU8(); // player bless count
    }
    
    // parse general info
    msg->getU8(); // premium
    msg->getU8(); // promotion
    msg->getU8(); // pvp min xp loss
    msg->getU8(); // pvp max xp loss
    msg->getU8(); // pve exp loss
    msg->getU8(); // equip pvp loss
    msg->getU8(); // equip pve loss
    msg->getU8(); // skull
    msg->getU8(); // aol

    // parse log
    uint8_t logCount = msg->getU8(); // log count
    for (int i = 0; i < logCount; i++) {
        msg->getU32(); // timestamp
        msg->getU8(); // color message (0 = white loss, 1 = red)
        msg->getString(); // history message
    }

    // TODO: implement bless dialog usage
}

void ProtocolGame::parseRestingAreaState(const InputMessagePtr& msg) {
    msg->getU8(); // zone
    msg->getU8(); // state
    msg->getString(); // message

    // TODO: implement resting area state usage
}

void ProtocolGame::parseUpdateImpactTracker(const InputMessagePtr& msg) {
    msg->getU8(); // is heal
    msg->getU32(); // amount
    
    // TODO: implement impact tracker usage
}

void ProtocolGame::parseItemsPrice(const InputMessagePtr& msg) {
    uint16_t priceCount = msg->getU16(); // count

    for (int i = 0; i < priceCount; i++) {
        msg->getU16(); // item client id
        msg->getU32(); // price
    }

    // TODO: implement items price usage
}

void ProtocolGame::parseUpdateSupplyTracker(const InputMessagePtr& msg) {
    msg->getU16(); // item client ID

    // TODO: implement supply tracker usage
}

void ProtocolGame::parseUpdateLootTracker(const InputMessagePtr& msg) {
    getItem(msg); // item
    msg->getString(); // item name
    
    // TODO: implement loot tracker usage
}

void ProtocolGame::parseKillTrackerUpdate(const InputMessagePtr& msg) {
    msg->getString(); // creature name
    
    msg->getU16(); // creature looktype
    msg->getU8(); // head
    msg->getU8(); // body
    msg->getU8(); // legs
    msg->getU8(); // feet
    msg->getU8(); // addons

    uint8_t corpseSize = msg->getU8(); // corpse size

    for (int i = 0; i < corpseSize; i++) {
        getItem(msg); // corpse item
    }
    
    // TODO: implement kill tracker usage
}

void ProtocolGame::parseBestiaryEntryChanged(const InputMessagePtr& msg) {
    msg->getU16(); // monster ID

    // TODO: implement bestiary entry changed usage
}

void ProtocolGame::parseDailyRewardCollectionState(const InputMessagePtr& msg) {
    msg->getU8(); // state

    // TODO: implement daily reward collection state usage
}

void ProtocolGame::parseOpenRewardWall(const InputMessagePtr& msg) {
    msg->getU8(); // bonus shrine (1) or instant bonus (0)
    msg->getU32(); // next reward time
    msg->getU8(); // day streak day
    uint8_t wasDailyRewardTaken = msg->getU8(); // taken (player already took reward?)

    if (wasDailyRewardTaken) {
        msg->getString(); // error message
    }

    msg->getU32(); // time left to pickup reward without loosing streak
    msg->getU16(); // day streak level
    msg->getU16(); // unknown

    // TODO: implement open reward wall usage
}

void ProtocolGame::parseDailyReward(const InputMessagePtr& msg) {
    msg->getU8(); // state

    // TODO: implement daily reward usage
}

void ProtocolGame::parseRewardHistory(const InputMessagePtr& msg) {
    uint8_t historyCount = msg->getU8(); // history count

    for (int i = 0; i < historyCount; i++) {
        msg->getU32(); // timestamp
        msg->getU8(); // is Premium
        msg->getString(); // description
        msg->getU16(); // daystreak
    }

    // TODO: implement reward history usage
}
void ProtocolGame::parsePreyTimeLeft(const InputMessagePtr& msg) {
    // TODO: implement protocol parse
}

void ProtocolGame::getPreyMonster(const InputMessagePtr& msg) {
    msg->getString(); // mosnter name
    msg->getU16(); // looktype
    msg->getU8(); // head
    msg->getU8(); // body
    msg->getU8(); // legs
    msg->getU8(); // feet
    msg->getU8(); // addons
}

void ProtocolGame::getPreyMonsters(const InputMessagePtr& msg) {
    uint8_t monstersSize = msg->getU8(); // monster list size
    for (uint8_t i = 0; i < monstersSize; i++) 
        getPreyMonster(msg);
}

void ProtocolGame::parsePreyData(const InputMessagePtr& msg) {
    msg->getU8(); // slot
    uint8_t slotState = msg->getU8(); // slot state

    // if is initialization, get empty stuff
    if (msg->peekU8() == 0x00) {
        msg->getU8();
    } else {
        switch(slotState) {
            case Otc::PREY_STATE_LOCKED:
                msg->getU8(); // prey slot unlocked
                break;
            case Otc::PREY_STATE_INACTIVE:
                break;
            case Otc::PREY_STATE_ACTIVE:
                getPreyMonster(msg);
                msg->getU8(); // bonus type
                msg->getU16(); // bonus value
                msg->getU8(); // bonus grade
                msg->getU16(); // time left
                break;
            case Otc::PREY_STATE_SELECTION:
                getPreyMonsters(msg);
                break;
            case Otc::PREY_STATE_SELECTION_CHANGE_MONSTER:
                msg->getU8(); // bonus type
                msg->getU16(); // bonus value
                msg->getU8(); // bonus grade
                getPreyMonsters(msg);
                break;
        }
    }

    msg->getU16(); // next free roll
    msg->getU8(); // wildcards
    
    // TODO: implement prey data usage
}

void ProtocolGame::parsePreyRerollPrice(const InputMessagePtr& msg) {
    msg->getU32(); // reroll price
    msg->getU8(); // wildcard
    msg->getU8(); // select directly
    // TODO: implement prey reroll price usage
}

void ProtocolGame::getImbuementInfo(const InputMessagePtr& msg) {
    msg->getU32(); // imbuid
    msg->getString(); // name
    msg->getString(); // description
    msg->getString(); // subgroup

    msg->getU16(); // iconId
    msg->getU32(); // duration

    msg->getU8(); // is premium

    uint8_t itemsSize = msg->getU8(); // items size
    for (uint8_t i = 0; i < itemsSize; i++) {
        msg->getU16(); // item client ID
        msg->getString(); // item name
        msg->getU16(); // count
    }

    msg->getU32(); // base price
    msg->getU8(); // base percent
    msg->getU32(); // base protection
}

void ProtocolGame::parseImbuementWindow(const InputMessagePtr& msg) {
    msg->getU16(); // item client ID
    uint8_t slot = msg->getU8(); // slot id 

    for (uint8_t i = 0; i < slot; i++) {
        uint8_t firstByte = msg->getU8();
        if (firstByte == 0x01) {
            getImbuementInfo(msg);
            msg->getU32(); // info >> 8
            msg->getU32(); // removecust
        }
    }

    uint16_t imbSize = msg->getU16(); // imbuement size
    for (uint16 i = 0; i < imbSize; i++) {
        getImbuementInfo(msg);
    }

    uint32_t neededItemsSize = msg->getU32(); // needed items size
    for (uint32_t i = 0; i < neededItemsSize; i++) {
        msg->getU16(); // item client id
        msg->getU16(); // item count
    }

    // TODO: implement imbuement window usage
}

void ProtocolGame::parseCloseImbuementWindow(const InputMessagePtr& msg) {
    // TODO: implement close imbuement window usage
}

void ProtocolGame::parseError(const InputMessagePtr& msg) {
    msg->getU8(); // error code
    msg->getString(); // error

    // TODO: implement error usage
}

void ProtocolGame::parseCollectionResource(const InputMessagePtr& msg) {
    msg->getU8(); // id (0x1 bank, 0x2 inventory, 0xA prey)
    msg->getU64(); // resource value

    // TODO: implement collection resource usage
}

void ProtocolGame::parseTibiaTime(const InputMessagePtr& msg) {
    msg->getU8(); // hours
    msg->getU8(); // minutes
    
    // TODO: implement tibia time usage
}
