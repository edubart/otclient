/*
* Copyright (c) 2010-2022 OTClient <https://github.com/edubart/otclient>
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

#include "effect.h"
#include "framework/net/inputmessage.h"
#include "game.h"
#include "item.h"
#include "localplayer.h"
#include "luavaluecasts.h"
#include "map.h"
#include "missile.h"
#include "statictext.h"
#include "thingtypemanager.h"
#include "tile.h"
#include <framework/core/eventdispatcher.h>

void ProtocolGame::parseMessage(const InputMessagePtr& msg)
{
    int opcode = -1;
    int prevOpcode = -1;

    try {
        while (!msg->eof()) {
            opcode = msg->getU8();

            // must be > so extended will be enabled before GameStart.
            if (!g_game.getFeature(Otc::GameLoginPending)) {
                if (!m_gameInitialized && opcode > Proto::GameServerFirstGameOpcode) {
                    g_game.processGameStart();
                    m_gameInitialized = true;
                }
            }

            // try to parse in lua first
            const int readPos = msg->getReadPos();
            if (callLuaField<bool>("onOpcode", opcode, msg))
                continue;
            msg->setReadPos(readPos);
            // restore read pos

            switch (opcode) {
                case Proto::GameServerLoginOrPendingState:
                    if (g_game.getFeature(Otc::GameLoginPending))
                        parsePendingGame(msg);
                    else
                        parseLogin(msg);
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
                case Proto::GameServerSessionEnd:
                    parseSessionEnd(msg);
                    break;
                case Proto::GameServerPing:
                case Proto::GameServerPingBack:
                    if (((opcode == Proto::GameServerPing) && (g_game.getFeature(Otc::GameClientPing))) ||
                       ((opcode == Proto::GameServerPingBack) && !g_game.getFeature(Otc::GameClientPing)))
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
                case Proto::GameServerItemClasses:
                    if (g_game.getClientVersion() >= 1281)
                        parseItemClasses(msg);
                    else
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
                    if (g_game.getClientVersion() >= 1200)
                        parseExperienceTracker(msg);
                    else
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
                case Proto::GameServerKillTracker:
                    parseKillTracker(msg);
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
                case Proto::GameServerPlayerHelpers:
                    parsePlayerHelpers(msg);
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
                case Proto::GameServerResourceBalance: // 1281
                    parseResourceBalance(msg);
                    break;
                case Proto::GameServerWorldTime:
                    parseWorldTime(msg);
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
                    // 12x
                case Proto::GameServerLootContainers:
                    parseLootContainers(msg);
                    break;
                case Proto::GameServerSupplyStash:
                    parseSupplyStash(msg);
                    break;
                case Proto::GameServerSpecialContainer:
                    parseSpecialContainer(msg);
                    break;
                case Proto::GameServerPartyAnalyzer:
                    parsePartyAnalyzer(msg);
                    break;
                case Proto::GameServerRefreshBestiaryTracker:
                    parseBestiaryTracker(msg);
                    break;
                case Proto::GameServerTaskHuntingBasicData:
                    parseTaskHuntingBasicData(msg);
                    break;
                case Proto::GameServerTaskHuntingData:
                    parseTaskHuntingData(msg);
                    break;
                case Proto::GameServerSendShowDescription:
                    parseShowDescription(msg);
                    break;
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
                default:
                    stdext::throw_exception(stdext::format("unhandled opcode %d", opcode));
                    break;
            }
            prevOpcode = opcode;
        }
    } catch (stdext::exception& e) {
        g_logger.error(stdext::format("ProtocolGame parse message exception (%d bytes unread, last opcode is %d, prev opcode is %d): %s",
                                      msg->getUnreadSize(), opcode, prevOpcode, e.what()));
    }
}

void ProtocolGame::parseLogin(const InputMessagePtr& msg)
{
    const uint playerId = msg->getU32();
    const int serverBeat = msg->getU16();

    if (g_game.getFeature(Otc::GameNewSpeedLaw)) {
        Creature::speedA = msg->getDouble();
        Creature::speedB = msg->getDouble();
        Creature::speedC = msg->getDouble();
    }

    const bool canReportBugs = msg->getU8();

    if (g_game.getClientVersion() >= 1054)
        msg->getU8(); // can change pvp frame option

    if (g_game.getClientVersion() >= 1058) {
        const int expertModeEnabled = msg->getU8();
        g_game.setExpertPvpMode(expertModeEnabled);
    }

    if (g_game.getFeature(Otc::GameIngameStore)) {
        // URL to ingame store images
        msg->getString();

        // premium coin package size
        // e.g you can only buy packs of 25, 50, 75, .. coins in the market
        msg->getU16();
    }

    if (g_game.getClientVersion() >= 1281) {
        msg->getU8(); // exiva button enabled (bool)
        msg->getU8(); // Tournament button (bool)
    }

    m_localPlayer->setId(playerId);
    g_game.setServerBeat(serverBeat);
    g_game.setCanReportBugs(canReportBugs);

    Game::processLogin();
}

void ProtocolGame::parsePendingGame(const InputMessagePtr&)
{
    //set player to pending game state
    g_game.processPendingGame();
}

void ProtocolGame::parseEnterGame(const InputMessagePtr&)
{
    //set player to entered game state
    g_game.processEnterGame();

    if (!m_gameInitialized) {
        g_game.processGameStart();
        m_gameInitialized = true;
    }
}

void ProtocolGame::parseStoreButtonIndicators(const InputMessagePtr& msg)
{
    msg->getU8(); // unknown
    msg->getU8(); // unknown
}

void ProtocolGame::parseSetStoreDeepLink(const InputMessagePtr& msg)
{
    msg->getU8(); // currentlyFeaturedServiceType
}

void ProtocolGame::parseBlessings(const InputMessagePtr& msg)
{
    const uint16 blessings = msg->getU16();
    if (g_game.getClientVersion() >= 1200) {
        msg->getU8(); // Blessing count
    }
    m_localPlayer->setBlessings(blessings);
}

void ProtocolGame::parsePreset(const InputMessagePtr& msg)
{
    msg->getU32(); // preset
}

void ProtocolGame::parseRequestPurchaseData(const InputMessagePtr& msg)
{
    msg->getU32(); // transactionId
    msg->getU8(); // productType
}

void ProtocolGame::parseResourceBalance(const InputMessagePtr& msg)
{
    const auto type = static_cast<Otc::ResourceTypes_t>(msg->getU8());
    const uint64_t value = msg->getU64();
    m_localPlayer->setResourceBalance(type, value);
}

void ProtocolGame::parseWorldTime(const InputMessagePtr& msg)
{
    msg->getU8(); // hour
    msg->getU8(); // min
}

void ProtocolGame::parseStore(const InputMessagePtr& msg)
{
    parseCoinBalance(msg);

    const int categories = msg->getU16();
    for (int i = 0; i < categories; ++i) {
        const auto category = msg->getString();
        const auto description = msg->getString();

        if (g_game.getFeature(Otc::GameIngameStoreHighlights))
            msg->getU8(); // highlightState

        std::vector<std::string_view> icons;
        const int iconCount = msg->getU8();
        for (int j = 0; j < iconCount; ++j) {
            icons.push_back(msg->getString());
        }

        // If this is a valid category name then
        // the category we just parsed is a child of that
        msg->getString();
    }
}

void ProtocolGame::parseCoinBalance(const InputMessagePtr& msg)
{
    const bool update = msg->getU8() == 1;
    if (update) {
        // amount of coins that can be used to buy prodcuts
        // in the ingame store
        msg->getU32(); // coins

        // amount of coins that can be sold in market
        // or be transfered to another player
        msg->getU32(); // transferableCoins
    }
}

void ProtocolGame::parseCoinBalanceUpdating(const InputMessagePtr& msg)
{
    // coin balance can be updating and might not be accurate
    msg->getU8(); // == 1; // isUpdating
}

void ProtocolGame::parseCompleteStorePurchase(const InputMessagePtr& msg)
{
    // not used
    msg->getU8();

    const auto message = msg->getString();
    const int coins = msg->getU32();
    const int transferableCoins = msg->getU32();

    g_logger.info(stdext::format("Purchase Complete: %s\nAvailable coins: %d (transferable: %d)", message, coins, transferableCoins));
}

void ProtocolGame::parseStoreTransactionHistory(const InputMessagePtr& msg)
{
    int currentPage;
    if (g_game.getClientVersion() <= 1096) {
        currentPage = msg->getU16();
        msg->getU8(); // hasNextPage (bool)
    } else {
        currentPage = msg->getU32();
        msg->getU32(); // pageCount
    }

    const int entries = msg->getU8();
    for (int i = 0; i < entries; ++i) {
        int time = msg->getU16();
        int productType = msg->getU8();
        int coinChange = msg->getU32();
        const auto productName = msg->getString();
        g_logger.error(stdext::format("Time %i, type %i, change %i, product name %s", time, productType, coinChange, productName));
    }
}

void ProtocolGame::parseStoreOffers(const InputMessagePtr& msg)
{
    msg->getString(); // categoryName

    const int offers = msg->getU16();
    for (int i = 0; i < offers; ++i) {
        msg->getU32(); // offerId
        msg->getString(); // offerName
        msg->getString(); // offerDescription

        msg->getU32(); // price
        const int highlightState = msg->getU8();
        if (highlightState == 2 && g_game.getFeature(Otc::GameIngameStoreHighlights) && g_game.getClientVersion() >= 1097) {
            msg->getU32(); // saleValidUntilTimestamp
            msg->getU32(); // basePrice
        }

        const int disabledState = msg->getU8();
        if (g_game.getFeature(Otc::GameIngameStoreHighlights) && disabledState == 1) {
            msg->getString(); // disabledReason
        }

        std::vector<std::string_view> icons;
        const int iconCount = msg->getU8();
        for (int j = 0; j < iconCount; ++j) {
            icons.push_back(msg->getString());
        }

        const int subOffers = msg->getU16();
        for (int j = 0; j < subOffers; ++j) {
            msg->getString(); // name
            msg->getString(); // description

            const int subIcons = msg->getU8();
            for (int k = 0; k < subIcons; k++) {
                msg->getString(); // icon
            }
            msg->getString(); // serviceType
        }
    }
}

void ProtocolGame::parseStoreError(const InputMessagePtr& msg)
{
    const int errorType = msg->getU8();
    const auto message = msg->getString();
    g_logger.error(stdext::format("Store Error: %s [%i]", message, errorType));
}

void ProtocolGame::parseUnjustifiedStats(const InputMessagePtr& msg)
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

void ProtocolGame::parsePvpSituations(const InputMessagePtr& msg)
{
    const uint8 openPvpSituations = msg->getU8();

    g_game.setOpenPvpSituations(openPvpSituations);
}

void ProtocolGame::parsePlayerHelpers(const InputMessagePtr& msg)
{
    const uint id = msg->getU32();
    const int helpers = msg->getU16();

    const CreaturePtr creature = g_map.getCreatureById(id);
    if (creature)
        Game::processPlayerHelpers(helpers);
    else
        g_logger.traceError(stdext::format("could not get creature with id %d", id));
}

void ProtocolGame::parseGMActions(const InputMessagePtr& msg)
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

void ProtocolGame::parseUpdateNeeded(const InputMessagePtr& msg)
{
    const auto signature = msg->getString();
    Game::processUpdateNeeded(signature);
}

void ProtocolGame::parseLoginError(const InputMessagePtr& msg)
{
    const auto error = msg->getString();

    Game::processLoginError(error);
}

void ProtocolGame::parseLoginAdvice(const InputMessagePtr& msg)
{
    const auto message = msg->getString();

    Game::processLoginAdvice(message);
}

void ProtocolGame::parseLoginWait(const InputMessagePtr& msg)
{
    const auto message = msg->getString();
    const int time = msg->getU8();

    Game::processLoginWait(message, time);
}

void ProtocolGame::parseSessionEnd(const InputMessagePtr& msg)
{
    const int reason = msg->getU8();
    Game::processSessionEnd(reason);
}

void ProtocolGame::parsePing(const InputMessagePtr&)
{
    g_game.processPing();
}

void ProtocolGame::parsePingBack(const InputMessagePtr&)
{
    g_game.processPingBack();
}

void ProtocolGame::parseChallenge(const InputMessagePtr& msg)
{
    const uint timestamp = msg->getU32();
    const uint8 random = msg->getU8();

    sendLoginPacket(timestamp, random);
}

void ProtocolGame::parseDeath(const InputMessagePtr& msg)
{
    int penality = 100;
    int deathType = Otc::DeathRegular;

    if (g_game.getFeature(Otc::GameDeathType))
        deathType = msg->getU8();

    if (g_game.getFeature(Otc::GamePenalityOnDeath) && deathType == Otc::DeathRegular)
        penality = msg->getU8();

    if (g_game.getClientVersion() >= 1281) {
        msg->getU8(); // can use death redemption (bool)
    }

    g_game.processDeath(deathType, penality);
}

void ProtocolGame::parseMapDescription(const InputMessagePtr& msg)
{
    const Position pos = getPosition(msg);

    if (!m_mapKnown)
        m_localPlayer->setPosition(pos);

    g_map.setCentralPosition(pos);

    AwareRange range = g_map.getAwareRange();
    setMapDescription(msg, pos.x - range.left, pos.y - range.top, pos.z, range.horizontal(), range.vertical());

    if (!m_mapKnown) {
        g_dispatcher.addEvent([] { g_lua.callGlobalField("g_game", "onMapKnown"); });
        m_mapKnown = true;
    }

    g_dispatcher.addEvent([] { g_lua.callGlobalField("g_game", "onMapDescription"); });
}

void ProtocolGame::parseMapMoveNorth(const InputMessagePtr& msg)
{
    Position pos;
    if (g_game.getFeature(Otc::GameMapMovePosition))
        pos = getPosition(msg);
    else
        pos = g_map.getCentralPosition();
    --pos.y;

    AwareRange range = g_map.getAwareRange();
    setMapDescription(msg, pos.x - range.left, pos.y - range.top, pos.z, range.horizontal(), 1);
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseMapMoveEast(const InputMessagePtr& msg)
{
    Position pos;
    if (g_game.getFeature(Otc::GameMapMovePosition))
        pos = getPosition(msg);
    else
        pos = g_map.getCentralPosition();
    ++pos.x;

    AwareRange range = g_map.getAwareRange();
    setMapDescription(msg, pos.x + range.right, pos.y - range.top, pos.z, 1, range.vertical());
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseMapMoveSouth(const InputMessagePtr& msg)
{
    Position pos;
    if (g_game.getFeature(Otc::GameMapMovePosition))
        pos = getPosition(msg);
    else
        pos = g_map.getCentralPosition();

    ++pos.y;

    AwareRange range = g_map.getAwareRange();
    setMapDescription(msg, pos.x - range.left, pos.y + range.bottom, pos.z, range.horizontal(), 1);
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseMapMoveWest(const InputMessagePtr& msg)
{
    Position pos;
    if (g_game.getFeature(Otc::GameMapMovePosition))
        pos = getPosition(msg);
    else
        pos = g_map.getCentralPosition();
    --pos.x;

    AwareRange range = g_map.getAwareRange();
    setMapDescription(msg, pos.x - range.left, pos.y - range.top, pos.z, 1, range.vertical());
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseUpdateTile(const InputMessagePtr& msg)
{
    const Position tilePos = getPosition(msg);
    setTileDescription(msg, tilePos);
}

void ProtocolGame::parseTileAddThing(const InputMessagePtr& msg)
{
    const Position pos = getPosition(msg);
    int stackPos = -1;

    if (g_game.getClientVersion() >= 841)
        stackPos = msg->getU8();

    const ThingPtr thing = getThing(msg);
    g_map.addThing(thing, pos, stackPos);
}

void ProtocolGame::parseTileTransformThing(const InputMessagePtr& msg)
{
    const ThingPtr thing = getMappedThing(msg);
    const ThingPtr newThing = getThing(msg);

    if (!thing) {
        g_logger.traceError("no thing");
        return;
    }

    const Position pos = thing->getPosition();
    const int stackpos = thing->getStackPos();

    if (!g_map.removeThing(thing)) {
        g_logger.traceError("unable to remove thing");
        return;
    }

    g_map.addThing(newThing, pos, stackpos);
}

void ProtocolGame::parseTileRemoveThing(const InputMessagePtr& msg)
{
    const ThingPtr thing = getMappedThing(msg);
    if (!thing) {
        g_logger.traceError("no thing");
        return;
    }

    if (!g_map.removeThing(thing))
        g_logger.traceError("unable to remove thing");
}

void ProtocolGame::parseCreatureMove(const InputMessagePtr& msg)
{
    const ThingPtr thing = getMappedThing(msg);
    const Position newPos = getPosition(msg);

    if (!thing || !thing->isCreature()) {
        g_logger.traceError("no creature found to move");
        return;
    }

    if (!g_map.removeThing(thing)) {
        g_logger.traceError("unable to remove creature");
        return;
    }

    const CreaturePtr creature = thing->static_self_cast<Creature>();
    creature->allowAppearWalk();

    g_map.addThing(thing, newPos, -1);
}

void ProtocolGame::parseOpenContainer(const InputMessagePtr& msg)
{
    const int containerId = msg->getU8();
    const ItemPtr containerItem = getItem(msg);
    const auto name = msg->getString();
    const int capacity = msg->getU8();
    const bool hasParent = msg->getU8() != 0;

    bool isUnlocked = true;
    bool hasPages = false;
    int containerSize = 0;
    int firstIndex = 0;

    if (g_game.getClientVersion() >= 1281) {
        msg->getU8(); // show search icon (boolean)
    }

    if (g_game.getFeature(Otc::GameContainerPagination)) {
        isUnlocked = msg->getU8() != 0; // drag and drop
        hasPages = msg->getU8() != 0; // pagination
        containerSize = msg->getU16(); // container size
        firstIndex = msg->getU16(); // first index
    }

    const int itemCount = msg->getU8();

    std::vector<ItemPtr> items(itemCount);
    for (int i = 0; i < itemCount; ++i)
        items[i] = getItem(msg);

    g_game.processOpenContainer(containerId, containerItem, name, capacity, hasParent, items, isUnlocked, hasPages, containerSize, firstIndex);
}

void ProtocolGame::parseCloseContainer(const InputMessagePtr& msg)
{
    const int containerId = msg->getU8();
    g_game.processCloseContainer(containerId);
}

void ProtocolGame::parseContainerAddItem(const InputMessagePtr& msg)
{
    const int containerId = msg->getU8();
    int slot = 0;
    if (g_game.getFeature(Otc::GameContainerPagination)) {
        slot = msg->getU16(); // slot
    }
    const ItemPtr item = getItem(msg);
    g_game.processContainerAddItem(containerId, item, slot);
}

void ProtocolGame::parseContainerUpdateItem(const InputMessagePtr& msg)
{
    const int containerId = msg->getU8();
    int slot;
    if (g_game.getFeature(Otc::GameContainerPagination)) {
        slot = msg->getU16();
    } else {
        slot = msg->getU8();
    }
    const ItemPtr item = getItem(msg);
    g_game.processContainerUpdateItem(containerId, slot, item);
}

void ProtocolGame::parseContainerRemoveItem(const InputMessagePtr& msg)
{
    const int containerId = msg->getU8();
    int slot;
    ItemPtr lastItem;
    if (g_game.getFeature(Otc::GameContainerPagination)) {
        slot = msg->getU16();

        const int itemId = msg->getU16();
        if (itemId != 0)
            lastItem = getItem(msg, itemId);
    } else {
        slot = msg->getU8();
    }
    g_game.processContainerRemoveItem(containerId, slot, lastItem);
}

void ProtocolGame::parseAddInventoryItem(const InputMessagePtr& msg)
{
    const int slot = msg->getU8();
    const ItemPtr item = getItem(msg);
    g_game.processInventoryChange(slot, item);
}

void ProtocolGame::parseRemoveInventoryItem(const InputMessagePtr& msg)
{
    const int slot = msg->getU8();
    g_game.processInventoryChange(slot, ItemPtr());
}

void ProtocolGame::parseOpenNpcTrade(const InputMessagePtr& msg)
{
    std::vector<std::tuple<ItemPtr, std::string, int, int, int>> items;

    if (g_game.getFeature(Otc::GameNameOnNpcTrade))
        const auto npcName = msg->getString();

    if (g_game.getClientVersion() >= 1281) {
        msg->getU16(); // currency
        msg->getString(); // currency name
    }

    int listCount;

    if (g_game.getClientVersion() >= 900)
        listCount = msg->getU16();
    else
        listCount = msg->getU8();

    for (int i = 0; i < listCount; ++i) {
        const uint16 itemId = msg->getU16();
        const uint8 count = msg->getU8();

        ItemPtr item = Item::create(itemId);
        item->setCountOrSubType(count);

        const auto name = msg->getString();
        int weight = msg->getU32();
        int buyPrice = msg->getU32();
        int sellPrice = msg->getU32();
        items.emplace_back(item, name, weight, buyPrice, sellPrice);
    }

    Game::processOpenNpcTrade(items);
}

void ProtocolGame::parsePlayerGoods(const InputMessagePtr& msg)
{
    std::vector<std::tuple<ItemPtr, int>> goods;

    // 12.x NOTE: this u64 is parsed only, because TFS stil sends it, we use resource balance in this protocol
    int money = 0;
    if (g_game.getClientVersion() >= 973)
        money = msg->getU64();
    else
        money = msg->getU32();

    if (g_game.getClientVersion() >= 1281) {
        money = m_localPlayer->getResourceBalance(Otc::RESOURCE_BANK_BALANCE) + m_localPlayer->getResourceBalance(Otc::RESOURCE_GOLD_EQUIPPED);
    }

    const int size = msg->getU8();
    for (int i = 0; i < size; ++i) {
        const int itemId = msg->getU16();
        int amount;

        if (g_game.getFeature(Otc::GameDoubleShopSellAmount))
            amount = msg->getU16();
        else
            amount = msg->getU8();

        goods.emplace_back(Item::create(itemId), amount);
    }

    Game::processPlayerGoods(money, goods);
}

void ProtocolGame::parseCloseNpcTrade(const InputMessagePtr&)
{
    Game::processCloseNpcTrade();
}

void ProtocolGame::parseOwnTrade(const InputMessagePtr& msg)
{
    const auto name = g_game.formatCreatureName(msg->getString());
    const int count = msg->getU8();

    std::vector<ItemPtr> items(count);
    for (int i = 0; i < count; ++i)
        items[i] = getItem(msg);

    Game::processOwnTrade(name, items);
}

void ProtocolGame::parseCounterTrade(const InputMessagePtr& msg)
{
    const auto name = g_game.formatCreatureName(msg->getString());
    const int count = msg->getU8();

    std::vector<ItemPtr> items(count);
    for (int i = 0; i < count; ++i)
        items[i] = getItem(msg);

    Game::processCounterTrade(name, items);
}

void ProtocolGame::parseCloseTrade(const InputMessagePtr&)
{
    Game::processCloseTrade();
}

void ProtocolGame::parseWorldLight(const InputMessagePtr& msg)
{
    Light light;
    light.intensity = msg->getU8();
    light.color = msg->getU8();

    g_map.setLight(light);
}

void ProtocolGame::parseMagicEffect(const InputMessagePtr& msg)
{
    const Position pos = getPosition(msg);
    Position toPos;

    Otc::MagicEffectsType_t type = Otc::MAGIC_EFFECTS_CREATE_EFFECT;

    if (g_game.getClientVersion() >= 1281) {
        type = static_cast<Otc::MagicEffectsType_t>(msg->getU8()); // type
    }

    int effectId;
    if (g_game.getFeature(Otc::GameMagicEffectU16))
        effectId = msg->getU16();
    else
        effectId = msg->getU8();

    if (g_game.getClientVersion() <= 750)
        effectId += 1; //hack to fix effects in earlier clients

    if (!g_things.isValidDatId(effectId, ThingCategoryEffect)) {
        g_logger.traceError(stdext::format("invalid effect id %d", effectId));
        return;
    }

    if (g_game.getClientVersion() >= 1281) {
        if (type == Otc::MAGIC_EFFECTS_CREATE_DISTANCEEFFECT) {
            const int8_t x = msg->getU8();
            const int8_t y = msg->getU8();
            toPos = Position(pos.x + x, pos.y + y, pos.z);
        }

        msg->getU8(); // end loop
    }

    if (type == Otc::MAGIC_EFFECTS_CREATE_DISTANCEEFFECT) {
        const auto missile = MissilePtr(new Missile());
        missile->setId(effectId);
        missile->setPath(pos, toPos);
        g_map.addThing(missile, pos);
    } else {
        const auto effect = EffectPtr(new Effect());
        effect->setId(effectId);
        g_map.addThing(effect, pos);
    }
}

void ProtocolGame::parseAnimatedText(const InputMessagePtr& msg)
{
    const Position& position = getPosition(msg);
    const int color = msg->getU8();
    const auto text = msg->getString();

    const auto animatedText = AnimatedTextPtr(new AnimatedText);
    animatedText->setColor(color);
    animatedText->setText(text);
    g_map.addThing(animatedText, position);
}

void ProtocolGame::parseDistanceMissile(const InputMessagePtr& msg)
{
    const Position fromPos = getPosition(msg);
    const Position toPos = getPosition(msg);

    int shotId;
    if (g_game.getFeature(Otc::GameDistanceEffectU16))
        shotId = msg->getU16();
    else
        shotId = msg->getU8();

    if (!g_things.isValidDatId(shotId, ThingCategoryMissile)) {
        g_logger.traceError(stdext::format("invalid missile id %d", shotId));
        return;
    }

    const auto missile = MissilePtr(new Missile());
    missile->setId(shotId);
    missile->setPath(fromPos, toPos);
    g_map.addThing(missile, fromPos);
}

void ProtocolGame::parseItemClasses(const InputMessagePtr& msg)
{
    const uint8_t classSize = msg->getU8();
    for (uint8_t i = 0; i < classSize; i++) {
        msg->getU8(); // class id

        // tiers
        const uint8_t tiersSize = msg->getU8();
        for (uint8_t j = 0; j < tiersSize; j++) {
            msg->getU8(); // tier id
            msg->getU64(); // upgrade cost
        }
    }

    for (uint8_t i = 1; i <= 11; i++) {
        msg->getU8(); // Forge values
    }
}

void ProtocolGame::parseCreatureMark(const InputMessagePtr& msg)
{
    const uint id = msg->getU32();
    const int color = msg->getU8();

    const CreaturePtr creature = g_map.getCreatureById(id);
    if (creature)
        creature->addTimedSquare(color);
    else
        g_logger.traceError("could not get creature");
}

void ProtocolGame::parseTrappers(const InputMessagePtr& msg)
{
    const int numTrappers = msg->getU8();

    if (numTrappers > 8)
        g_logger.traceError("too many trappers");

    for (int i = 0; i < numTrappers; ++i) {
        const uint id = msg->getU32();
        CreaturePtr creature = g_map.getCreatureById(id);
        if (creature) {
            //TODO: set creature as trapper
        } else
            g_logger.traceError("could not get creature");
    }
}

void ProtocolGame::parseCreatureHealth(const InputMessagePtr& msg)
{
    const uint id = msg->getU32();
    const int healthPercent = msg->getU8();

    const CreaturePtr creature = g_map.getCreatureById(id);
    if (creature) creature->setHealthPercent(healthPercent);
}

void ProtocolGame::parseCreatureLight(const InputMessagePtr& msg)
{
    const uint id = msg->getU32();

    Light light;
    light.intensity = msg->getU8();
    light.color = msg->getU8();

    const CreaturePtr creature = g_map.getCreatureById(id);
    if (!creature) {
        g_logger.traceError("could not get creature");
        return;
    }

    creature->setLight(light);
}

void ProtocolGame::parseCreatureOutfit(const InputMessagePtr& msg)
{
    const uint id = msg->getU32();
    const Outfit outfit = getOutfit(msg);

    const CreaturePtr creature = g_map.getCreatureById(id);
    if (!creature) {
        g_logger.traceError("could not get creature");
        return;
    }

    creature->setOutfit(outfit);
}

void ProtocolGame::parseCreatureSpeed(const InputMessagePtr& msg)
{
    const uint id = msg->getU32();

    int baseSpeed = -1;
    if (g_game.getClientVersion() >= 1059)
        baseSpeed = msg->getU16();

    const int speed = msg->getU16();

    const CreaturePtr creature = g_map.getCreatureById(id);
    if (!creature) return;

    creature->setSpeed(speed);
    if (baseSpeed != -1)
        creature->setBaseSpeed(baseSpeed);
}

void ProtocolGame::parseCreatureSkulls(const InputMessagePtr& msg)
{
    const uint id = msg->getU32();
    const int skull = msg->getU8();

    const CreaturePtr creature = g_map.getCreatureById(id);
    if (!creature) {
        g_logger.traceError("could not get creature");
        return;
    }

    creature->setSkull(skull);
}

void ProtocolGame::parseCreatureShields(const InputMessagePtr& msg)
{
    const uint id = msg->getU32();
    const int shield = msg->getU8();

    const CreaturePtr creature = g_map.getCreatureById(id);
    if (!creature) {
        g_logger.traceError("could not get creature");
        return;
    }

    creature->setShield(shield);
}

void ProtocolGame::parseCreatureUnpass(const InputMessagePtr& msg)
{
    const uint id = msg->getU32();
    const bool unpass = msg->getU8();

    const CreaturePtr creature = g_map.getCreatureById(id);
    if (!creature) {
        g_logger.traceError("could not get creature");
        return;
    }

    creature->setPassable(!unpass);
}

void ProtocolGame::parseEditText(const InputMessagePtr& msg)
{
    const uint id = msg->getU32();

    int itemId;
    if (g_game.getClientVersion() >= 1010) {
        // TODO: processEditText with ItemPtr as parameter
        const ItemPtr item = getItem(msg);
        itemId = item->getId();
    } else
        itemId = msg->getU16();

    const int maxLength = msg->getU16();
    const auto text = msg->getString();

    const auto writer = msg->getString();

    if (g_game.getClientVersion() >= 1281) {
        msg->getU8(); // suffix
    }

    std::string date;
    if (g_game.getFeature(Otc::GameWritableDate))
        date = msg->getString();

    Game::processEditText(id, itemId, maxLength, text, writer, date);
}

void ProtocolGame::parseEditList(const InputMessagePtr& msg)
{
    const int doorId = msg->getU8();
    const uint id = msg->getU32();
    const auto text = msg->getString();

    Game::processEditList(id, doorId, text);
}

void ProtocolGame::parsePremiumTrigger(const InputMessagePtr& msg)
{
    const int triggerCount = msg->getU8();
    std::vector<int> triggers;

    for (int i = 0; i < triggerCount; ++i) {
        triggers.push_back(msg->getU8());
    }

    if (g_game.getClientVersion() <= 1096) {
        msg->getU8(); // == 1; // something
    }
}

void ProtocolGame::parsePlayerInfo(const InputMessagePtr& msg)
{
    const bool premium = msg->getU8(); // premium
    if (g_game.getFeature(Otc::GamePremiumExpiration))
        msg->getU32(); // premium expiration used for premium advertisement
    const int vocation = msg->getU8(); // vocation

    if (g_game.getClientVersion() >= 1281) {
        msg->getU8(); // prey enabled
    }

    const int spellCount = msg->getU16();
    std::vector<int> spells;
    for (int i = 0; i < spellCount; ++i)
        spells.push_back(msg->getU8()); // spell id

    if (g_game.getClientVersion() >= 1281) {
        msg->getU8(); // is magic shield active (bool)
    }

    m_localPlayer->setPremium(premium);
    m_localPlayer->setVocation(vocation);
    m_localPlayer->setSpells(spells);
}

void ProtocolGame::parsePlayerStats(const InputMessagePtr& msg)
{
    double health;
    double maxHealth;

    if (g_game.getFeature(Otc::GameDoubleHealth)) {
        health = msg->getU32();
        maxHealth = msg->getU32();
    } else {
        health = msg->getU16();
        maxHealth = msg->getU16();
    }

    double freeCapacity = 0;
    double totalCapacity = 0;

    if (g_game.getFeature(Otc::GameDoubleFreeCapacity))
        freeCapacity = msg->getU32() / 100.0;
    else
        freeCapacity = msg->getU16() / 100.0;

    if (g_game.getClientVersion() < 1281) {
        if (g_game.getFeature(Otc::GameTotalCapacity))
            totalCapacity = msg->getU32() / 100.0;
    }

    double experience;
    if (g_game.getFeature(Otc::GameDoubleExperience))
        experience = msg->getU64();
    else
        experience = msg->getU32();

    double level;
    if (g_game.getFeature(Otc::GameLevelU16))
        level = msg->getU16();
    else
        level = msg->getU8();

    const double levelPercent = msg->getU8();

    if (g_game.getFeature(Otc::GameExperienceBonus)) {
        if (g_game.getClientVersion() <= 1096) {
            msg->getDouble(); // experienceBonus
        } else {
            msg->getU16(); // baseXpGain
            if (g_game.getClientVersion() < 1281) {
                msg->getU16(); // voucherAddend
            }
            msg->getU16(); // grindingAddend
            msg->getU16(); // storeBoostAddend
            msg->getU16(); // huntingBoostFactor
        }
    }

    double mana;
    double maxMana;

    if (g_game.getFeature(Otc::GameDoubleHealth)) {
        mana = msg->getU32();
        maxMana = msg->getU32();
    } else {
        mana = msg->getU16();
        maxMana = msg->getU16();
    }

    if (g_game.getClientVersion() < 1281) {
        const double magicLevel = msg->getU8();

        double baseMagicLevel = 0;
        if (g_game.getFeature(Otc::GameSkillsBase))
            baseMagicLevel = msg->getU8();
        else
            baseMagicLevel = magicLevel;

        const double magicLevelPercent = msg->getU8();

        m_localPlayer->setMagicLevel(magicLevel, magicLevelPercent);
        m_localPlayer->setBaseMagicLevel(baseMagicLevel);
    }

    double soul = 0;
    if (g_game.getFeature(Otc::GameSoul))
        soul = msg->getU8();

    double stamina = 0;
    if (g_game.getFeature(Otc::GamePlayerStamina))
        stamina = msg->getU16();

    double baseSpeed = 0;
    if (g_game.getFeature(Otc::GameSkillsBase))
        baseSpeed = msg->getU16();

    double regeneration = 0;
    if (g_game.getFeature(Otc::GamePlayerRegenerationTime))
        regeneration = msg->getU16();

    double training = 0;
    if (g_game.getFeature(Otc::GameOfflineTrainingTime)) {
        training = msg->getU16();
    }

    if (g_game.getClientVersion() >= 1097) {
        msg->getU16(); // xp boost time (seconds)
        msg->getU8(); // enables exp boost in the store
    }

    if (g_game.getClientVersion() >= 1281) {
        msg->getU16();  // remaining mana shield
        msg->getU16();  // total mana shield
    }

    m_localPlayer->setHealth(health, maxHealth);
    m_localPlayer->setFreeCapacity(freeCapacity);
    m_localPlayer->setTotalCapacity(totalCapacity);
    m_localPlayer->setExperience(experience);
    m_localPlayer->setLevel(level, levelPercent);
    m_localPlayer->setMana(mana, maxMana);
    m_localPlayer->setStamina(stamina);
    m_localPlayer->setSoul(soul);
    m_localPlayer->setBaseSpeed(baseSpeed);
    m_localPlayer->setRegenerationTime(regeneration);
    m_localPlayer->setOfflineTrainingTime(training);
}

void ProtocolGame::parsePlayerSkills(const InputMessagePtr& msg)
{
    if (g_game.getClientVersion() >= 1281) {
        // magic level
        const int magicLevel = msg->getU16();
        const int baseMagicLevel = msg->getU16();
        msg->getU16(); // base + loyalty bonus(?)
        const int percent = msg->getU16() / 100; // perfect opportunity to use float :)

        m_localPlayer->setMagicLevel(magicLevel, percent);
        m_localPlayer->setBaseMagicLevel(baseMagicLevel);
    }

    for (int skill = Otc::Fist; skill <= Otc::Fishing; ++skill) {
        int level;

        if (g_game.getFeature(Otc::GameDoubleSkills))
            level = msg->getU16();
        else
            level = msg->getU8();

        int baseLevel;
        if (g_game.getFeature(Otc::GameSkillsBase))
            if (g_game.getFeature(Otc::GameBaseSkillU16))
                baseLevel = msg->getU16();
            else
                baseLevel = msg->getU8();
        else
            baseLevel = level;

        int levelPercent = 0;

        if (g_game.getClientVersion() >= 1281) {
            msg->getU16(); // base + loyalty bonus(?)
            levelPercent = msg->getU16() / 100;
        } else {
            levelPercent = msg->getU8();
        }

        m_localPlayer->setSkill(static_cast<Otc::Skill>(skill), level, levelPercent);
        m_localPlayer->setBaseSkill(static_cast<Otc::Skill>(skill), baseLevel);
    }

    if (g_game.getFeature(Otc::GameAdditionalSkills)) {
        // Critical, Life Leech, Mana Leech, Dodge, Fatal, Momentum have no level percent, nor loyalty bonus

        const uint8_t lastSkill = g_game.getClientVersion() >= 1281 ? Otc::LastSkill : Otc::ManaLeechAmount + 1;
        for (int skill = Otc::CriticalChance; skill < lastSkill; ++skill) {
            const int level = msg->getU16();
            const int baseLevel = msg->getU16();
            m_localPlayer->setSkill(static_cast<Otc::Skill>(skill), level, 0);
            m_localPlayer->setBaseSkill(static_cast<Otc::Skill>(skill), baseLevel);
        }
    }

    if (g_game.getClientVersion() >= 1281) {
        // bonus cap
        const int capacity = msg->getU32(); // base + bonus capacity
        msg->getU32(); // base capacity

        //m_localPlayer->setFreeCapacity(freeCapacity);
        m_localPlayer->setTotalCapacity(capacity);
    }
}

void ProtocolGame::parsePlayerState(const InputMessagePtr& msg)
{
    int states;

    if (g_game.getClientVersion() >= 1281) {
        states = msg->getU32();
    } else {
        if (g_game.getFeature(Otc::GamePlayerStateU16))
            states = msg->getU16();
        else
            states = msg->getU8();
    }

    m_localPlayer->setStates(states);
}

void ProtocolGame::parsePlayerCancelAttack(const InputMessagePtr& msg)
{
    uint seq = 0;
    if (g_game.getFeature(Otc::GameAttackSeq))
        seq = msg->getU32();

    g_game.processAttackCancel(seq);
}

void ProtocolGame::parsePlayerModes(const InputMessagePtr& msg)
{
    int fightMode = msg->getU8();
    int chaseMode = msg->getU8();
    const bool safeMode = msg->getU8();

    int pvpMode = 0;
    if (g_game.getFeature(Otc::GamePVPMode))
        pvpMode = msg->getU8();

    g_game.processPlayerModes(static_cast<Otc::FightModes>(fightMode), static_cast<Otc::ChaseModes>(chaseMode), safeMode, static_cast<Otc::PVPModes>(pvpMode));
}

void ProtocolGame::parseSpellCooldown(const InputMessagePtr& msg)
{
    const int spellId = msg->getU8();
    const int delay = msg->getU32();

    g_lua.callGlobalField("g_game", "onSpellCooldown", spellId, delay);
}

void ProtocolGame::parseSpellGroupCooldown(const InputMessagePtr& msg)
{
    const int groupId = msg->getU8();
    const int delay = msg->getU32();

    g_lua.callGlobalField("g_game", "onSpellGroupCooldown", groupId, delay);
}

void ProtocolGame::parseMultiUseCooldown(const InputMessagePtr& msg)
{
    const int delay = msg->getU32();

    g_lua.callGlobalField("g_game", "onMultiUseCooldown", delay);
}

void ProtocolGame::parseTalk(const InputMessagePtr& msg)
{
    if (g_game.getFeature(Otc::GameMessageStatements))
        msg->getU32(); // channel statement guid

    const auto name = g_game.formatCreatureName(msg->getString());

    if (g_game.getClientVersion() >= 1281) {
        msg->getU8(); // suffix
    }

    int level = 0;
    if (g_game.getFeature(Otc::GameMessageLevel))
        level = msg->getU16();

    const Otc::MessageMode mode = Proto::translateMessageModeFromServer(msg->getU8());
    int channelId = 0;
    Position pos;

    switch (mode) {
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

    const auto text = msg->getString();

    Game::processTalk(name, level, mode, text, channelId, pos);
}

void ProtocolGame::parseChannelList(const InputMessagePtr& msg)
{
    const int count = msg->getU8();
    std::vector<std::tuple<int, std::string> > channelList;
    for (int i = 0; i < count; ++i) {
        int id = msg->getU16();
        const auto name = msg->getString();
        channelList.emplace_back(id, name);
    }

    Game::processChannelList(channelList);
}

void ProtocolGame::parseOpenChannel(const InputMessagePtr& msg)
{
    const int channelId = msg->getU16();
    const auto name = msg->getString();

    if (g_game.getFeature(Otc::GameChannelPlayerList)) {
        const int joinedPlayers = msg->getU16();
        for (int i = 0; i < joinedPlayers; ++i)
            g_game.formatCreatureName(msg->getString()); // player name
        const int invitedPlayers = msg->getU16();
        for (int i = 0; i < invitedPlayers; ++i)
            g_game.formatCreatureName(msg->getString()); // player name
    }

    Game::processOpenChannel(channelId, name);
}

void ProtocolGame::parseOpenPrivateChannel(const InputMessagePtr& msg)
{
    const auto name = g_game.formatCreatureName(msg->getString());

    Game::processOpenPrivateChannel(name);
}

void ProtocolGame::parseOpenOwnPrivateChannel(const InputMessagePtr& msg)
{
    const int channelId = msg->getU16();
    const auto name = msg->getString();

    Game::processOpenOwnPrivateChannel(channelId, name);
}

void ProtocolGame::parseCloseChannel(const InputMessagePtr& msg)
{
    const int channelId = msg->getU16();

    Game::processCloseChannel(channelId);
}

void ProtocolGame::parseRuleViolationChannel(const InputMessagePtr& msg)
{
    const int channelId = msg->getU16();

    Game::processRuleViolationChannel(channelId);
}

void ProtocolGame::parseRuleViolationRemove(const InputMessagePtr& msg)
{
    const auto name = msg->getString();

    Game::processRuleViolationRemove(name);
}

void ProtocolGame::parseRuleViolationCancel(const InputMessagePtr& msg)
{
    const auto name = msg->getString();
    Game::processRuleViolationCancel(name);
}

void ProtocolGame::parseRuleViolationLock(const InputMessagePtr&)
{
    Game::processRuleViolationLock();
}

void ProtocolGame::parseTextMessage(const InputMessagePtr& msg)
{
    const int code = msg->getU8();
    const Otc::MessageMode mode = Proto::translateMessageModeFromServer(code);
    std::string_view text;

    switch (mode) {
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
            const Position pos = getPosition(msg);
            uint value[2];
            int color[2];

            // physical damage
            value[0] = msg->getU32();
            color[0] = msg->getU8();

            // magic damage
            value[1] = msg->getU32();
            color[1] = msg->getU8();
            text = msg->getString();

            for (int i = 0; i < 2; ++i) {
                if (value[i] == 0)
                    continue;
                auto animatedText = AnimatedTextPtr(new AnimatedText);
                animatedText->setColor(color[i]);
                animatedText->setText(std::to_string(value[i]));
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
            const Position pos = getPosition(msg);
            const uint value = msg->getU32();
            const int color = msg->getU8();
            text = msg->getString();

            const auto animatedText = AnimatedTextPtr(new AnimatedText);
            animatedText->setColor(color);
            animatedText->setText(std::to_string(value));
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

    Game::processTextMessage(mode, text);
}

void ProtocolGame::parseCancelWalk(const InputMessagePtr& msg)
{
    const auto direction = static_cast<Otc::Direction>(msg->getU8());

    g_game.processWalkCancel(direction);
}

void ProtocolGame::parseWalkWait(const InputMessagePtr& msg)
{
    const int millis = msg->getU16();
    m_localPlayer->lockWalk(millis);
}

void ProtocolGame::parseFloorChangeUp(const InputMessagePtr& msg)
{
    Position pos;
    if (g_game.getFeature(Otc::GameMapMovePosition))
        pos = getPosition(msg);
    else
        pos = g_map.getCentralPosition();
    AwareRange range = g_map.getAwareRange();
    --pos.z;

    int skip = 0;
    if (pos.z == SEA_FLOOR)
        for (int i = SEA_FLOOR - AWARE_UNDEGROUND_FLOOR_RANGE; i >= 0; --i)
            skip = setFloorDescription(msg, pos.x - range.left, pos.y - range.top, i, range.horizontal(), range.vertical(), 8 - i, skip);
    else if (pos.z > SEA_FLOOR)
        skip = setFloorDescription(msg, pos.x - range.left, pos.y - range.top, pos.z - AWARE_UNDEGROUND_FLOOR_RANGE, range.horizontal(), range.vertical(), 3, skip);

    ++pos.x;
    ++pos.y;
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseFloorChangeDown(const InputMessagePtr& msg)
{
    Position pos;
    if (g_game.getFeature(Otc::GameMapMovePosition))
        pos = getPosition(msg);
    else
        pos = g_map.getCentralPosition();
    AwareRange range = g_map.getAwareRange();
    ++pos.z;

    int skip = 0;
    if (pos.z == UNDERGROUND_FLOOR) {
        int j, i;
        for (i = pos.z, j = -1; i <= pos.z + AWARE_UNDEGROUND_FLOOR_RANGE; ++i, --j)
            skip = setFloorDescription(msg, pos.x - range.left, pos.y - range.top, i, range.horizontal(), range.vertical(), j, skip);
    } else if (pos.z > UNDERGROUND_FLOOR && pos.z < MAX_Z - 1)
        skip = setFloorDescription(msg, pos.x - range.left, pos.y - range.top, pos.z + AWARE_UNDEGROUND_FLOOR_RANGE, range.horizontal(), range.vertical(), -3, skip);

    --pos.x;
    --pos.y;
    g_map.setCentralPosition(pos);
}

void ProtocolGame::parseOpenOutfitWindow(const InputMessagePtr& msg)
{
    const Outfit currentOutfit = getOutfit(msg);

    // mount color bytes are required here regardless of having one
    if (g_game.getClientVersion() >= 1281) {
        if (currentOutfit.getMount() == 0) {
            msg->getU8(); //head
            msg->getU8(); //body
            msg->getU8(); //legs
            msg->getU8(); //feet
        }

        msg->getU16(); // current familiar looktype
    }

    std::vector<std::tuple<int, std::string, int>> outfitList;

    if (g_game.getFeature(Otc::GameNewOutfitProtocol)) {
        const int outfitCount = g_game.getClientVersion() >= 1281 ? msg->getU16() : msg->getU8();
        for (int i = 0; i < outfitCount; ++i) {
            int outfitId = msg->getU16();
            const auto outfitName = msg->getString();
            int outfitAddons = msg->getU8();

            if (g_game.getClientVersion() >= 1281) {
                msg->getU8(); // mode: 0x00 - available, 0x01 store (requires U32 store offerId), 0x02 golden outfit tooltip (hardcoded)
            }

            outfitList.emplace_back(outfitId, outfitName, outfitAddons);
        }
    } else {
        int outfitStart, outfitEnd;
        if (g_game.getFeature(Otc::GameLooktypeU16)) {
            outfitStart = msg->getU16();
            outfitEnd = msg->getU16();
        } else {
            outfitStart = msg->getU8();
            outfitEnd = msg->getU8();
        }

        for (int i = outfitStart; i <= outfitEnd; ++i)
            outfitList.emplace_back(i, "", 0);
    }

    std::vector<std::tuple<int, std::string> > mountList;
    if (g_game.getFeature(Otc::GamePlayerMounts)) {
        const int mountCount = g_game.getClientVersion() >= 1281 ? msg->getU16() : msg->getU8();
        for (int i = 0; i < mountCount; ++i) {
            int mountId = msg->getU16(); // mount type
            const auto mountName = msg->getString(); // mount name

            if (g_game.getClientVersion() >= 1281) {
                msg->getU8(); // mode: 0x00 - available, 0x01 store (requires U32 store offerId)
            }

            mountList.emplace_back(mountId, mountName);
        }
    }

    if (g_game.getClientVersion() >= 1281) {
        msg->getU16(); // familiars.size()
        // size > 0
        // U16 looktype
        // String name
        // 0x00 // mode: 0x00 - available, 0x01 store (requires U32 store offerId)

        msg->getU8(); //Try outfit mode (?)
        msg->getU8(); // mounted
        msg->getU8(); // randomize mount (bool)
    }

    g_game.processOpenOutfitWindow(currentOutfit, outfitList, mountList);
}

void ProtocolGame::parseKillTracker(const InputMessagePtr& msg)
{
    msg->getString(); // monster name
    getOutfit(msg, false);

    // corpse items
    const int size = msg->getU8();
    for (int i = 0; i < size; i++) {
        getItem(msg);
    }
}

void ProtocolGame::parseVipAdd(const InputMessagePtr& msg)
{
    uint iconId = 0;
    std::string desc;
    bool notifyLogin = false;

    const uint id = msg->getU32();
    const auto name = g_game.formatCreatureName(msg->getString());
    if (g_game.getFeature(Otc::GameAdditionalVipInfo)) {
        desc = msg->getString();
        iconId = msg->getU32();
        notifyLogin = msg->getU8();
    }
    const uint status = msg->getU8();

    if (g_game.getClientVersion() >= 1281) {
        msg->getU8(); // vip groups
    }

    g_game.processVipAdd(id, name, status, desc, iconId, notifyLogin);
}

void ProtocolGame::parseVipState(const InputMessagePtr& msg)
{
    const uint id = msg->getU32();
    if (g_game.getFeature(Otc::GameLoginPending)) {
        const uint status = msg->getU8();
        g_game.processVipStateChange(id, status);
    } else {
        g_game.processVipStateChange(id, 1);
    }
}

void ProtocolGame::parseVipLogout(const InputMessagePtr& msg)
{
    const uint id = msg->getU32();
    g_game.processVipStateChange(id, 0);
}

void ProtocolGame::parseTutorialHint(const InputMessagePtr& msg)
{
    const int id = msg->getU8();
    Game::processTutorialHint(id);
}

void ProtocolGame::parseAutomapFlag(const InputMessagePtr& msg)
{
    const Position pos = getPosition(msg);
    const int icon = msg->getU8();
    const auto description = msg->getString();

    bool remove = false;
    if (g_game.getFeature(Otc::GameMinimapRemove))
        remove = msg->getU8() != 0;

    if (!remove)
        Game::processAddAutomapFlag(pos, icon, description);
    else
        Game::processRemoveAutomapFlag(pos, icon, description);
}

void ProtocolGame::parseQuestLog(const InputMessagePtr& msg)
{
    std::vector<std::tuple<int, std::string, bool> > questList;
    const int questsCount = msg->getU16();
    for (int i = 0; i < questsCount; ++i) {
        int id = msg->getU16();
        const auto name = msg->getString();
        bool completed = msg->getU8();
        questList.emplace_back(id, name, completed);
    }

    Game::processQuestLog(questList);
}

void ProtocolGame::parseQuestLine(const InputMessagePtr& msg)
{
    std::vector<std::tuple<std::string, std::string>> questMissions;
    const int questId = msg->getU16();
    const int missionCount = msg->getU8();
    for (int i = 0; i < missionCount; ++i) {
        const auto missionName = msg->getString();
        const auto missionDescrition = msg->getString();
        questMissions.emplace_back(missionName, missionDescrition);
    }

    Game::processQuestLine(questId, questMissions);
}

void ProtocolGame::parseChannelEvent(const InputMessagePtr& msg)
{
    const uint16 channelId = msg->getU16();
    const auto name = g_game.formatCreatureName(msg->getString());
    const uint8 type = msg->getU8();

    g_lua.callGlobalField("g_game", "onChannelEvent", channelId, name, type);
}

void ProtocolGame::parseItemInfo(const InputMessagePtr& msg)
{
    std::vector<std::tuple<ItemPtr, std::string>> list;
    const int size = msg->getU8();
    for (int i = 0; i < size; ++i) {
        ItemPtr item(new Item);
        item->setId(msg->getU16());
        item->setCountOrSubType(msg->getU8());

        const auto desc = msg->getString();
        list.emplace_back(item, desc);
    }

    g_lua.callGlobalField("g_game", "onItemInfo", list);
}

void ProtocolGame::parsePlayerInventory(const InputMessagePtr& msg)
{
    const int size = msg->getU16();
    for (int i = 0; i < size; ++i) {
        msg->getU16(); // id
        msg->getU8(); // subtype
        msg->getU16(); // count
    }
}

void ProtocolGame::parseModalDialog(const InputMessagePtr& msg)
{
    const uint32 windowId = msg->getU32();
    const auto title = msg->getString();
    const auto message = msg->getString();

    const int sizeButtons = msg->getU8();
    std::vector<std::tuple<int, std::string> > buttonList;
    for (int i = 0; i < sizeButtons; ++i) {
        const auto value = msg->getString();
        int buttonId = msg->getU8();
        buttonList.emplace_back(buttonId, value);
    }

    const int sizeChoices = msg->getU8();
    std::vector<std::tuple<int, std::string> > choiceList;
    for (int i = 0; i < sizeChoices; ++i) {
        const auto value = msg->getString();
        int choideId = msg->getU8();
        choiceList.emplace_back(choideId, value);
    }

    int enterButton, escapeButton;
    if (g_game.getClientVersion() > 970) {
        escapeButton = msg->getU8();
        enterButton = msg->getU8();
    } else {
        enterButton = msg->getU8();
        escapeButton = msg->getU8();
    }

    const bool priority = msg->getU8() == 0x01;

    Game::processModalDialog(windowId, title, message, buttonList, enterButton, escapeButton, choiceList, priority);
}

void ProtocolGame::parseExtendedOpcode(const InputMessagePtr& msg)
{
    const int opcode = msg->getU8();
    const auto buffer = msg->getString();

    if (opcode == 0)
        m_enableSendExtendedOpcode = true;
    else if (opcode == 2)
        parsePingBack(msg);
    else
        callLuaField("onExtendedOpcode", opcode, buffer);
}

void ProtocolGame::parseChangeMapAwareRange(const InputMessagePtr& msg)
{
    const int xrange = msg->getU8();
    const int yrange = msg->getU8();

    AwareRange range;
    range.left = xrange / 2 - (xrange + 1) % 2;
    range.right = xrange / 2;
    range.top = yrange / 2 - (yrange + 1) % 2;
    range.bottom = yrange / 2;

    g_map.setAwareRange(range);
    g_lua.callGlobalField("g_game", "onMapChangeAwareRange", xrange, yrange);
}

void ProtocolGame::parseCreaturesMark(const InputMessagePtr& msg)
{
    int len;
    if (g_game.getClientVersion() >= 1035) {
        len = 1;
    } else {
        len = msg->getU8();
    }

    for (int i = 0; i < len; ++i) {
        const uint32 id = msg->getU32();
        const bool isPermanent = msg->getU8() != 1;
        const uint8 markType = msg->getU8();

        CreaturePtr creature = g_map.getCreatureById(id);
        if (creature) {
            if (isPermanent) {
                if (markType == 0xff)
                    creature->hideStaticSquare();
                else
                    creature->showStaticSquare(Color::from8bit(markType));
            } else
                creature->addTimedSquare(markType);
        } else
            g_logger.traceError("could not get creature");
    }
}

void ProtocolGame::parseCreatureType(const InputMessagePtr& msg)
{
    const uint32 id = msg->getU32();
    const uint8 type = msg->getU8();

    const CreaturePtr creature = g_map.getCreatureById(id);
    if (creature)
        creature->setType(type);
    else
        g_logger.traceError("could not get creature");
}

void ProtocolGame::setMapDescription(const InputMessagePtr& msg, int x, int y, int z, int width, int height)
{
    int startz, endz, zstep;

    if (z > SEA_FLOOR) {
        startz = z - AWARE_UNDEGROUND_FLOOR_RANGE;
        endz = std::min<int>(z + AWARE_UNDEGROUND_FLOOR_RANGE, MAX_Z);
        zstep = 1;
    } else {
        startz = SEA_FLOOR;
        endz = 0;
        zstep = -1;
    }

    int skip = 0;
    for (int nz = startz; nz != endz + zstep; nz += zstep)
        skip = setFloorDescription(msg, x, y, nz, width, height, z - nz, skip);
}

int ProtocolGame::setFloorDescription(const InputMessagePtr& msg, int x, int y, int z, int width, int height, int offset, int skip)
{
    for (int nx = 0; nx < width; ++nx) {
        for (int ny = 0; ny < height; ++ny) {
            Position tilePos(x + nx + offset, y + ny + offset, z);
            if (skip == 0)
                skip = setTileDescription(msg, tilePos);
            else {
                g_map.cleanTile(tilePos);
                --skip;
            }
        }
    }
    return skip;
}

int ProtocolGame::setTileDescription(const InputMessagePtr& msg, Position position)
{
    g_map.cleanTile(position);

    bool gotEffect = false;
    for (int stackPos = 0; stackPos < 256; ++stackPos) {
        if (msg->peekU16() >= 0xff00)
            return msg->getU16() & 0xff;

        if (g_game.getFeature(Otc::GameEnvironmentEffect) && !gotEffect) {
            msg->getU16(); // environment effect
            gotEffect = true;
            continue;
        }

        if (stackPos > MAX_THINGS)
            g_logger.traceError(stdext::format("too many things, pos=%s, stackpos=%d", stdext::to_string(position), stackPos));

        ThingPtr thing = getThing(msg);
        g_map.addThing(thing, position, stackPos);
    }

    return 0;
}

Outfit ProtocolGame::getOutfit(const InputMessagePtr& msg, bool parseMount/* = true*/)
{
    Outfit outfit;

    int lookType;
    if (g_game.getFeature(Otc::GameLooktypeU16))
        lookType = msg->getU16();
    else
        lookType = msg->getU8();

    if (lookType != 0) {
        outfit.setCategory(ThingCategoryCreature);
        const int head = msg->getU8();
        const int body = msg->getU8();
        const int legs = msg->getU8();
        const int feet = msg->getU8();
        int addons = 0;
        if (g_game.getFeature(Otc::GamePlayerAddons))
            addons = msg->getU8();

        if (!g_things.isValidDatId(lookType, ThingCategoryCreature)) {
            g_logger.traceError(stdext::format("invalid outfit looktype %d", lookType));
            lookType = 0;
        }

        outfit.setId(lookType);
        outfit.setHead(head);
        outfit.setBody(body);
        outfit.setLegs(legs);
        outfit.setFeet(feet);
        outfit.setAddons(addons);
    } else {
        int lookTypeEx = msg->getU16();
        if (lookTypeEx == 0) {
            outfit.setCategory(ThingCategoryEffect);
            outfit.setAuxId(13); // invisible effect id
        } else {
            if (!g_things.isValidDatId(lookTypeEx, ThingCategoryItem)) {
                g_logger.traceError(stdext::format("invalid outfit looktypeex %d", lookTypeEx));
                lookTypeEx = 0;
            }
            outfit.setCategory(ThingCategoryItem);
            outfit.setAuxId(lookTypeEx);
        }
    }

    if (g_game.getFeature(Otc::GamePlayerMounts) && parseMount) {
        const int mount = msg->getU16();
        if (g_game.getClientVersion() >= 1281 && mount != 0) {
            msg->getU8(); //head
            msg->getU8(); //body
            msg->getU8(); //legs
            msg->getU8(); //feet
        }
        outfit.setMount(mount);
    }

    return outfit;
}

