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

#ifndef PROTOCOLCODES_H
#define PROTOCOLCODES_H

#include "global.h"

namespace Proto {
    enum LoginServerOpts {
        LoginServerError = 10,
        LoginServerMotd = 20,
        LoginServerUpdateNeeded = 30,
        LoginServerCharacterList = 100
    };

    enum ItemOpcode {
        StaticText = 96,
        UnknownCreature = 97,
        OutdatedCreature = 98,
        Creature = 99
    };

    enum GameServerOpcodes : uint8
    {
        GameServerLoginOrPendingState       = 0xA,
        GameServerGMActions                 = 0xB,
        GameServerEnterGame                 = 0xF,
        GameServerUpdateNeeded              = 0x11,
        GameServerLoginError                = 0x14,
        GameServerLoginAdvice               = 0x15,
        GameServerLoginWait                 = 0x16,
        GameServerLoginSuccess              = 0x17,
        GameServerLoginToken                = 0x18,
        GameServerStoreButtonIndicators     = 0x19, // 1097
        GameServerPingBack                  = 0x1D,
        GameServerPing                      = 0x1E,
        GameServerChallenge                 = 0x1F,
        GameServerDeath                     = 0x28,

        // all in game opcodes must be greater than 50
        GameServerFirstGameOpcode           = 0x32,

        // otclient ONLY
        GameServerExtendedOpcode            = 0x32,

        // NOTE: add any custom opcodes in this range
        // 51 - 99
        GameServerChangeMapAwareRange       = 0x33,

        // original tibia ONLY
        GameServerFullMap                   = 0x64,
        GameServerMapTopRow                 = 0x65,
        GameServerMapRightRow               = 0x66,
        GameServerMapBottomRow              = 0x67,
        GameServerMapLeftRow                = 0x68,
        GameServerUpdateTile                = 0x69,
        GameServerCreateOnMap               = 0x6A,
        GameServerChangeOnMap               = 0x6B,
        GameServerDeleteOnMap               = 0x6C,
        GameServerMoveCreature              = 0x6D,
        GameServerOpenContainer             = 0x6E,
        GameServerCloseContainer            = 0x6F,
        GameServerCreateContainer           = 0x70,
        GameServerChangeInContainer         = 0x71,
        GameServerDeleteInContainer         = 0x72,
        GameServerSetInventory              = 0x78,
        GameServerDeleteInventory           = 0x79,
        GameServerOpenNpcTrade              = 0x7A,
        GameServerPlayerGoods               = 0x7B,
        GameServerCloseNpcTrade             = 0x7C,
        GameServerOwnTrade                  = 0x7D,
        GameServerCounterTrade              = 0x7E,
        GameServerCloseTrade                = 0x7F,
        GameServerAmbient                   = 0x82,
        GameServerGraphicalEffect           = 0x83,
        GameServerTextEffect                = 0x84,
        GameServerMissleEffect              = 0x85,
        GameServerMarkCreature              = 0x86,
        GameServerTrappers                  = 0x87,
        GameServerCreatureHealth            = 0x8C,
        GameServerCreatureLight             = 0x8D,
        GameServerCreatureOutfit            = 0x8E,
        GameServerCreatureSpeed             = 0x8F,
        GameServerCreatureSkull             = 0x90,
        GameServerCreatureParty             = 0x91,
        GameServerCreatureUnpass            = 0x92,
        GameServerCreatureMarks             = 0x93,
        GameServerCreatureType              = 0x95,
        GameServerEditText                  = 0x96,
        GameServerEditList                  = 0x97,
        GameServerBlessings                 = 0x9C,
        GameServerPreset                    = 0x9D,
        GameServerPremiumTrigger            = 0x9E, // 1038
        GameServerPlayerDataBasic           = 0x9F, // 950
        GameServerPlayerData                = 0xA0,
        GameServerPlayerSkills              = 0xA1,
        GameServerPlayerState               = 0xA2,
        GameServerClearTarget               = 0xA3,
        GameServerPlayerModes               = 0xA7,
        GameServerSpellDelay                = 0xA4, // 870
        GameServerSpellGroupDelay           = 0xA5, // 870
        GameServerMultiUseDelay             = 0xA6, // 870
        GameServerSetStoreDeepLink          = 0xA8, // 1097
        GameServerTalk                      = 0xAA,
        GameServerChannels                  = 0xAB,
        GameServerOpenChannel               = 0xAC,
        GameServerOpenPrivateChannel        = 0xAD,
        GameServerRuleViolationChannel      = 0xAE,
        GameServerRuleViolationRemove       = 0xAF,
        GameServerRuleViolationCancel       = 0xB0,
        GameServerRuleViolationLock         = 0xB1,
        GameServerOpenOwnChannel            = 0xB2,
        GameServerCloseChannel              = 0xB3,
        GameServerTextMessage               = 0xB4,
        GameServerCancelWalk                = 0xB5,
        GameServerWalkWait                  = 0xB6,
        GameServerUnjustifiedStats          = 0xB7,
        GameServerPvpSituations             = 0xB8,
        GameServerFloorChangeUp             = 0xBE,
        GameServerFloorChangeDown           = 0xBF,
        GameServerChooseOutfit              = 0xC8,
        GameServerVipAdd                    = 0xD2,
        GameServerVipState                  = 0xD3,
        GameServerVipLogout                 = 0xD4,
        GameServerTutorialHint              = 0xDC,
        GameServerAutomapFlag               = 0xDD,
        GameServerCoinBalance               = 0xDF, // 1080
        GameServerStoreError                = 0xE0, // 1080
        GameServerRequestPurchaseData       = 0xE1, // 1080
        GameServerSendShowDescription       = 0xEA,
        GameServerSendResourceBalance       = 0xEE, // market module
        GameServerQuestLog                  = 0xF0,
        GameServerQuestLine                 = 0xF1,
        GameServerCoinBalanceUpdating       = 0xF2, // 1080
        GameServerChannelEvent              = 0xF3, // 910
        GameServerItemInfo                  = 0xF4, // 910
        GameServerPlayerInventory           = 0xF5, // 910
        GameServerMarketEnter               = 0xF6, // 944
        GameServerMarketLeave               = 0xF7, // 944
        GameServerMarketDetail              = 0xF8, // 944
        GameServerMarketBrowse              = 0xF9, // 944
        GameServerModalDialog               = 0xFA, // 960
        GameServerStore                     = 0xFB, // 1080
        GameServerStoreOffers               = 0xFC, // 1080
        GameServerStoreTransactionHistory   = 0xFD, // 1080
        GameServerStoreCompletePurchase     = 0xFE, // 1080

