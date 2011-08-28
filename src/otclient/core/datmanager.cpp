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

        if(opt == 0xFF)
            break;

        parseThingAttributesOpt(fin, thingAttributes, opt);
    }

    thingAttributes.width = Fw::getU8(fin);
    thingAttributes.height = Fw::getU8(fin);
    if(thingAttributes.width > 1 || thingAttributes.height > 1)
        Fw::getU8(fin); // ??

    thingAttributes.blendframes = Fw::getU8(fin);
    thingAttributes.xdiv = Fw::getU8(fin);
    thingAttributes.ydiv = Fw::getU8(fin);
    thingAttributes.zdiv = Fw::getU8(fin);
    thingAttributes.animcount = Fw::getU8(fin);

    int totalSprites = thingAttributes.width
                       * thingAttributes.height
                       * thingAttributes.blendframes
                       * thingAttributes.xdiv
                       * thingAttributes.ydiv
                       * thingAttributes.zdiv
                       * thingAttributes.animcount;

    thingAttributes.sprites.resize(totalSprites);
    for(uint16 i = 0; i < totalSprites; i++)
        thingAttributes.sprites[i] = Fw::getU16(fin);
}

void DatManager::parseThingAttributesOpt(std::stringstream& fin, ThingAttributes& thingAttributes, uint8 opt)
{
    switch(opt) {
        case 0x00: // Ground tile
            thingAttributes.speed = Fw::getU16(fin);
            thingAttributes.group = Otc::ThingGroundGroup;
            break;
        case 0x01: // All OnTop
            thingAttributes.alwaysOnTop = true;
            thingAttributes.alwaysOnTopOrder = 1;
            break;
        case 0x02: // Can walk trough (open doors, arces, bug pen fence)
            thingAttributes.alwaysOnTop = true;
            thingAttributes.alwaysOnTopOrder = 2;
            break;
        case 0x03: // Can walk trough (arces)
            thingAttributes.alwaysOnTop = true;
            thingAttributes.alwaysOnTopOrder = 3;
            break;
        case 0x04: // Container
            thingAttributes.group = Otc::ThingContainerGroup;
            break;
        case 0x05: // Stackable
            thingAttributes.stackable = true;
            break;
        case 0x06: // Unknown
            break;
        case 0x07: // Useable
            thingAttributes.useable = true;
            break;
        case 0x08: // Writtable
            thingAttributes.group = Otc::ThingWriteableGroup;
            thingAttributes.readable = true;
            thingAttributes.subParam08 = Fw::getU16(fin);
            break;
        case 0x09: // Writtable once
            // Writtable objects that can't be edited by players
            thingAttributes.readable = true;
            thingAttributes.subParam08 = Fw::getU16(fin);
            break;
        case 0x0A: // Fluid containers
            thingAttributes.group = Otc::ThingFluidGroup;
            Fw::getU8(fin);
            break;
        case 0x0B: // Splashes
            thingAttributes.group = Otc::ThingSplashGroup;
            break;
        case 0x0C: // Blocks solid objects (creatures, walls etc)
            thingAttributes.blockSolid = true;
            break;
        case 0x0D: // Not moveable
            thingAttributes.moveable = false;
            break;
        case 0x0E: // Blocks missiles (walls, magic wall etc)
            thingAttributes.blockProjectile = true;
            break;
        case 0x0F: // Blocks pathfind algorithms (monsters)
            thingAttributes.blockPathFind = true;
            break;
        case 0x10: // Blocks monster movement (flowers, parcels etc)
            thingAttributes.pickupable = true;
            break;
        case 0x11: // Hangable objects (wallpaper etc)
            thingAttributes.isHangable = true;
            break;
        case 0x12: // Horizontal wall
            thingAttributes.isHorizontal = true;
            break;
        case 0x13: // Vertical wall
            thingAttributes.isVertical = true;
            break;
        case 0x14: // Rotable
            thingAttributes.rotable = true;
            break;
        case 0x15: // Light info
            thingAttributes.lightLevel = Fw::getU16(fin);
            thingAttributes.lightColor = Fw::getU16(fin);
            break;
        case 0x16:
            break;
        case 0x17: // Changes floor
            break;
        case 0x18: // Thing must be drawed with offset
            thingAttributes.hasHeight = true;
            thingAttributes.drawOffset = Fw::getU8(fin);
            Fw::getU8(fin);
            Fw::getU16(fin);
            break;
        case 0x19: // pixels characters height
            thingAttributes.drawNextOffset = Fw::getU8(fin);
            Fw::getU8(fin);
            break;
        case 0x1A:
            //thingAttributes.hasHeight = true;
            break;
        case 0x1B:
            break;
        case 0x1C: // Minimap color
            thingAttributes.miniMapColor = Fw::getU16(fin);
            thingAttributes.hasMiniMapColor = true;
            break;
        case 0x1D: // Unknown
            if(Fw::getU16(fin) == 1112)
                thingAttributes.readable = true;
            break;
        case 0x1E:
            break;
        case 0x1F:
            thingAttributes.lookThrough = true;
            break;
        case 0x20:
            break;
        default:
            throw std::runtime_error(Fw::mkstr("unknown .dat byte code: 0x", std::hex, (int)opt));
    }
}
