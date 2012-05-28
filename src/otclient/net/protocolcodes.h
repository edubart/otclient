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

#ifndef PROTOCOLCODES_H
#define PROTOCOLCODES_H

#include <otclient/global.h>

#if !(PROTOCOL == 810) && \
    !(PROTOCOL == 854) && \
    !(PROTOCOL >= 860 && PROTOCOL <= 862) && \
    !(PROTOCOL >= 870 && PROTOCOL <= 871) && \
    !(PROTOCOL >= 910 && PROTOCOL <= 953)
#error "the supplied protocol version is not supported"
#endif

namespace Proto {
#ifdef CIPSOFT_RSA
    constexpr const char* RSA = "1321277432058722840622950990822933849527763264961655079678763618"
                          "4334395343554449668205332383339435179772895415509701210392836078"
                          "6959821132214473291575712138800495033169914814069637740318278150"
                          "2907336840325241747827401343576296990629870233111328210165697754"
                          "88792221429527047321331896351555606801473202394175817";
#else
    constexpr const char* RSA = "1091201329673994292788609605089955415282375029027981291234687579"
                          "3726629149257644633073969600111060390723088861007265581882535850"
                          "3429057592827629436413108566029093628212635953836686562675849720"
                          "6207862794310902180176810615217550567108238764764442605581471797"
                          "07119674283982419152118103759076030616683978566631413";
#endif

    constexpr int PicSignature = 0x4F8C231A; // 953 pic signature
    constexpr int ClientVersion = PROTOCOL;

    enum OsTypes {
        OsLinux = 1,
        OsWindows = 2,
        OsFlash = 3,
        OsOtclientLinux = 10,
        OsOtclientWindows = 11,
        OsOtclientMac = 12,
    };

#ifdef OSTYPE
    constexpr int ClientOs = OSTYPE;
#elif defined WIN32
    constexpr int ClientOs = OsOtclientWindows;
#else
    constexpr int ClientOs = OsOtclientLinux;
#endif

    enum LoginServerOpts {
        LoginServerError = 10,
        LoginServerMotd = 20,
        LoginServerUpdateNeeded = 30,
        LoginServerCharacterList = 100
    };

    enum CreatureOpcode {
        UnknownCreature = 97,
        OutdatedCreature = 98,
        Creature = 99
    };

    enum GameServerOpcodes {
        GameServerInitGame                  = 10,
        GameServerGMActions                 = 11,
        GameServerLoginError                = 20,
        GameServerLoginAdvice               = 21,
        GameServerLoginWait                 = 22,
        GameServerPingBack                  = 29,
        GameServerPing                      = 30,
        GameServerChallange                 = 31,
        GameServerDeath                     = 40,

        // all in game opcodes must be greater than 50
        GameServerFirstGameOpcode           = 50,

        // otclient ONLY
        GameServerExtendedOpcode            = 50,

        // NOTE: add any custom opcodes in this range
        // 51 - 99

