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

#include "datmanager.h"
#include "spritemanager.h"
#include "thing.h"
#include <framework/core/resourcemanager.h>

DatManager g_dat;

bool DatManager::load(const std::string& file)
{
    try {
        std::stringstream fin;
        g_resources.loadFile(file, fin);

        m_signature = Fw::getU32(fin);
        int numItems = Fw::getU16(fin);
        int numCreatures = Fw::getU16(fin);
        int numEffects = Fw::getU16(fin);
        int numShots = Fw::getU16(fin);

        m_itemsAttributes.resize(numItems);
        for(int id = 100; id < numItems; ++id)
            parseThingAttributes(fin, m_itemsAttributes[id - 100]);

        m_creaturesAttributes.resize(numItems);
        for(int id = 0; id < numCreatures; ++id)
            parseThingAttributes(fin, m_creaturesAttributes[id]);

        m_effectsAttributes.resize(numItems);
        for(int id = 0; id < numEffects; ++id)
            parseThingAttributes(fin, m_effectsAttributes[id]);

        m_shotsAttributes.resize(numItems);
        for(int id = 0; id < numShots; ++id)
            parseThingAttributes(fin, m_shotsAttributes[id]);

        return true;
    } catch(std::exception& e) {
        logError("failed to load dat from '", file, "': ", e.what());
        return false;
    }
}

void DatManager::unload()
{
    m_itemsAttributes.clear();
    m_creaturesAttributes.clear();
    m_effectsAttributes.clear();
    m_shotsAttributes.clear();
}

void DatManager::parseThingAttributes(std::stringstream& fin, ThingAttributes& thingAttributes)
{
    assert(fin.good());

    while(true) {
        uint8 opt;
        fin.read((char*)&opt, 1);

        if(opt == Otc::LastDatFlag)
            break;

        parseThingAttributesOpt(fin, thingAttributes, opt);
    }

    thingAttributes.width = Fw::getU8(fin);
    thingAttributes.height = Fw::getU8(fin);

    if(thingAttributes.width > 1 || thingAttributes.height > 1)
        thingAttributes.exactSize = Fw::getU8(fin);
    else
        thingAttributes.exactSize = 32;

    thingAttributes.layers = Fw::getU8(fin);
    thingAttributes.xPattern = Fw::getU8(fin);
    thingAttributes.yPattern = Fw::getU8(fin);
    thingAttributes.zPattern = Fw::getU8(fin);
    thingAttributes.animationPhases = Fw::getU8(fin);

    int totalSprites = thingAttributes.width
                       * thingAttributes.height
                       * thingAttributes.layers
                       * thingAttributes.xPattern
                       * thingAttributes.yPattern
                       * thingAttributes.zPattern
                       * thingAttributes.animationPhases;

    thingAttributes.sprites.resize(totalSprites);
    for(uint16 i = 0; i < totalSprites; i++)
        thingAttributes.sprites[i] = Fw::getU16(fin);
}

