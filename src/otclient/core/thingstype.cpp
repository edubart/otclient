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

#include "thingstype.h"
#include "spritemanager.h"
#include "thing.h"
#include <framework/core/resourcemanager.h>

ThingsType g_thingsType;

bool ThingsType::load(const std::string& file)
{
    try {
        std::stringstream fin;
        g_resources.loadFile(file, fin);

        m_signature = Fw::getU32(fin);
        int numItems = Fw::getU16(fin);
        int numCreatures = Fw::getU16(fin);
        int numEffects = Fw::getU16(fin);
        int numShots = Fw::getU16(fin);

        m_itemsType.resize(numItems);
        for(int id = 100; id < numItems; ++id)
            parseThingType(fin, m_itemsType[id - 100]);

        m_creaturesType.resize(numItems);
        for(int id = 0; id < numCreatures; ++id)
            parseThingType(fin, m_creaturesType[id]);

        m_effectsType.resize(numItems);
        for(int id = 0; id < numEffects; ++id)
            parseThingType(fin, m_effectsType[id]);

        m_shotsType.resize(numItems);
        for(int id = 0; id < numShots; ++id)
            parseThingType(fin, m_shotsType[id]);

        return true;
    } catch(std::exception& e) {
        logError("failed to load dat from '", file, "': ", e.what());
        return false;
    }
}

void ThingsType::unload()
{
    m_itemsType.clear();
    m_creaturesType.clear();
    m_effectsType.clear();
    m_shotsType.clear();
}

void ThingsType::parseThingType(std::stringstream& fin, ThingType& thingType)
{
    assert(fin.good());

    bool done = false;
    while(!done) {
        uint8 opt = Fw::getU8(fin);

        switch(opt) {
            case Otc::DatGround: // Grounds, must be drawn first
                thingType.groundSpeed = Fw::getU16(fin);
                thingType.isGround = true;
                break;
            case Otc::DatGroundClip: // Objects that clips (has transparent pixels) and must be drawn just after ground (e.g: ground borders)
                thingType.isGroundClip = true;
                break;
            case Otc::DatOnBottom: // Bottom items, must be drawn above general items and below creatures (e.g: stairs)
                thingType.isOnBottom = true;
                break;
            case Otc::DatOnTop: // Top items, must be drawn above creatures (e.g: doors)
                thingType.isOnTop = true;
                break;
            case Otc::DatContainer: // Containers
                thingType.isContainer = true;
                break;
            case Otc::DatStackable: // Stackable
                thingType.isStackable = true;
                break;
            case Otc::DatForceUse: // Items that are automatically used when step over?
                thingType.isForceUse = true;
                break;
            case Otc::DatMultiUse: // Usable items
                thingType.isMultiUse = true;
                break;
            case Otc::DatWritable: // Writable
                thingType.isWritable = true;
                thingType.maxTextLength = Fw::getU16(fin);
                break;
            case Otc::DatWritableOnce: // Writable once. objects that can't be edited by players
                thingType.isWritableOnce = true;
                thingType.maxTextLength = Fw::getU16(fin);
                break;
            case Otc::DatFluidContainer: // Fluid containers
                thingType.isFluidContainer = true;
                break;
            case Otc::DatSplash: // Splashes
                thingType.isStackable = true;
                break;
            case Otc::DatBlockWalk: // Blocks solid objects (creatures, walls etc)
                thingType.isNotWalkable = true;
                break;
            case Otc::DatNotMovable: // Not movable
                thingType.isNotMoveable = true;
                break;
            case Otc::DatBlockProjectile: // Blocks missiles (walls, magic wall etc)
                thingType.isNotProjectable = true;
                break;
            case Otc::DatBlockPathFind: // Blocks pathfind algorithms (monsters)
                thingType.isNotPathable = true;
                break;
            case Otc::DatPickupable: // Pickupable
                thingType.isPickupable = true;
                break;
            case Otc::DatHangable: // Hangable objects (wallpaper etc)
                thingType.isHangable = true;
                break;
            case Otc::DatHookSouth: // Horizontal walls
                thingType.isHookSouth = true;
                break;
            case Otc::DatHookEast: // Vertical walls
                thingType.isHookEast = true;
                break;
            case Otc::DatRotable: // Rotable
                thingType.isRotable = true;
                break;
            case Otc::DatLight: // Light info
                thingType.hasLight = true;
                thingType.lightLevel = Fw::getU16(fin);
                thingType.lightColor = Fw::getU16(fin);
                break;
            case Otc::DatDontHide: // A few monuments that are not supposed to be hidden by floors
                break;
            case Otc::DatTranslucent: // Grounds that are translucent
                thingType.isTranslucent = true;
                break;
            case Otc::DatDisplacment: // Must shift draw
                thingType.xDisplacment = Fw::getU16(fin);
                thingType.yDisplacment = Fw::getU16(fin);
                break;
            case Otc::DatElevation: // Must elevate draw
                thingType.elevation = Fw::getU16(fin);
                break;
            case Otc::DatLyingCorpse: // Some corpses
                thingType.isLyingCorpse = true;
                break;
            case Otc::DatAnimateAlways: // Unknown, check if firesword is a kind of AnimateAlways.
                thingType.isAnimatedAlways = true;
                break;
            case Otc::DatMinimapColor: // Minimap color
                thingType.hasMiniMapColor = true;
                thingType.miniMapColor = Fw::getU16(fin);
                break;
            case Otc::DatLensHelp: // Used for giving players tips?
                thingType.isLensHelp = true;
                thingType.lensHelp = Fw::getU16(fin);
                break;
            case Otc::DatFullGround: // Grounds that has no transparent pixels
                thingType.isFullGround = true;
                break;
            case Otc::DatIgnoreLook: // Ignore look, then looks at the item on the bottom of it
                thingType.isIgnoreLook = true;
                break;
            case Otc::DatCloth: // Clothes
                thingType.isCloth = true;
                thingType.clothSlot = Fw::getU16(fin);
                break;
            case Otc::DatLastOpt:
                done = true;
                break;
            default:
                throw std::runtime_error(Fw::mkstr("unknown .dat byte code: ", (int)opt));
        }
    }

    thingType.width = Fw::getU8(fin);
    thingType.height = Fw::getU8(fin);

    if(thingType.width > 1 || thingType.height > 1)
        thingType.exactSize = Fw::getU8(fin);
    else
        thingType.exactSize = 32;

    thingType.layers = Fw::getU8(fin);
    thingType.xPattern = Fw::getU8(fin);
    thingType.yPattern = Fw::getU8(fin);
    thingType.zPattern = Fw::getU8(fin);
    thingType.animationPhases = Fw::getU8(fin);
    if(thingType.animationPhases > 1)
        thingType.isAnimation = true;

    int totalSprites = thingType.width
                       * thingType.height
                       * thingType.layers
                       * thingType.xPattern
                       * thingType.yPattern
                       * thingType.zPattern
                       * thingType.animationPhases;

    thingType.sprites.resize(totalSprites);
    for(uint16 i = 0; i < totalSprites; i++)
        thingType.sprites[i] = Fw::getU16(fin);
}
