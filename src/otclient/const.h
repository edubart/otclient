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

#ifndef OTCLIENT_CONST_H
#define OTCLIENT_CONST_H

namespace Otc
{
    enum {
        TILE_PIXELS = 32,
        MAX_ELEVATION = 24,

        SEA_FLOOR = 7,
        MAX_Z = 15,
        UNDERGROUND_FLOOR = SEA_FLOOR+1,
        VISIBLE_X_TILES = 15,
        VISIBLE_Y_TILES = 11,
        AWARE_UNDEGROUND_FLOOR_RANGE = 2,
        AWARE_X_TILES = VISIBLE_X_TILES + 3,
        AWARE_Y_TILES = VISIBLE_Y_TILES + 3,
        AWARE_X_LEFT_TILES = AWARE_X_TILES/2 - 1,
        AWARE_X_RIGHT_TILES = AWARE_X_TILES/2,
        AWARE_Y_TOP_TILES = AWARE_Y_TILES/2 - 1,
        AWARE_Y_BOTTOM_TILES = AWARE_Y_TILES/2,

        EFFECT_TICKS_PER_FRAME = 75,
        INVISIBLE_TICKS_PER_FRAME = 500,
        ITEM_TICKS_PER_FRAME = 500,
        ANIMATED_TEXT_DURATION = 1000,
        STATIC_DURATION_PER_CHARACTER = 60,
        MIN_STATIC_TEXT_DURATION = 3000,
        MAX_STATIC_TEXT_WIDTH = 200
    };

    enum DrawFlags {
        DrawGround = 1,
        DrawGroundBorders = 2,
        DrawOnBottom = 4,
        DrawOnTop = 8,
        DrawItems = 16,
        DrawCreatures = 32,
        DrawEffects = 64,
        DrawMissiles = 128,
        DrawCreaturesInformation = 256,
        DrawStaticTexts = 512,
        DrawAnimatedTexts = 1024,
        DrawAnimations = 2048,
        DrawWalls = DrawOnBottom | DrawOnTop,
        DrawEverything = DrawGround | DrawGroundBorders | DrawWalls | DrawItems |
                         DrawCreatures | DrawEffects | DrawMissiles |
                         DrawCreaturesInformation | DrawStaticTexts | DrawAnimatedTexts | DrawAnimations
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
        DatNotMoveable,
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

    enum InventorySlot {
        InventorySlotHead = 1,
        InventorySlotNecklace,
        InventorySlotBackpack,
        InventorySlotArmor,
        InventorySlotRight,
        InventorySlotLeft,
        InventorySlotLegs,
        InventorySlotFeet,
        InventorySlotRing,
        InventorySlotAmmo,
        InventorySlotPurse,
        LastInventorySlot
    };

    enum Statistic {
        Health = 0,
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

    enum Direction {
        North = 0,
        East,
        South,
        West,
        NorthEast,
        SouthEast,
        SouthWest,
        NorthWest,
        InvalidDirection
    };

    enum FluidsColor {
        FluidTransparent = 0,
        FluidBlue,
        FluidRed,
        FluidBrown,
        FluidGreen,
        FluidYellow,
        FluidWhite,
        FluidPurple
    };

    enum FluidsType {
        FluidNone = 0,
        FluidWater,
        FluidMana,
        FluidBeer,
        FluidOil,
        FluidBlood,
        FluidSlime,
        FluidMud,
        FluidLemonade,
        FluidMilk,
        FluidWine,
        FluidHealth,
        FluidUrine,
        FluidRum,
        FluidFruidJuice,
        FluidCoconutMilk,
        FluidTea,
        FluidMead
    };

    enum FightModes {
        FightOffensive = 1,
        FightBalanced = 2,
        FightDefensive = 3
    };

    enum ChaseModes {
        DontChase = 0,
        ChaseOpponent = 1
    };

    enum PlayerSkulls {
        SkullNone = 0,
        SkullYellow,
        SkullGreen,
        SkullWhite,
        SkullRed,
        SkullBlack,
        SkullOrange
    };

    enum PlayerShields {
        ShieldNone = 0,
        ShieldWhiteYellow, // 1 party leader
        ShieldWhiteBlue, // 2 party member
        ShieldBlue, // 3 party member sexp off
        ShieldYellow, // 4 party leader sexp off
        ShieldBlueSharedExp, // 5 party member sexp on
        ShieldYellowSharedExp, // 6 // party leader sexp on
        ShieldBlueNoSharedExpBlink, // 7 party member sexp inactive guilty
        ShieldYellowNoSharedExpBlink, // 8 // party leader sexp inactive guilty
        ShieldBlueNoSharedExp, // 9 party member sexp inactive innocent
        ShieldYellowNoSharedExp // 10 party leader sexp inactive innocent
    };

    enum PlayerEmblems {
        EmblemNone = 0,
        EmblemGreen,
        EmblemRed,
        EmblemBlue
    };

    enum PlayerStates {
        IconNone = 0,
        IconPoison = 1,
        IconBurn = 2,
        IconEnergy = 4,
        IconDrunk = 8,
        IconManaShield = 16,
        IconParalyze = 32,
        IconHaste = 64,
        IconSwords = 128,
        IconDrowning = 256,
        IconFreezing = 512,
        IconDazzled = 1024,
        IconCursed = 2048,
        IconPartyBuff = 4096,
        IconPzBlock = 8192,
        IconPz = 16384
    };

    enum SpeakType {
        SpeakSay = 1,
        SpeakWhisper,
        SpeakYell,
        SpeakBroadcast,
        SpeakPrivate,
        SpeakPrivateRed,
        SpeakPrivatePlayerToNpc,
        SpeakPrivateNpcToPlayer,
        SpeakChannelYellow,
        SpeakChannelWhite,
        SpeakChannelRed,
        SpeakChannelOrange,
        SpeakMonsterSay,
        SpeakMonsterYell
    };

    enum GameFeature {
        // 1-50 defined in c++
        GameProtocolChecksum = 1,
        GameAccountNames = 2,
        GameChallangeOnLogin = 3,
        GamePenalityOnDeath = 5,
        GameNameOnNpcTrade = 6,
        GameDoubleFreeCapacity = 7,
        GameDoubleExperience = 8,
        GameTotalCapacity = 9,
        GameSkillsBase = 10,
        GamePlayerRegenerationTime = 11,
        GameChannelPlayerList = 13,
        GamePlayerMounts = 14,
        GameEnvironmentEffect = 15,
        GameCreatureEmblems = 17,
        GameItemAnimationPhase = 19,
        GameMagicEffectU16 = 22,
        GamePlayerMarket = 23,
        GameSpritesU32 = 24,
        GameChargeableItems = 25,
        GameOfflineTrainingTime = 26,
        GamePurseSlot = 27,
        // 23-50 unused yet
        // 51-100 reserved to be defined in lua
        LastGameFeature = 101
    };

    enum PathFindResult {
        PATHFIND_OK = 0,
        PATHFIND_SAME_POSITION,
        PATHFIND_IMPOSSIBLE,
        PATHFIND_TOO_FAR,
        PATHFIND_NO_WAY
    };
}

#endif