void DatManager::parseThingAttributesOpt(std::stringstream& fin, ThingAttributes& thingAttributes, uint8 opt)
{
    switch(opt) {
        case Otc::DatGround: // Grounds, must be drawn first
            thingAttributes.groundSpeed = Fw::getU16(fin);
            thingAttributes.isGround = true;
            break;
        case Otc::DatGroundClip: // Objects that clips (has transparent pixels) and must be drawn just after ground (e.g: ground borders)
            thingAttributes.isGroundClip = true;
            break;
        case Otc::DatOnBottom: // Bottom items, must be drawn above general items and below creatures (e.g: stairs)
            thingAttributes.isOnBottom = true;
            break;
        case Otc::DatOnTop: // Top items, must be drawn above creatures (e.g: doors)
            thingAttributes.isOnTop = true;
            break;
        case Otc::DatContainer: // Containers
            thingAttributes.isContainer = true;
            break;
        case Otc::DatStackable: // Stackable
            thingAttributes.isStackable = true;
            break;
        case Otc::DatForceUse: // Items that are automatically used when step over?
            thingAttributes.isForceUse = true;
            break;
        case Otc::DatMultiUse: // Usable items
            thingAttributes.isMultiUse = true;
            break;
        case Otc::DatWritable: // Writable
            thingAttributes.isWritable = true;
            thingAttributes.maxTextLength = Fw::getU16(fin);
            break;
        case Otc::DatWritableOnce: // Writable once. objects that can't be edited by players
            thingAttributes.isWritableOnce = true;
            thingAttributes.maxTextLength = Fw::getU16(fin);
            break;
        case Otc::DatFluidContainer: // Fluid containers
            thingAttributes.fluidParam = Fw::getU8(fin);
            break;
        case Otc::DatSplash: // Splashes
            thingAttributes.isStackable = true;
            break;
        case Otc::DatBlockWalk: // Blocks solid objects (creatures, walls etc)
            thingAttributes.isNotWalkable = true;
            break;
        case Otc::DatNotMovable: // Not movable
            thingAttributes.isNotMoveable = true;
            break;
        case Otc::DatBlockProjectile: // Blocks missiles (walls, magic wall etc)
            thingAttributes.isNotProjectable = true;
            break;
        case Otc::DatBlockPathFind: // Blocks pathfind algorithms (monsters)
            thingAttributes.isNotPathable = true;
            break;
        case Otc::DatPickupable: // Pickupable
            thingAttributes.isPickupable = true;
            break;
        case Otc::DatHangable: // Hangable objects (wallpaper etc)
            thingAttributes.isHangable = true;
            break;
        case Otc::DatHookSouth: // Horizontal walls
            thingAttributes.isHookSouth = true;
            break;
        case Otc::DatHookEast: // Vertical walls
            thingAttributes.isHookEast = true;
            break;
        case Otc::DatRotable: // Rotable
            thingAttributes.isRotable = true;
            break;
        case Otc::DatLight: // Light info
            thingAttributes.hasLight = true;
            thingAttributes.lightLevel = Fw::getU16(fin);
            thingAttributes.lightColor = Fw::getU16(fin);
            break;
        case Otc::DatDontHide: // A few monuments that are not supposed to be hidden by floors
            break;
        case Otc::DatTranslucent: // Grounds that are translucent
            thingAttributes.isTranslucent = true;
            break;
        case Otc::DatDisplacment: // Must shift draw
            thingAttributes.xDisplacment = Fw::getU16(fin);
            thingAttributes.yDisplacment = Fw::getU16(fin);
            break;
        case Otc::DatElevation: // Must elevate draw
            thingAttributes.elevation = Fw::getU16(fin);
            break;
        case Otc::DatLyingCorpse: // Some corpses
            thingAttributes.isLyingCorpse = true;
            break;
        case Otc::DatAnimateAlways: // Unknown, check if firesword is a kind of AnimateAlways.
            thingAttributes.isAnimatedAlways = true;
            break;
        case Otc::DatMinimapColor: // Minimap color
            thingAttributes.hasMiniMapColor = true;
            thingAttributes.miniMapColor = Fw::getU16(fin);
            break;
        case Otc::DatLensHelp: // Used for giving players tips?
            thingAttributes.isLensHelp = true;
            thingAttributes.lensHelpParam = Fw::getU16(fin);
            break;
        case Otc::DatFullGround: // Grounds that has no transparent pixels
            thingAttributes.isFullGround = true;
            break;
        case Otc::DatIgnoreLook: // Ignore look, then looks at the item on the bottom of it
            thingAttributes.isIgnoreLook = true;
            break;
        case Otc::DatClothe: // Clothes
            break;
        case Otc::DatAnimation: // Not used in 8.62
            break;
        default:
            throw std::runtime_error(Fw::mkstr("unknown .dat byte code: ", (int)opt));
    }
}
