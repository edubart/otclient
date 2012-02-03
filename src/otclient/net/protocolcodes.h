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

#if PROTOCOL < 860 || PROTOCOL > 862
#error "the supplied protocol version is not supported"
#endif

namespace Proto {
#ifdef CIPSOFT_RSA
    static const char* RSA = "1321277432058722840622950990822933849527763264961655079678763618"
                             "4334395343554449668205332383339435179772895415509701210392836078"
                             "6959821132214473291575712138800495033169914814069637740318278150"
                             "2907336840325241747827401343576296990629870233111328210165697754"
                             "88792221429527047321331896351555606801473202394175817";
#else
    static const char* RSA = "1091201329673994292788609605089955415282375029027981291234687579"
                             "3726629149257644633073969600111060390723088861007265581882535850"
                             "3429057592827629436413108566029093628212635953836686562675849720"
                             "6207862794310902180176810615217550567108238764764442605581471797"
                             "07119674283982419152118103759076030616683978566631413";
#endif

    static const int ClientVersion = PROTOCOL;
    static const int PicSignature = 0x4E119CBF;

#if PROTOCOL==860
    const int NumViolationReasons = 20;
#elif PROTOCOL==861 || PROTOCOL==862
    const int NumViolationReasons = 19;
#endif

    enum OsTypes {
        OsWindow = 1,
        OsLinux = 2,
        OsMac = 3,
        OsBrowser = 4
    };

    enum LoginServerOpts {
        LoginServerError = 10,
        LoginServerMotd = 20,
        LoginServerUpdateNeeded = 30,
        LoginServerCharacterList = 100
    };

    enum GameServerOpts {
        GameServerInitGame = 10,
        GameServerGMActions = 11, // deprecated in last tibia
        GameServerLoginError = 20,
        GameServerLoginAdvice = 21,
        GameServerLoginWait = 22,
        GameServerPing = 30,
        GameServerChallange = 31,
        GameServerDead = 40,
        GameServerFullMap = 100,
        GameServerMapTopRow = 101,
        GameServerMapRightRow = 102,
        GameServerMapBottomRow = 103,
        GameServerMapLeftRow = 104,
        GameServerTileData = 105,
        GameServerCreateOnMap = 106,
        GameServerChangeOnMap = 107,
        GameServerDeleteOnMap = 108,
        GameServerMoveCreature = 109,
        GameServerOpenContainer = 110,
        GameServerCloseContainer = 111,
        GameServerCreateContainer = 112,
        GameServerChangeInContainer = 113,
        GameServerDeleteInContainer = 114,
        GameServerSetInventory = 120,
        GameServerDeleteInventory = 121,
        GameServerNpcOffer = 122,
        GameServerPlayerGoods = 123,
        GameServerCloseNpcTrade = 124,
        GameServerOwnOffer = 125,
        GameServerCounterOffer = 126,
        GameServerCloseTrade = 127,
        GameServerAmbient = 130,
        GameServerGraphicalEffect = 131,
        GameServerTextEffect = 132, // deprecated in last tibia
        GameServerMissleEffect = 133,
        GameServerMarkCreature = 134,
        GameServerTrappers = 135,
        GameServerCreatureHealth = 140,
        GameServerCreatureLight = 141,
        GameServerCreatureOutfit = 142,
        GameServerCreatureSpeed = 143,
        GameServerCreatureSkull = 144,
        GameServerCreatureParty = 145,
        GameServerCreatureUnpass = 146,
        GameServerEditText = 150,
        GameServerEditList = 151,
        GameServerPlayerData = 160,
        GameServerPlayerSkills = 161,
        GameServerPlayerState = 162,
        GameServerClearTarget = 163,
        GameServerSpellDelay = 164,
        GameServerSpellGroupDelay = 165,
        GameServerTalk = 170,
        GameServerChannels = 171,
        GameServerOpenChannel = 172,
        GameServerOpenPrivateChannel = 173,
        GameServerRuleViolationChannel = 174, // deprecated in last tibia
        GameServerRuleViolationRemove = 175, // deprecated in last tibia
        GameServerRuleViolationCancel = 176, // deprecated in last tibia
        GameServerRuleViolationLock = 177, // deprecated in last tibia
        GameServerOpenOwnChannel = 178,
        GameServerCloseChannel = 179,
        GameServerMessage = 180,
        GameServerSnapBack = 181,
        GameServerWait = 182,
        GameServerTopFloor = 190,
        GameServerBottomFloor = 191,
        GameServerOutfit = 200,
        GameServerBuddyData = 210,
        GameServerBuddyLogin = 211,
        GameServerBuddyLogout = 212,
        GameServerTutorialHint = 220,
        GameServerAutomapFlag = 221,
        GameServerQuestLog = 240,
        GameServerQuestLine = 241,
        GameServerChannelEvent = 243,
        GameServerObjectInfo = 244,
        GameServerPlayerInventory = 245
    };