        // original tibia ONLY
        GameServerFullMap                   = 100,
        GameServerMapTopRow                 = 101,
        GameServerMapRightRow               = 102,
        GameServerMapBottomRow              = 103,
        GameServerMapLeftRow                = 104,
        GameServerUpdateTile                = 105,
        GameServerCreateOnMap               = 106,
        GameServerChangeOnMap               = 107,
        GameServerDeleteOnMap               = 108,
        GameServerMoveCreature              = 109,
        GameServerOpenContainer             = 110,
        GameServerCloseContainer            = 111,
        GameServerCreateContainer           = 112,
        GameServerChangeInContainer         = 113,
        GameServerDeleteInContainer         = 114,
        GameServerSetInventory              = 120,
        GameServerDeleteInventory           = 121,
        GameServerOpenNpcTrade              = 122,
        GameServerPlayerGoods               = 123,
        GameServerCloseNpcTrade             = 124,
        GameServerOwnTrade                  = 125,
        GameServerCounterTrade              = 126,
        GameServerCloseTrade                = 127,
        GameServerAmbient                   = 130,
        GameServerGraphicalEffect           = 131,
        GameServerTextEffect                = 132,
        GameServerMissleEffect              = 133,
        GameServerMarkCreature              = 134,
        GameServerTrappers                  = 135,
        GameServerCreatureHealth            = 140,
        GameServerCreatureLight             = 141,
        GameServerCreatureOutfit            = 142,
        GameServerCreatureSpeed             = 143,
        GameServerCreatureSkull             = 144,
        GameServerCreatureParty             = 145,
        GameServerCreatureUnpass            = 146,
        GameServerEditText                  = 150,
        GameServerEditList                  = 151,
        GameServerPlayerDataBasic           = 159, // 910
        GameServerPlayerData                = 160,
        GameServerPlayerSkills              = 161,
        GameServerPlayerState               = 162,
        GameServerClearTarget               = 163,
        GameServerSpellDelay                = 164, // 870
        GameServerSpellGroupDelay           = 165, // 870
        GameServerMultiUseDelay             = 166, // 870
        GameServerTalk                      = 170,
        GameServerChannels                  = 171,
        GameServerOpenChannel               = 172,
        GameServerOpenPrivateChannel        = 173,
        GameServerRuleViolationChannel      = 174,
        GameServerRuleViolationRemove       = 175,
        GameServerRuleViolationCancel       = 176,
        GameServerRuleViolationLock         = 177,
        GameServerOpenOwnChannel            = 178,
        GameServerCloseChannel              = 179,
        GameServerTextMessage               = 180,
        GameServerCancelWalk                = 181,
        GameServerWalkWait                  = 182,
        GameServerFloorChangeUp             = 190,
        GameServerFloorChangeDown           = 191,
        GameServerChooseOutfit              = 200,
        GameServerVipAdd                    = 210,
        GameServerVipLogin                  = 211,
        GameServerVipLogout                 = 212,
        GameServerTutorialHint              = 220,
        GameServerAutomapFlag               = 221,
        GameServerQuestLog                  = 240,
        GameServerQuestLine                 = 241,
        GameServerChannelEvent              = 243, // 910
        GameServerItemInfo                  = 244, // 910
        GameServerPlayerInventory           = 245, // 910
        GameServerMarketEnter               = 246, // 944
        GameServerMarketLeave               = 247, // 944
        GameServerMarketDetail              = 248, // 944
        GameServerMarketBrowse              = 249, // 944
    };

    enum ClientOpcodes {
        ClientEnterAccount                  = 1,
        ClientEnterGame                     = 10,
        ClientLeaveGame                     = 20,
        ClientPing                          = 29,
        ClientPingBack                      = 30,

        // all in game opcodes must be equal or greater than 50
        ClientFirstGameOpcode               = 50,

        // otclient ONLY
        ClientExtendedOpcode                = 50,

        // NOTE: add any custom opcodes in this range
        // 51 - 99

