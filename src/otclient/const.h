/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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

#ifndef OTCLIENT_CONST_H
#define OTCLIENT_CONST_H

#include <framework/util/color.h>

namespace Otc
{
    static const char* AppName = "OTClient";
    static const char* AppPathName = "otclient";
    static const char* AppVersion = "0.4.0";

    static const char* CipsoftPublicRSA = "1321277432058722840622950990822933849527763264961655079678763618"
                                          "4334395343554449668205332383339435179772895415509701210392836078"
                                          "6959821132214473291575712138800495033169914814069637740318278150"
                                          "2907336840325241747827401343576296990629870233111328210165697754"
                                          "88792221429527047321331896351555606801473202394175817";

    static const char* OtservPublicRSA = "1091201329673994292788609605089955415282375029027981291234687579"
                                         "3726629149257644633073969600111060390723088861007265581882535850"
                                         "3429057592827629436413108566029093628212635953836686562675849720"
                                         "6207862794310902180176810615217550567108238764764442605581471797"
                                         "07119674283982419152118103759076030616683978566631413";

    static const int ClientVersion = 862;
    static const int PicSignature = 0x4E119CBF;
    static const char* Host = "sv3.radbr.com";
    static const int HostPort = 7171;

    enum OsTypes {
        OsWindow = 1,
        OsLinux = 2,
        OsMac = 3,
        OsBrowser = 4
    };

    enum DatOpts {
        DatGround = 0,
        DatGroundClip,
        DatOnBottom,
        DatOnTop,
        DatContainer,
        DatStackable,
        DatForceUse,
        DatMultiUse,
        DatWritable,
        DatWritableOnce,
        DatFluidContainer,
        DatSplash,
        DatBlockWalk,
        DatNotMovable,
        DatBlockProjectile,
        DatBlockPathFind,
        DatPickupable,
        DatHangable,
        DatHookSouth,
        DatHookEast,
        DatRotable,
        DatLight,
        DatDontHide,
        DatTranslucent,
        DatDisplacement,
        DatElevation,
        DatLyingCorpse,
        DatAnimateAlways,
        DatMinimapColor,
        DatLensHelp,
        DatFullGround,
        DatIgnoreLook,
        DatCloth,
        DatAnimation, // lastest tibia
        DatLastOpt = 255
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

    enum SpriteMask {
        SpriteRedMask = 0,
        SpriteGreenMask,
        SpriteBlueMask,
        SpriteYellowMask,
        SpriteNoMask = 255
    };

    enum InventorySlots {
        InventorySlotHead = 1,
        InventorySlotNecklace,
        InventorySlotBackpack,
        InventorySlotArmor,
        InventorySlotRight,
        InventorySlotLeft,
        InventorySlotLegs,
        InventorySlotFeet,
        InventorySlotRing,
        InventorySlotAmmo
    };

    enum Statistic {
        Health,
        MaxHealth,
        FreeCapacity,
        Experience,
        Level,
        LevelPercent,
        Mana,
        MaxMana,
        MagicLevel,
        MagicLevelPercent,
        Soul,
        Stamina,
        LastStatistic
    };

    enum Skill {
        Fist = 0,
        Club,
        Sword,
        Axe,
        Distance,
        Shielding,
        Fishing,
        LastSkill
    };

    enum SkillType {
        SkillLevel,
        SkillPercent,
        LastSkillType
    };

    enum Direction {
        North = 0,
        East,
        South,
        West,
        NorthEast,
        SouthEast,
        SouthWest,
        NorthWest
    };

    enum SpeakClasses {
        SpeakSay                = 0x01, //normal talk
        SpeakWhisper            = 0x02, //whispering - #w text
        SpeakYell               = 0x03, //yelling - #y text
        SpeakPrivatePlayerToNpc = 0x04, //Player-to-NPC speaking(NPCs channel)
        SpeakPrivateNpcToPlayer = 0x05, //NPC-to-Player speaking
        SpeakPrivate            = 0x06, //Players speaking privately to players
        SpeakChannelYellow      = 0x07, //Yellow message in chat
        SpeakChannelWhite       = 0x08, //White message in chat
        SpeakBroadcast          = 0x09, //Broadcast a message - #b
        SpeakChannelRed         = 0x0A, //Talk red on chat - #c
        SpeakPrivateRed         = 0x0B, //Red private - @name@ text
        SpeakChannelOrange      = 0x0C, //Talk orange on text
        SpeakMonsterSay         = 0x0D, //Talk orange
        SpeakMonsterYell        = 0x0E  //Yell orange
    };

}

#endif