    enum ClientOpts {
        ClientEnterAccount = 1,
        ClientEnterGame = 10,
        ClientQuitGame = 20,
        ClientPingBack = 30,
        ClientGoPath = 100,
        ClientGoNorth = 101,
        ClientGoEast = 102,
        ClientGoSouth = 103,
        ClientGoWest = 104,
        ClientStop = 105,
        ClientGoNorthEast = 106,
        ClientGoSouthEast = 107,
        ClientGoSouthWest = 108,
        ClientGoNorthWest = 109,
        ClientRotateNorth = 111,
        ClientRotateEast = 112,
        ClientRotateSouth = 113,
        ClientRotateWest = 114,
        ClientEquipObject = 119,
        ClientMoveObject = 120,
        ClientInspectNpcTrade = 121,
        ClientBuyObject = 122,
        ClientSellObject = 123,
        ClientCloseNpcTrade = 124,
        ClientTradeObject = 125,
        ClientInspectTrade = 126,
        ClientAcceptTrade = 127,
        ClientRejectTrade = 128,
        ClientUseObject = 130,
        ClientUseTwoObjects = 131,
        ClientUseOnCreature = 132,
        ClientTurnObject = 133,
        ClientCloseContainer = 135,
        ClientUpContainer = 136,
        ClientEditText = 137,
        ClientEditList = 138,
        ClientLook = 140,
        ClientTalk = 150,
        ClientGetChannels = 151,
        ClientJoinChannel = 152,
        ClientLeaveChannel = 153,
        ClientOpenPrivateChannel = 154,
        ClientCloseNpcChannel = 158,
        ClientSetTactics = 160,
        ClientAttack = 161,
        ClientFollow = 162,
        ClientInviteToParty = 163,
        ClientJoinParty = 164,
        ClientRevokeInvitation = 165,
        ClientPassLeadership = 166,
        ClientLeaveParty = 167,
        ClientShareExperience = 168,
        ClientDisbandParty = 169,
        ClientOpenChannel = 170,
        ClientInviteToChannel = 171,
        ClientExcludeFromChannel = 172,
        ClientCancel = 190,
        ClientRefreshContainer = 202,
        ClientGetOutfit = 210,
        ClientSetOutfit = 211,
        ClientMount = 212,
        ClientAddBuddy = 220,
        ClientRemoveBuddy = 221,
        ClientBugReport = 230,
        ClientErrorFileEntry = 232,
        ClientGetQuestLog = 240,
        ClientGetQuestLine = 241,
        ClientRuleViolationReport = 242,
        ClientGetObjectInfo = 243
    };

    enum SpeakTypes {
#if PROTOCOL==861 || PROTOCOL==862
        SpeakSay = 1,
        SpeakWhisper,
        SpeakYell,
        SpeakPrivatePlayerToNpc,
        SpeakPrivateNpcToPlayer,
        SpeakPrivate,
        SpeakChannelYellow,
        SpeakChannelWhite,
        SpeakBroadcast,
        SpeakChannelRed,
        SpeakPrivateRed,
        SpeakChannelOrange,
        SpeakMonsterSay,
        SpeakMonsterYell,

        // removed
        ServerSpeakRVRChannel = 255,
        ServerSpeakRVRAnswer,
        ServerSpeakRVRContinue,
        ServerSpeakChannelRed2
#elif PROTOCOL==860
        ServerSpeakSay = 1,
        ServerSpeakWhisper,
        ServerSpeakYell,
        ServerSpeakPrivatePlayerToNpc,
        ServerSpeakPrivateNpcToPlayer,
        ServerSpeakPrivate,
        ServerSpeakChannelYellow,
        ServerSpeakChannelWhite,
        ServerSpeakRVRChannel,
        ServerSpeakRVRAnswer,
        ServerSpeakRVRContinue,
        ServerSpeakBroadcast,
        ServerSpeakChannelRed,
        ServerSpeakPrivateRed,
        ServerSpeakChannelOrange,
        ServerSpeakChannelRed2 = 17,
        ServerSpeakMonsterSay = 19,
        ServerSpeakMonsterYell
#endif
    };

    enum MessageTypes {
#if PROTOCOL==862 || PROTOCOL==861
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
#elif PROTOCOL==860
        MessageConsoleRed = 18,
        MessageConsoleOrange,
        MessageConsoleOrange2,
        MessageWarning,
        MessageEventAdvance,
        MessageEventDefault,
        MessageStatusDefault,
        MessageInfoDescription ,
        MessageStatusSmall,
        MessageConsoleBlue
#endif
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
            case Proto::ServerSpeakPrivate: return Otc::SpeakPrivate;
            case Proto::ServerSpeakPrivatePlayerToNpc: return Otc::SpeakPrivate;
            case Proto::ServerSpeakBroadcast: return Otc::SpeakBroadcast;
            case Proto::ServerSpeakPrivateRed: return Otc::SpeakPrivateRed;
            default:
                logError("unknown protocol speak type ", type);
                return Otc::SpeakSay;
        }
    }

    inline Proto::ServerSpeakType translateSpeakTypeToServer(int type) {
        switch(type) {
            case Otc::SpeakSay: return Proto::ServerSpeakSay;
            case Otc::SpeakWhisper: return Proto::ServerSpeakWhisper;
            case Otc::SpeakYell: return Proto::ServerSpeakYell;
            case Otc::SpeakBroadcast: return Proto::ServerSpeakBroadcast;
            case Otc::SpeakPrivate: return Proto::ServerSpeakPrivate;
            case Otc::SpeakPrivateRed: return Proto::ServerSpeakPrivateRed;
            case Otc::SpeakPrivatePlayerToNpc: return Proto::ServerSpeakPrivatePlayerToNpc;
            case Otc::SpeakPrivateNpcToPlayer: return Proto::ServerSpeakPrivateNpcToPlayer;
            case Otc::SpeakChannelYellow: return Proto::ServerSpeakChannelYellow;
            case Otc::SpeakChannelWhite: return Proto::ServerSpeakChannelWhite;
            case Otc::SpeakChannelRed: return Proto::ServerSpeakChannelRed;
            case Otc::SpeakChannelOrange: return Proto::ServerSpeakChannelOrange;
            case Otc::SpeakMonsterSay: return Proto::ServerSpeakMonsterSay;
            case Otc::SpeakMonsterYell: return Proto::ServerSpeakMonsterYell;
            default:
                logError("unknown protocol speak type desc ", type);
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
                logError("unknown protocol text message type ", type);
                return "unknown";
        }
    }
}

#endif