        // 12.x +
        GameServerSendClientCheck = 0x63,
        GameServerSendGameNews = 0x98,
        GameServerSendBlessDialog = 0x9B,
        GameServerSendRestingAreaState = 0xA9,
        GameServerSendUpdateImpactTracker = 0xCC,
        GameServerSendItemsPrice = 0xCD,
        GameServerSendUpdateSupplyTracker = 0xCE,
        GameServerSendUpdateLootTracker = 0xCF,
        GameServerSendKillTrackerUpdate = 0xD1,
        GameServerSendBestiaryEntryChanged = 0xD9,
        GameServerSendDailyRewardCollectionState = 0xDE,
        GameServerSendOpenRewardWall = 0xE2,
        GameServerSendDailyReward = 0xE4,
        GameServerSendRewardHistory = 0xE5,
        GameServerSendPreyTimeLeft = 0xE7,
        GameServerSendPreyData = 0xE8,
        GameServerSendPreyRerollPrice = 0xE9,
        GameServerSendImbuementWindow = 0xEB,
        GameServerSendCloseImbuementWindow = 0xEC,
        GameServerSendError = 0xED,
        GameServerSendCollectionResource = 0xEE,
        GameServerSendTibiaTime = 0xEF

    };

    enum ClientOpcodes : uint8
    {
        ClientEnterAccount                  = 0x1,
        ClientPendingGame                   = 0xA,
        ClientEnterGame                     = 0xF,
        ClientLeaveGame                     = 0x14,
        ClientPing                          = 0x1D,
        ClientPingBack                      = 0x1E,

        // all in game opcodes must be equal or greater than 50
        ClientFirstGameOpcode               = 0x32,

        // otclient ONLY
        ClientExtendedOpcode                = 0x32,
        ClientChangeMapAwareRange           = 0x33,

        // NOTE: add any custom opcodes in this range
        // 51 - 99