        // original tibia ONLY
        ClientAutoWalk                      = 100,
        ClientWalkNorth                     = 101,
        ClientWalkEast                      = 102,
        ClientWalkSouth                     = 103,
        ClientWalkWest                      = 104,
        ClientStop                          = 105,
        ClientWalkNorthEast                 = 106,
        ClientWalkSouthEast                 = 107,
        ClientWalkSouthWest                 = 108,
        ClientWalkNorthWest                 = 109,
        ClientTurnNorth                     = 111,
        ClientTurnEast                      = 112,
        ClientTurnSouth                     = 113,
        ClientTurnWest                      = 114,
        ClientEquipItem                     = 119, // 910
        ClientMove                          = 120,
        ClientInspectNpcTrade               = 121,
        ClientBuyItem                       = 122,
        ClientSellItem                      = 123,
        ClientCloseNpcTrade                 = 124,
        ClientRequestTrade                  = 125,
        ClientInspectTrade                  = 126,
        ClientAcceptTrade                   = 127,
        ClientRejectTrade                   = 128,
        ClientUseItem                       = 130,
        ClientUseItemWith                   = 131,
        ClientUseOnCreature                 = 132,
        ClientRotateItem                    = 133,
        ClientCloseContainer                = 135,
        ClientUpContainer                   = 136,
        ClientEditText                      = 137,
        ClientEditList                      = 138,
        ClientLook                          = 140,
        ClientTalk                          = 150,
        ClientRequestChannels               = 151,
        ClientJoinChannel                   = 152,
        ClientLeaveChannel                  = 153,
        ClientOpenPrivateChannel            = 154,
        ClientCloseNpcChannel               = 158,
        ClientChangeFightModes              = 160,
        ClientAttack                        = 161,
        ClientFollow                        = 162,
        ClientInviteToParty                 = 163,
        ClientJoinParty                     = 164,
        ClientRevokeInvitation              = 165,
        ClientPassLeadership                = 166,
        ClientLeaveParty                    = 167,
        ClientShareExperience               = 168,
        ClientDisbandParty                  = 169,
        ClientOpenOwnChannel                = 170,
        ClientInviteToOwnChannel            = 171,
        ClientExcludeFromOwnChannel         = 172,
        ClientCancelAttackAndFollow         = 190,
        ClientRefreshContainer              = 202,
        ClientRequestOutfit                 = 210,
        ClientChangeOutfit                  = 211,
        ClientMount                         = 212, // 870
        ClientAddVip                        = 220,
        ClientRemoveVip                     = 221,
        ClientBugReport                     = 230,
        ClientRuleViolation                 = 231,
        ClientDebugReport                   = 232,
        ClientRequestQuestLog               = 240,
        ClientRequestQuestLine              = 241,
        ClientNewRuleViolation              = 242, // 910
        ClientRequestItemInfo               = 243, // 910
        ClientMarketLeave                   = 244, // 944
        ClientMarketBrowse                  = 245, // 944
        ClientMarketCreate                  = 246, // 944
        ClientMarketCancel                  = 247, // 944
        ClientMarketAccept                  = 248, // 944
    };

    enum ServerSpeakType {
#if PROTOCOL>=910
        ServerSpeakSay = 1,
        ServerSpeakWhisper,
        ServerSpeakYell,
        ServerSpeakPrivateFrom,         // new
        ServerSpeakPrivateTo,           // new
        ServerSpeakChannelManagement,   // new
        ServerSpeakChannelYellow,
        ServerSpeakChannelOrange,
        ServerSpeakSpell,               // new
        ServerSpeakPrivatePlayerToNpc,
        ServerSpeakPrivateNpcToPlayer,
        ServerSpeakBroadcast,
        ServerSpeakChannelRed,
        ServerSpeakPrivateRedFrom,      // new
        ServerSpeakPrivateRedTo,        // new
        // 16 - 33
        ServerSpeakMonsterSay = 34,
        ServerSpeakMonsterYell,

        // unsupported
        ServerSpeakRVRChannel = 255,
        ServerSpeakRVRAnswer,
        ServerSpeakRVRContinue,
        ServerSpeakChannelRed2,
        ServerSpeakChannelWhite
#elif PROTOCOL>=861
        ServerSpeakSay = 1,
        ServerSpeakWhisper,
        ServerSpeakYell,
        ServerSpeakPrivatePlayerToNpc,
        ServerSpeakPrivateNpcToPlayer,
        ServerSpeakPrivateTo,
        ServerSpeakPrivateFrom = ServerSpeakPrivateTo,
        ServerSpeakChannelYellow,
        ServerSpeakChannelWhite,
        ServerSpeakBroadcast,
        ServerSpeakChannelRed,
        ServerSpeakPrivateRedTo,
        ServerSpeakPrivateRedFrom = ServerSpeakPrivateRedTo,
        ServerSpeakChannelOrange,
        ServerSpeakMonsterSay,
        ServerSpeakMonsterYell,

