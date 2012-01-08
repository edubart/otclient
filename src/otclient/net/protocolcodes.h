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

#if PROTOCOL != 860 && PROTOCOL != 862
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
        GameServerTextEffect = 132, // absolate in last tibia?
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
        GameServerPrivateChannel = 173,
        GameServerRuleViolation = 174, // absolate in last tibia?
        GameServerRuleViolation1 = 175, // absolate in last tibia?
        GameServerRuleViolation2 = 176, // absolate in last tibia?
        GameServerRuleViolation3 = 177, // absolate in last tibia?
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
        ClientPrivateChannel = 154,
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
#if PROTOCOL==862
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
        SpeakRVRChannel = 255,
        SpeakRVRAnswer,
        SpeakRVRContinue,
        SpeakChannelRed2
#elif PROTOCOL==860
        SpeakSay = 1,
        SpeakWhisper,
        SpeakYell,
        SpeakPrivatePlayerToNpc,
        SpeakPrivateNpcToPlayer,
        SpeakPrivate,
        SpeakChannelYellow,
        SpeakChannelWhite,
        SpeakRVRChannel,
        SpeakRVRAnswer,
        SpeakRVRContinue,
        SpeakBroadcast,
        SpeakChannelRed,
        SpeakPrivateRed,
        SpeakChannelOrange,
        SpeakChannelRed2 = 17,
        SpeakMonsterSay = 19,
        SpeakMonsterYell
#endif
    };

    enum MessageTypes {
#if PROTOCOL==860
        MessageConsoleRed = 18,
        MessageEventOrange,
        MessageConsoleOrange,
        MessageWarning,
        MessageEventAdvance,
        MessageEventDefault,
        MessageStatusDefault,
        MessageInfoDescription ,
        MessageStatusSmall,
        MessageConsoleBlue
#elif PROTOCOL==862
        MessageEventOrange = 12,
        MessageConsoleOrange,
        MessageWarning,
        MessageEventAdvance,
        MessageEventDefault,
        MessageStatusDefault,
        MessageInfoDescription,
        MessageStatusSmall,
        MessageConsoleBlue,
        MessageConsoleRed
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

    inline std::string translateSpeakType(int type) {
        switch(type) {
            case Proto::SpeakSay:
                return "say";
            case Proto::SpeakWhisper:
                return "whisper";
            case Proto::SpeakYell:
                return "yell";
            case Proto::SpeakMonsterSay:
                return  "monsterSay";
            case Proto::SpeakMonsterYell:
                return "monsterYell";
            case Proto::SpeakPrivateNpcToPlayer:
                return "npcToPlayer";
            case Proto::SpeakChannelYellow:
                return "channelYellow";
            case Proto::SpeakChannelWhite:
                return "channelWhite";
            case Proto::SpeakChannelRed:
            case Proto::SpeakChannelRed2:
                return "channelRed";
            case Proto::SpeakChannelOrange:
                return "channelOrange";
            case Proto::SpeakPrivate:
                return "private";
            case Proto::SpeakPrivatePlayerToNpc:
                return "playerToNpc";
            case Proto::SpeakBroadcast:
                return "broadcast";
            case Proto::SpeakPrivateRed:
                return "privateRed";
            default:
                logError("unknown protocol speak type ", type);
                return "unknown";
        }
    }

    inline std::string translateTextMessageType(int type) {
        switch(type) {
            case Proto::MessageConsoleOrange:
                return "consoleOrange";
            case Proto::MessageEventOrange:
                return "eventOrange";
            case Proto::MessageWarning:
                return "warning";
            case Proto::MessageEventAdvance:
                return "eventAdvance";
            case Proto::MessageEventDefault:
                return "eventDefault";
            case Proto::MessageStatusDefault:
                return "statusDefault";
            case Proto::MessageInfoDescription:
                return "infoDescription";
            case Proto::MessageStatusSmall:
                return "statusSmall";
            case Proto::MessageConsoleBlue:
                return "consoleBlue";
            case Proto::MessageConsoleRed:
                return "consoleRed";
            default:
                logError("unknown protocol text message type ", type);
                return "unknown";
        }
    }
}

#endif
