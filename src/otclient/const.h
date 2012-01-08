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

#include <framework/math/color.h>

namespace Otc
{
    static const char* AppName = "OTClient";
    static const char* AppCompactName = "otclient";
    static const char* AppVersion = "0.4.0";

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
        NorthWest,
        InvalidDirection
    };

    enum FluidsColor {
        FluidTransparent,
        FluidBlue,
        FluidRed,
        FluidBrown,
        FluidGreen,
        FluidYellow,
        FluidWhite,
        FluidPurple
    };

    enum FluidsType {
        FluidWater = 1,
        FluidMana = 2,
        FluidBeer = 3,
        FluidBlood = 5,
        FluidSlime = 6,
        FluidLemonade = 8,
        FluidMilk = 9,
        FluidWine = 10,
        FluidHealth = 11,
        FluidCoconutMilk = 15
    };

    enum FightModes {
        FightOffensive = 1,
        FightBalanced = 2,
        FightDefensive = 3
    };

    enum ChaseModes {
        StandWhileFighting = 0,
        ChaseOpponent = 1
    };

    enum PlayerSkulls {
        SkullNone = 0,
        SkullYellow = 1,
        SkullGreen = 2,
        SkullWhite = 3,
        SkullRed = 4,
        SkullBlack = 5,
        SkullOrange = 6
    };

    enum PlayerShields {
        ShieldNone = 0,
        ShieldWhiteYellow = 1,
        ShieldWhiteBlue = 2,
        ShieldBlue = 3,
        ShieldYellow = 4,
        ShieldBlueSharedExp = 5,
        ShieldYellowSharedExp = 6,
        ShieldBlueNoSharedExpBlink = 7,
        ShieldYellowNoSharedExpBlink = 8,
        ShieldBlueNoSharedExp = 9,
        ShieldYellowNoSharedExp = 10
    };

    enum PlayerEmblems {
        EmblemNone = 0,
        EmblemGreen = 1,
        EmblemRed = 2,
        EmblemBlue = 3
    };
}

#endif