        // unsupported
        ServerSpeakRVRChannel = 255,
        ServerSpeakRVRAnswer,
        ServerSpeakRVRContinue,
        ServerSpeakChannelRed2
#elif PROTOCOL>=854
        ServerSpeakSay = 1,
        ServerSpeakWhisper,
        ServerSpeakYell,
        ServerSpeakPrivatePlayerToNpc,
        ServerSpeakPrivateNpcToPlayer,
        ServerSpeakPrivateTo,
        ServerSpeakPrivateFrom = ServerSpeakPrivateTo,
        ServerSpeakChannelYellow,
        ServerSpeakChannelWhite,
        ServerSpeakRVRChannel,
        ServerSpeakRVRAnswer,
        ServerSpeakRVRContinue,
        ServerSpeakBroadcast,
        ServerSpeakChannelRed,
        ServerSpeakPrivateRedTo,
        ServerSpeakPrivateRedFrom = ServerSpeakPrivateRedTo,
        ServerSpeakChannelOrange,
        // 16
        ServerSpeakChannelRed2 = 17,
        // 18
        ServerSpeakMonsterSay = 19,
        ServerSpeakMonsterYell
#elif PROTOCOL>=810
        ServerSpeakSay = 1,
        ServerSpeakWhisper,
        ServerSpeakYell,
        ServerSpeakPrivateTo,
        ServerSpeakPrivateFrom = ServerSpeakPrivateTo,
        ServerSpeakChannelYellow,
        ServerSpeakRVRChannel,
        ServerSpeakRVRAnswer,
        ServerSpeakRVRContinue,
        ServerSpeakBroadcast,
        ServerSpeakChannelRed,
        ServerSpeakPrivateRedTo,
        ServerSpeakPrivateRedFrom = ServerSpeakPrivateRedTo,
        ServerSpeakChannelOrange,
        // 13
        ServerSpeakChannelRed2 = 14,
        // 15
        ServerSpeakMonsterSay = 16,
        ServerSpeakMonsterYell,

        // unsupported
        ServerSpeakPrivatePlayerToNpc = 255,
        ServerSpeakPrivateNpcToPlayer,
        ServerSpeakChannelWhite
#endif
    };

    enum MessageTypes {
#if PROTOCOL>=910
        // 1-3
        MessageConsoleBlue = 4,         // old
        // 5-11
        MessageConsoleRed = 12,         // old
        // 13-15
        MessageStatusDefault = 16,      // old
        MessageWarning,                 // old
        MessageEventAdvance,            // old
        MessageStatusSmall,             // old
        MessageInfoDescription,         // old
        MessageDamageDealt,             // new
        MessageDamageReceived,          // new
        MessageHealed,                  // new
        MessageExperience,              // new
        MessageDamageOthers,            // new
        MessageHealedOthers,            // new
        MessageExperienceOthers,        // new
        MessageEventDefault,            // old
        MessageLoot,                    // new
        MessageTradeNpc,                // unused
        MessageChannelGuild,            // new
        MessagePartyManagment,          // unused
        MessageParty,                   // unused
        MessageEventOrange,             // old
        MessageConsoleOrange,           // old
        MessageReport,                  // unused
        MessageHotkeyUse,               // unused
        MessageTutorialHint,            // unused

        // unsupported
        MessageConsoleOrange2 = 255
#elif PROTOCOL>=861
        MessageConsoleOrange = 13,
        MessageConsoleOrange2,
        MessageWarning,
        MessageEventAdvance,
        MessageEventDefault,
        MessageStatusDefault,
        MessageInfoDescription,
        MessageStatusSmall,
        MessageConsoleBlue,
        MessageConsoleRed
#elif PROTOCOL>=854
        MessageConsoleRed = 18,
        MessageConsoleOrange,
        MessageConsoleOrange2,
        MessageWarning,
        MessageEventAdvance,
        MessageEventDefault,
        MessageStatusDefault,
        MessageInfoDescription,
        MessageStatusSmall,
        MessageConsoleBlue
#elif PROTOCOL>=810
        MessageWarning = 18,
        MessageEventAdvance,
        MessageEventDefault,
        MessageStatusDefault,
        MessageInfoDescription,
        MessageStatusSmall,
        MessageConsoleBlue,
        MessageConsoleRed,

        // unsupported
        MessageConsoleOrange = 255,
        MessageConsoleOrange2,
#endif
    };

    enum CreatureType {
        CreatureTypePlayer = 0,
        CreatureTypeMonster,
        CreatureTypeNpc
    };