ThingPtr ProtocolGame::getThing(const InputMessagePtr& msg)
{
    ThingPtr thing;

    const int id = msg->getU16();

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

ThingPtr ProtocolGame::getMappedThing(const InputMessagePtr& msg)
{
    ThingPtr thing;
    const uint16 x = msg->getU16();

    if (x != 0xffff) {
        Position pos;
        pos.x = x;
        pos.y = msg->getU16();
        pos.z = msg->getU8();
        const uint8 stackpos = msg->getU8();
        assert(stackpos != UINT8_MAX);
        thing = g_map.getThing(pos, stackpos);
        if (!thing)
            g_logger.traceError(stdext::format("no thing at pos:%s, stackpos:%d", stdext::to_string(pos), stackpos));
    } else {
        const uint32 id = msg->getU32();
        thing = g_map.getCreatureById(id);
        if (!thing)
            g_logger.traceError(stdext::format("no creature with id %u", id));
    }

    return thing;
}

CreaturePtr ProtocolGame::getCreature(const InputMessagePtr& msg, int type)
{
    if (type == 0)
        type = msg->getU16();

    CreaturePtr creature;
    const bool known = type != Proto::UnknownCreature;
    if (type == Proto::OutdatedCreature || type == Proto::UnknownCreature) {
        if (known) {
            const uint id = msg->getU32();
            creature = g_map.getCreatureById(id);
            if (!creature)
                g_logger.traceError("server said that a creature is known, but it's not");

            // Is necessary reset camera?
            // if(creature->isLocalPlayer()) g_map.resetLastCamera();
        } else {
            const uint removeId = msg->getU32();
            const uint id = msg->getU32();

            if (id == removeId) {
                creature = g_map.getCreatureById(id);
            } else {
                g_map.removeCreatureById(removeId);
            }

            int creatureType;
            if (g_game.getClientVersion() >= 910)
                creatureType = msg->getU8();
            else {
                if (id >= Proto::PlayerStartId && id < Proto::PlayerEndId)
                    creatureType = Proto::CreatureTypePlayer;
                else if (id >= Proto::MonsterStartId && id < Proto::MonsterEndId)
                    creatureType = Proto::CreatureTypeMonster;
                else
                    creatureType = Proto::CreatureTypeNpc;
            }

            if (g_game.getClientVersion() >= 1281 && creatureType == Proto::CreatureTypeSummonOwn) {
                msg->getU32(); // master id
            }

            const auto name = g_game.formatCreatureName(msg->getString());

            if (!creature) {
                if ((id == m_localPlayer->getId()) ||
                    // fixes a bug server side bug where GameInit is not sent and local player id is unknown
                    (creatureType == Proto::CreatureTypePlayer && !m_localPlayer->getId() && name == m_localPlayer->getName())) {
                    creature = m_localPlayer;
                } else switch (creatureType) {
                    case Proto::CreatureTypePlayer:
                        creature = PlayerPtr(new Player);
                        break;

                    case Proto::CreatureTypeNpc:
                        creature = NpcPtr(new Npc);
                        break;

                    case Proto::CreatureTypeHidden:
                    case Proto::CreatureTypeMonster:
                    case Proto::CreatureTypeSummonOwn:
                    case Proto::CreatureTypeSummonOther:
                        creature = MonsterPtr(new Monster);
                        break;

                    default:
                        g_logger.traceError("creature type is invalid");
                }
            }

            if (creature) {
                creature->setId(id);
                creature->setName(name);

                g_map.addCreature(creature);
            }
        }

        const int healthPercent = msg->getU8();
        const auto direction = static_cast<Otc::Direction>(msg->getU8());
        const Outfit& outfit = getOutfit(msg);

        Light light;
        light.intensity = msg->getU8();
        light.color = msg->getU8();

        const int speed = msg->getU16();

        if (g_game.getClientVersion() >= 1281) {
            const uint8_t iconDebuff = msg->getU8(); // creature debuffs
            if (iconDebuff != 0) {
                msg->getU8(); // Icon
                msg->getU8(); // Update (?)
                msg->getU16(); // Counter text
            }
        }

        const int skull = msg->getU8();
        const int shield = msg->getU8();

        // emblem is sent only when the creature is not known
        int8 emblem = -1;
        int8 creatureType = -1;
        int8 icon = -1;
        bool unpass = true;

        if (g_game.getFeature(Otc::GameCreatureEmblems) && !known)
            emblem = msg->getU8();

        if (g_game.getFeature(Otc::GameThingMarks)) {
            creatureType = msg->getU8();
        }

        if (g_game.getClientVersion() >= 1281) {
            if (creatureType == Proto::CreatureTypeSummonOwn) {
                msg->getU32(); // master id
            } else if (creatureType == Proto::CreatureTypePlayer) {
                msg->getU8(); // voc id
            }
        }

        if (g_game.getFeature(Otc::GameCreatureIcons)) {
            icon = msg->getU8();
        }

        if (g_game.getFeature(Otc::GameThingMarks)) {
            const uint8 mark = msg->getU8(); // mark

            if (g_game.getClientVersion() < 1281) {
                msg->getU16(); // helpers
            }

            if (creature) {
                if (mark == 0xff)
                    creature->hideStaticSquare();
                else
                    creature->showStaticSquare(Color::from8bit(mark));
            }
        }

        if (g_game.getClientVersion() >= 1281) {
            msg->getU8(); // inspection type
        }

        if (g_game.getClientVersion() >= 854)
            unpass = msg->getU8();

        if (creature) {
            creature->setHealthPercent(healthPercent);
            creature->turn(direction);
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
    } else if (type == Proto::Creature) {
        // this is send creature turn
        const uint id = msg->getU32();
        creature = g_map.getCreatureById(id);

        if (!creature)
            g_logger.traceError("invalid creature");

        const auto direction = static_cast<Otc::Direction>(msg->getU8());
        if (creature)
            creature->turn(direction);

        if (g_game.getClientVersion() >= 953) {
            const bool unpass = msg->getU8();

            if (creature)
                creature->setPassable(!unpass);
        }
    } else {
        stdext::throw_exception("invalid creature opcode");
    }

    return creature;
}

ItemPtr ProtocolGame::getItem(const InputMessagePtr& msg, int id)
{
    if (id == 0)
        id = msg->getU16();

    ItemPtr item = Item::create(id);
    if (item->getId() == 0)
        stdext::throw_exception(stdext::format("unable to create item with invalid id %d", id));

    if (g_game.getClientVersion() < 1281 && g_game.getFeature(Otc::GameThingMarks)) {
        msg->getU8(); // mark
    }

    if (item->isStackable() || item->isFluidContainer() || item->isSplash() || item->isChargeable()) {
        item->setCountOrSubType(msg->getU8());
    } else if (g_game.getClientVersion() >= 1281) {
        if (item->isContainer()) {
            const uint8 hasQuickLootFlags = msg->getU8();
            if (hasQuickLootFlags) {
                msg->getU32(); // quick loot flags
            }

            const uint8 hasQuiverAmmoCount = msg->getU8();
            if (hasQuiverAmmoCount) {
                msg->getU32(); // ammoTotal
            }
        } else if (item->getClassification() != 0) {
            msg->getU8();
        }
    }

    if (g_game.getFeature(Otc::GameItemAnimationPhase)) {
        if (item->getAnimationPhases() > 1) {
            // 0x00 => automatic phase
            // 0xFE => random phase
            // 0xFF => async phase
            msg->getU8();
            //item->setPhase(msg->getU8());
        }
    }

    if (g_game.getClientVersion() >= 1281) {
        if (id == 35973 || id == 35974) {
            const int looktype = msg->getU16();
            if (looktype != 0) {
                msg->getU8(); // lookHead
                msg->getU8(); // lookBody
                msg->getU8(); // lookLegs
                msg->getU8(); // lookFeet
                msg->getU8(); // lookAddons
            }

            const int lookmount = msg->getU16();
            if (lookmount != 0) {
                msg->getU8(); // lookHead
                msg->getU8(); // lookBody
                msg->getU8(); // lookLegs
                msg->getU8(); // lookFeet
            }

            msg->getU8(); // direction
            msg->getU8(); // visible (bool)
        }
    }

    return item;
}

StaticTextPtr ProtocolGame::getStaticText(const InputMessagePtr& msg, int)
{
    const int colorByte = msg->getU8();
    const Color color = Color::from8bit(colorByte);
    const auto fontName = msg->getString();
    const auto text = msg->getString();

    auto staticText = StaticTextPtr(new StaticText);
    staticText->setText(text);
    staticText->setFont(fontName);
    staticText->setColor(color);
    return staticText;
}

Position ProtocolGame::getPosition(const InputMessagePtr& msg)
{
    const uint16 x = msg->getU16();
    const uint16 y = msg->getU16();
    const uint8 z = msg->getU8();

    return { x, y, z };
}

// 12x
void ProtocolGame::parseShowDescription(const InputMessagePtr& msg)
{
    msg->getU32(); // offerId
    msg->getString();  // offer description
}

void ProtocolGame::parseBestiaryTracker(const InputMessagePtr& msg)
{
    const uint8_t size = msg->getU8();
    for (uint8_t i = 0; i < size; i++) {
        msg->getU16(); // RaceID
        msg->getU32(); // Kill count
        msg->getU16(); // First unlock
        msg->getU16(); // Second unlock
        msg->getU16(); // Last unlock
        msg->getU8(); // Status
    }
}

void ProtocolGame::parseTaskHuntingBasicData(const InputMessagePtr& msg)
{
    const uint16_t preys = msg->getU16();
    for (uint16_t i = 0; i < preys; i++) {
        msg->getU16(); // RaceID
        msg->getU8(); // Difficult
    }

    const uint8_t options = msg->getU8();
    for (uint8_t j = 0; j < options; j++) {
        msg->getU8(); // Difficult
        msg->getU8(); // Stars
        msg->getU16(); // First kill
        msg->getU16(); // First reward
        msg->getU16(); // Second kill
        msg->getU16(); // Second reward
    }
}
void ProtocolGame::parseTaskHuntingData(const InputMessagePtr& msg)
{
    msg->getU8(); // slot
    const auto state = static_cast<Otc::PreyTaskstate_t>(msg->getU8()); // slot state

    switch (state) {
        case Otc::PREY_TASK_STATE_LOCKED:
        {
            msg->getU8(); // task slot unlocked
            break;
        }
        case Otc::PREY_TASK_STATE_INACTIVE:
            break;
        case Otc::PREY_TASK_STATE_SELECTION:
        {
            const uint16_t creatures = msg->getU16();
            for (uint16_t i = 0; i < creatures; i++) {
                msg->getU16(); // RaceID
                msg->getU8(); // Is unlocked
            }
            break;
        }
        case Otc::PREY_TASK_STATE_LIST_SELECTION:
        {
            const uint16_t creatures = msg->getU16();
            for (uint16_t i = 0; i < creatures; i++) {
                msg->getU16(); // RaceID
                msg->getU8(); // Is unlocked
            }
            break;
        }
        case Otc::PREY_TASK_STATE_ACTIVE:
        {
            msg->getU16(); // RaceID
            msg->getU8(); // Upgraded
            msg->getU16(); // Required kills
            msg->getU16(); // Current kills
            msg->getU8(); // Stars
            break;
        }
        case Otc::PREY_TASK_STATE_COMPLETED:
        {
            msg->getU16(); // RaceID
            msg->getU8(); // Upgraded
            msg->getU16(); // Required kills
            msg->getU16(); // Current kills
            break;
        }
    }

    msg->getU32(); // next free roll
}

void ProtocolGame::parseExperienceTracker(const InputMessagePtr& msg)
{
    msg->get64(); // Raw exp
    msg->get64(); // Final exp
}

void ProtocolGame::parseLootContainers(const InputMessagePtr& msg)
{
    msg->getU8(); // quickLootFallbackToMainContainer ? 1 : 0
    const int containers = msg->getU8();
    for (int i = 0; i < containers; ++i) {
        msg->getU8(); // id?
        msg->getU16();
    }
}

void ProtocolGame::parseSupplyStash(const InputMessagePtr& msg)
{
    const int size = msg->getU16();
    for (int i = 0; i < size; ++i) {
        msg->getU16(); // item id
        msg->getU32(); // unknown
    }
    msg->getU16(); // available slots?
}

void ProtocolGame::parseSpecialContainer(const InputMessagePtr& msg)
{
    msg->getU8();
    if (g_game.getProtocolVersion() >= 1220) {
        msg->getU8();
    }
}

void ProtocolGame::parsePartyAnalyzer(const InputMessagePtr& msg)
{
    msg->getU32(); // Timestamp
    msg->getU32(); // LeaderID
    msg->getU8(); // Price type
    const uint8_t size = msg->getU8();
    for (uint8_t i = 0; i < size; i++) {
        msg->getU32(); // MemberID
        msg->getU8(); // Highlight
        msg->getU64(); // Loot
        msg->getU64(); // Supply
        msg->getU64(); // Damage
        msg->getU64(); // Healing
    }

    uint8_t names = msg->getU8();
    if (names != 0) {
        names = msg->getU8();
        for (uint8_t i = 0; i < names; i++) {
            msg->getU32(); // MemberID
            msg->getString(); // Member name
        }
    }
}
void ProtocolGame::parseClientCheck(const InputMessagePtr& msg)
{
    msg->getU32(); // 1
    msg->getU8(); // 1
}

void ProtocolGame::parseGameNews(const InputMessagePtr& msg)
{
    msg->getU32(); // 1
    msg->getU8(); // 1

    // TODO: implement game news usage
}

void ProtocolGame::parseBlessDialog(const InputMessagePtr& msg)
{
    // parse bless amount
    const uint8_t totalBless = msg->getU8(); // total bless

    // parse each bless
    for (int i = 0; i < totalBless; i++) {
        msg->getU16(); // bless bit wise
        msg->getU8(); // player bless count
        msg->getU8(); // store?
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
    const uint8_t logCount = msg->getU8(); // log count
    for (int i = 0; i < logCount; i++) {
        msg->getU32(); // timestamp
        msg->getU8(); // color message (0 = white loss, 1 = red)
        msg->getString(); // history message
    }

    // TODO: implement bless dialog usage
}

void ProtocolGame::parseRestingAreaState(const InputMessagePtr& msg)
{
    msg->getU8(); // zone
    msg->getU8(); // state
    msg->getString(); // message

    // TODO: implement resting area state usage
}

void ProtocolGame::parseUpdateImpactTracker(const InputMessagePtr& msg)
{
    const uint8_t type = msg->getU8();
    msg->getU32(); // amount
    if (type == 1) {
        msg->getU8(); // Element
    } else if (type == 2) {
        msg->getU8(); // Element
        msg->getString(); // Name
    }

    // TODO: implement impact tracker usage
}

void ProtocolGame::parseItemsPrice(const InputMessagePtr& msg)
{
    const uint16_t priceCount = msg->getU16(); // count

    for (int i = 0; i < priceCount; i++) {
        uint16_t itemId = msg->getU16(); // item client id
        if (g_game.getClientVersion() >= 1281) {
            const ItemPtr item = Item::create(itemId);
            if (item->getId() == 0)
                stdext::throw_exception(stdext::format("unable to create item with invalid id %d", itemId));

            if (item->getClassification() > 0) {
                msg->getU8();
            }
            msg->getU64(); // price
        } else {
            msg->getU32(); // price
        }
    }

    // TODO: implement items price usage
}

void ProtocolGame::parseUpdateSupplyTracker(const InputMessagePtr& msg)
{
    msg->getU16(); // item client ID

    // TODO: implement supply tracker usage
}

void ProtocolGame::parseUpdateLootTracker(const InputMessagePtr& msg)
{
    getItem(msg); // item
    msg->getString(); // item name

    // TODO: implement loot tracker usage
}

void ProtocolGame::parseBestiaryEntryChanged(const InputMessagePtr& msg)
{
    msg->getU16(); // monster ID

    // TODO: implement bestiary entry changed usage
}

void ProtocolGame::parseDailyRewardCollectionState(const InputMessagePtr& msg)
{
    msg->getU8(); // state

    // TODO: implement daily reward collection state usage
}

void ProtocolGame::parseOpenRewardWall(const InputMessagePtr& msg)
{
    msg->getU8(); // bonus shrine (1) or instant bonus (0)
    msg->getU32(); // next reward time
    msg->getU8(); // day streak day
    const uint8_t wasDailyRewardTaken = msg->getU8(); // taken (player already took reward?)

    if (wasDailyRewardTaken != 0) {
        msg->getString(); // error message
        const uint8_t token = msg->getU8();
        if (token != 0) {
            msg->getU16(); // Tokens
        }
    } else {
        msg->getU8(); // Unknown
        msg->getU32(); // time left to pickup reward without loosing streak
        msg->getU16(); // Tokens
    }

    msg->getU16(); // day streak level
    // TODO: implement open reward wall usage
}

void ProtocolGame::parseDailyReward(const InputMessagePtr& msg)
{
    const uint8_t days = msg->getU8(); // Reward count (7 days)
    for (uint8_t day = 1; day <= days; day++) {
        // Free account
        uint8_t type = msg->getU8();
        msg->getU8(); // Items to pick
        uint8_t size = msg->getU8();
        if (day == 1 || day == 2 || day == 4 || day == 6) {
            for (uint8_t i = 0; i < size; i++) {
                msg->getU16(); // Item ID
                msg->getString(); // Item name
                msg->getU32(); // Item weight
            }
        } else {
            msg->getU16(); // Amount
        }

        // Premium account
        type = msg->getU8();
        msg->getU8(); // Items to pick
        size = msg->getU8();
        if (day == 1 || day == 2 || day == 4 || day == 6) {
            for (uint8_t i = 0; i < size; i++) {
                msg->getU16(); // Item ID
                msg->getString(); // Item name
                msg->getU32(); // Item weight
            }
        } else {
            msg->getU16(); // Amount
        }
    }

    const uint8_t bonus = msg->getU8();
    for (uint8_t i = 0; i < bonus; i++) {
        msg->getString(); // Bonus name
        msg->getU8(); // Bonus ID
    }

    msg->getU8(); // Unknown
    // TODO: implement daily reward usage
}

void ProtocolGame::parseRewardHistory(const InputMessagePtr& msg)
{
    const uint8_t historyCount = msg->getU8(); // history count

    for (int i = 0; i < historyCount; i++) {
        msg->getU32(); // timestamp
        msg->getU8(); // is Premium
        msg->getString(); // description
        msg->getU16(); // daystreak
    }

    // TODO: implement reward history usage
}
void ProtocolGame::parsePreyTimeLeft(const InputMessagePtr& msg)
{
    msg->getU8(); // Slot
    msg->getU16(); // Time left
    // TODO: implement protocol parse
}

void ProtocolGame::getPreyMonster(const InputMessagePtr& msg)
{
    msg->getString(); // mosnter name
    const uint16_t lookType = msg->getU16(); // looktype
    if (lookType == 0) {
        msg->getU16(); // LookTypeEx
        return;
    }
    msg->getU8(); // head
    msg->getU8(); // body
    msg->getU8(); // legs
    msg->getU8(); // feet
    msg->getU8(); // addons
}

void ProtocolGame::getPreyMonsters(const InputMessagePtr& msg)
{
    const uint8_t monstersSize = msg->getU8(); // monster list size
    for (uint8_t i = 0; i < monstersSize; i++)
        getPreyMonster(msg);
}

void ProtocolGame::parsePreyData(const InputMessagePtr& msg)
{
    msg->getU8(); // slot
    const auto state = static_cast<Otc::PreyState_t>(msg->getU8()); // slot state

    switch (state) {
        case Otc::PREY_STATE_LOCKED:
        {
            msg->getU8(); // prey slot unlocked
            break;
        }
        case Otc::PREY_STATE_INACTIVE:
            break;
        case Otc::PREY_STATE_ACTIVE:
        {
            getPreyMonster(msg);
            msg->getU8(); // bonus type
            msg->getU16(); // bonus value
            msg->getU8(); // bonus grade
            msg->getU16(); // time left
            break;
        }
        case Otc::PREY_STATE_SELECTION:
        {
            const uint8_t listSize = msg->getU8();
            for (uint8_t i = 0; i < listSize; i++) {
                getPreyMonsters(msg);
            }
            break;
        }
        case Otc::PREY_STATE_SELECTION_CHANGE_MONSTER:
        {
            msg->getU8(); // bonus type
            msg->getU16(); // bonus value
            msg->getU8(); // bonus grade
            const uint8_t listSize = msg->getU8();
            for (uint8_t i = 0; i < listSize; i++) {
                getPreyMonsters(msg);
            }
            break;
        }
        case Otc::PREY_STATE_LIST_SELECTION:
        {
            const uint16_t creatures = msg->getU16();
            for (uint16_t i = 0; i < creatures; i++) {
                msg->getU16(); // RaceID
            }
            break;
        }
        case Otc::PREY_STATE_WILDCARD_SELECTION:
        {
            msg->getU8(); // bonus type
            msg->getU16(); // bonus value
            msg->getU8(); // bonus grade
            const uint16_t creatures = msg->getU16();
            for (uint16_t i = 0; i < creatures; i++) {
                msg->getU16(); // RaceID
            }
            break;
        }
    }

    msg->getU32(); // next free roll
    msg->getU8(); // wildcards
}

void ProtocolGame::parsePreyRerollPrice(const InputMessagePtr& msg)
{
    msg->getU32(); // reroll price
    msg->getU8(); // wildcard
    msg->getU8(); // selectCreatureDirectly price (5 in tibia)

    // Prey Task
    msg->getU32();
    msg->getU32();
    msg->getU8();
    msg->getU8();
}

void ProtocolGame::getImbuementInfo(const InputMessagePtr& msg)
{
    msg->getU32(); // imbuid
    msg->getString(); // name
    msg->getString(); // description
    msg->getString(); // subgroup

    msg->getU16(); // iconId
    msg->getU32(); // duration

    msg->getU8(); // is premium

    const uint8_t itemsSize = msg->getU8(); // items size
    for (uint8_t i = 0; i < itemsSize; i++) {
        msg->getU16(); // item client ID
        msg->getString(); // item name
        msg->getU16(); // count
    }

    msg->getU32(); // base price
    msg->getU8(); // base percent
    msg->getU32(); // base protection
}

void ProtocolGame::parseImbuementWindow(const InputMessagePtr& msg)
{
    uint16_t itemId = msg->getU16(); // item client ID
    const ItemPtr item = Item::create(itemId);
    if (item->getId() == 0)
        stdext::throw_exception(stdext::format("unable to create item with invalid id %d", itemId));

    if (item->getClassification() > 0) {
        msg->getU8();
    }

    const uint8_t slot = msg->getU8(); // slot id
    for (uint8_t i = 0; i < slot; i++) {
        const uint8_t firstByte = msg->getU8();
        if (firstByte == 0x01) {
            getImbuementInfo(msg);
            msg->getU32(); // info >> 8
            msg->getU32(); // removecust
        }
    }

    const uint16_t imbSize = msg->getU16(); // imbuement size
    for (uint16 i = 0; i < imbSize; i++) {
        getImbuementInfo(msg);
    }

    const uint32_t neededItemsSize = msg->getU32(); // needed items size
    for (uint32_t i = 0; i < neededItemsSize; i++) {
        msg->getU16(); // item client id
        msg->getU16(); // item count
    }

    // TODO: implement imbuement window usage
}

void ProtocolGame::parseCloseImbuementWindow(const InputMessagePtr& /*msg*/)
{
    // TODO: implement close imbuement window usage
}

void ProtocolGame::parseError(const InputMessagePtr& msg)
{
    msg->getU8(); // error code
    msg->getString(); // error

    // TODO: implement error usage
}