        // original tibia ONLY
        ClientAutoWalk                      = 0x64,
        ClientWalkNorth                     = 0x65,
        ClientWalkEast                      = 0x66,
        ClientWalkSouth                     = 0x67,
        ClientWalkWest                      = 0x68,
        ClientStop                          = 0x69,
        ClientWalkNorthEast                 = 0x6A,
        ClientWalkSouthEast                 = 0x6B,
        ClientWalkSouthWest                 = 0x6C,
        ClientWalkNorthWest                 = 0x6D,
        ClientTurnNorth                     = 0x6F,
        ClientTurnEast                      = 0x70,
        ClientTurnSouth                     = 0x71,
        ClientTurnWest                      = 0x72,
        ClientEquipItem                     = 0x77, // 910
        ClientMove                          = 0x78,
        ClientInspectNpcTrade               = 0x79,
        ClientBuyItem                       = 0x7A,
        ClientSellItem                      = 0x7B,
        ClientCloseNpcTrade                 = 0x7C,
        ClientRequestTrade                  = 0x7D,
        ClientInspectTrade                  = 0x7E,
        ClientAcceptTrade                   = 0x7F,
        ClientRejectTrade                   = 0x80,
        ClientUseItem                       = 0x82,
        ClientUseItemWith                   = 0x83,
        ClientUseOnCreature                 = 0x84,
        ClientRotateItem                    = 0x85,
        ClientCloseContainer                = 0x87,
        ClientUpContainer                   = 0x88,
        ClientEditText                      = 0x89,
        ClientEditList                      = 0x8A,
        ClientLook                          = 0x8C,
        ClientLookCreature                  = 0x8D,
        ClientTalk                          = 0x96,
        ClientRequestChannels               = 0x97,
        ClientJoinChannel                   = 0x98,
        ClientLeaveChannel                  = 0x99,
        ClientOpenPrivateChannel            = 0x9A,
        ClientOpenRuleViolation             = 0x9B,
        ClientCloseRuleViolation            = 0x9C,
        ClientCancelRuleViolation           = 0x9D,
        ClientCloseNpcChannel               = 0x9E,
        ClientChangeFightModes              = 0xA0,
        ClientAttack                        = 0xA1,
        ClientFollow                        = 0xA2,
        ClientInviteToParty                 = 0xA3,
        ClientJoinParty                     = 0xA4,
        ClientRevokeInvitation              = 0xA5,
        ClientPassLeadership                = 0xA6,
        ClientLeaveParty                    = 0xA7,
        ClientShareExperience               = 0xA8,
        ClientDisbandParty                  = 0xA9,
        ClientOpenOwnChannel                = 0xAA,
        ClientInviteToOwnChannel            = 0xAB,
        ClientExcludeFromOwnChannel         = 0xAC,
        ClientCancelAttackAndFollow         = 0xBE,
        ClientUpdateTile                    = 0xC9,
        ClientRefreshContainer              = 0xCA,
        ClientBrowseField                   = 0xCB,
        ClientSeekInContainer               = 0xCC,
        ClientRequestOutfit                 = 0xD2,
        ClientChangeOutfit                  = 0xD3,
        ClientMount                         = 0xD4, // 870
        ClientAddVip                        = 0xDC,
        ClientRemoveVip                     = 0xDD,
        ClientEditVip                       = 0xDE,
        ClientBugReport                     = 0xE6,
        ClientRuleViolation                 = 0xE7,
        ClientDebugReport                   = 0xE8,
        ClientTransferCoins                 = 0xEF, // 1080
        ClientRequestQuestLog               = 0xF0,
        ClientRequestQuestLine              = 0xF1,
        ClientNewRuleViolation              = 0xF2, // 910
        ClientRequestItemInfo               = 0xF3, // 910
        ClientMarketLeave                   = 0xF4, // 944
        ClientMarketBrowse                  = 0xF5, // 944
        ClientMarketCreate                  = 0xF6, // 944
        ClientMarketCancel                  = 0xF7, // 944
        ClientMarketAccept                  = 0xF8, // 944
        ClientAnswerModalDialog             = 0xF9, // 960
        ClientOpenStore                     = 0xFA, // 1080
        ClientRequestStoreOffers            = 0xFB, // 1080
        ClientBuyStoreOffer                 = 0xFC, // 1080
        ClientOpenTransactionHistory        = 0xFD, // 1080
        ClientRequestTransactionHistory     = 0xFE  // 1080
    };

    enum CreatureType {
        CreatureTypePlayer = 0,
        CreatureTypeMonster,
        CreatureTypeNpc,
        CreatureTypeSummonOwn,
        CreatureTypeSummonOther,
        CreatureTypeHidden,
        CreatureTypeUnknown = 0xFF
    };

    enum CreaturesIdRange {
        PlayerStartId = 0x10000000,
        PlayerEndId = 0x40000000,
        MonsterStartId = 0x40000000,
        MonsterEndId = 0x80000000,
        NpcStartId = 0x80000000,
        NpcEndId = 0xffffffff
    };

    void buildMessageModesMap(int version);
    Otc::MessageMode translateMessageModeFromServer(uint8 mode);
    uint8 translateMessageModeToServer(Otc::MessageMode mode);
}

#endif