    enum CreaturesIdRange {
        PlayerStartId = 0x10000000,
        PlayerEndId = 0x40000000,
        MonsterStartId = 0x40000000,
        MonsterEndId = 0x80000000,
        NpcStartId = 0x80000000,
        NpcEndId = 0xffffffff
    };

    inline Otc::SpeakType translateSpeakTypeFromServer(int type) {
        switch(type) {
            case Proto::ServerSpeakSay: return Otc::SpeakSay;
            case Proto::ServerSpeakWhisper: return Otc::SpeakWhisper;
            case Proto::ServerSpeakYell: return Otc::SpeakYell;
            case Proto::ServerSpeakMonsterSay: return Otc::SpeakMonsterSay;
            case Proto::ServerSpeakMonsterYell: return Otc::SpeakMonsterYell;
            case Proto::ServerSpeakPrivateNpcToPlayer: return Otc::SpeakPrivateNpcToPlayer;
            case Proto::ServerSpeakChannelYellow: return Otc::SpeakChannelYellow;
            case Proto::ServerSpeakChannelWhite: return Otc::SpeakChannelWhite;
            case Proto::ServerSpeakChannelRed: return Otc::SpeakChannelRed;
            case Proto::ServerSpeakChannelRed2: return Otc::SpeakChannelRed;
            case Proto::ServerSpeakChannelOrange: return Otc::SpeakChannelOrange;
            case Proto::ServerSpeakPrivateTo: return Otc::SpeakPrivate;
            case Proto::ServerSpeakPrivatePlayerToNpc: return Otc::SpeakPrivate;
            case Proto::ServerSpeakBroadcast: return Otc::SpeakBroadcast;
            case Proto::ServerSpeakPrivateRedTo: return Otc::SpeakPrivateRed;
            default:
                logError("unknown protocol speak type %d", type);
                return Otc::SpeakSay;
        }
    }

    inline Proto::ServerSpeakType translateSpeakTypeToServer(int type) {
        switch(type) {
            case Otc::SpeakSay: return Proto::ServerSpeakSay;
            case Otc::SpeakWhisper: return Proto::ServerSpeakWhisper;
            case Otc::SpeakYell: return Proto::ServerSpeakYell;
            case Otc::SpeakBroadcast: return Proto::ServerSpeakBroadcast;
            case Otc::SpeakPrivate: return Proto::ServerSpeakPrivateFrom;
            case Otc::SpeakPrivateRed: return Proto::ServerSpeakPrivateRedFrom;
            case Otc::SpeakPrivatePlayerToNpc: return Proto::ServerSpeakPrivatePlayerToNpc;
            case Otc::SpeakPrivateNpcToPlayer: return Proto::ServerSpeakPrivateNpcToPlayer;
            case Otc::SpeakChannelYellow: return Proto::ServerSpeakChannelYellow;
            case Otc::SpeakChannelWhite: return Proto::ServerSpeakChannelWhite;
            case Otc::SpeakChannelRed: return Proto::ServerSpeakChannelRed;
            case Otc::SpeakChannelOrange: return Proto::ServerSpeakChannelOrange;
            case Otc::SpeakMonsterSay: return Proto::ServerSpeakMonsterSay;
            case Otc::SpeakMonsterYell: return Proto::ServerSpeakMonsterYell;
            default:
                logError("unknown protocol speak type desc %d", type);
                return Proto::ServerSpeakSay;
        }
    }

    inline std::string translateTextMessageType(int type) {
        switch(type) {
            case Proto::MessageConsoleOrange: return "consoleOrange";
            case Proto::MessageConsoleOrange2: return "consoleOrange";
            case Proto::MessageWarning: return "warning";
            case Proto::MessageEventAdvance: return "eventAdvance";
            case Proto::MessageEventDefault: return "eventDefault";
            case Proto::MessageStatusDefault: return "statusDefault";
            case Proto::MessageInfoDescription: return "infoDescription";
            case Proto::MessageStatusSmall: return "statusSmall";
            case Proto::MessageConsoleBlue: return "consoleBlue";
            case Proto::MessageConsoleRed: return "consoleRed";
            default:
                logError("unknown protocol text message type %d", type);
                return "unknown";
        }
    }
}

#endif
