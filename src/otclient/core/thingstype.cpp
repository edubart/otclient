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

#include "thingstype.h"
#include "spritemanager.h"
#include "thing.h"
#include <framework/core/resourcemanager.h>
#include <framework/core/filestream.h>

ThingsType g_thingsType;
ThingType ThingsType::m_emptyThingType;

bool ThingsType::load(const std::string& file)
{
    FileStreamPtr fin = g_resources.openFile(file);
    if(!fin) {
        logError("unable to open dat file '", file, "'");
        return false;
    }

    m_signature = fin->getU32();

    int numThings[LastCategory];
    for(int i = 0; i < LastCategory; ++i)
        numThings[i] = fin->getU16();

    numThings[Item] -= 99;

    for(int i = 0; i < LastCategory; ++i) {
        m_things[i].resize(numThings[i]);
        for(int id = 0; id < numThings[i]; ++id)
            parseThingType(fin, m_things[i][id]);
    }

    m_loaded = true;
    return true;
}

void ThingsType::unload()
{
    for(int i = 0; i < LastCategory; ++i)
        m_things[i].clear();
}

void ThingsType::parseThingType(const FileStreamPtr& fin, ThingType& thingType)
{
    while(true) {
        int property = fin->getU8();
        if(property == ThingType::LastPropertyValue) {
            break;
        }

        thingType.m_properties[property] = true;

        if(property == ThingType::IsGround)
            thingType.m_parameters[ThingType::GroundSpeed] = fin->getU16();
        else if(property == ThingType::IsWritable || property == ThingType::IsWritableOnce)
            thingType.m_parameters[ThingType::MaxTextLenght] = fin->getU16();
        else if(property == ThingType::HasLight) {
            thingType.m_parameters[ThingType::LightLevel] = fin->getU16();
            thingType.m_parameters[ThingType::LightColor] = fin->getU16();
        }
        else if(property == ThingType::HasDisplacement) {
            thingType.m_parameters[ThingType::DisplacementX] = fin->getU16();
            thingType.m_parameters[ThingType::DisplacementY] = fin->getU16();
        }
        else if(property == ThingType::HasElevation)
            thingType.m_parameters[ThingType::Elevation] = fin->getU16();
        else if(property == ThingType::MiniMap)
            thingType.m_parameters[ThingType::MiniMapColor] = fin->getU16();
        else if(property == ThingType::LensHelp)
            thingType.m_parameters[ThingType::LensHelpParameter] = fin->getU16();
        else if(property == ThingType::Cloth)
            thingType.m_parameters[ThingType::ClothSlot] = fin->getU16();
#if PROTOCOL<=810
        else if(property == ThingType::IsRune)
            thingType.m_properties[ThingType::IsStackable] = true;
#endif
#if PROTOCOL>=944
        else if(property == ThingType::Market) {
            fin->getU16(); // category
            fin->getU16(); // trade as
            fin->getU16(); // show as
            fin->getString(); // name
            fin->getU16(); // restrict profession
            fin->getU16(); // level
        }
#endif
    }

    int totalSprites = 1;
    for(int i = 0; i < ThingType::LastDimension; ++i) {
        if(i == ThingType::ExactSize && thingType.m_dimensions[ThingType::Width] <= 1 && thingType.m_dimensions[ThingType::Height] <= 1) {
            thingType.m_dimensions[i] = 32;
            continue;
        }

        thingType.m_dimensions[i] = fin->getU8();

        if(i != ThingType::ExactSize)
            totalSprites *= thingType.m_dimensions[i];
    }

    thingType.m_spritesIndex.resize(totalSprites);
    thingType.m_sprites.resize(totalSprites);
    for(int i = 0; i < totalSprites; i++)
        thingType.m_spritesIndex[i] = fin->getU16();
}

ThingType *ThingsType::getThingType(uint16 id, Categories category)
{
    if(id == 0)
        return &m_emptyThingType;

    if(category == Item)
        id -= 100;
    else if(category == Creature || category == Effect || category == Missile)
        id -= 1;

    if(id >= m_things[category].size())
        return &m_emptyThingType;
    return &m_things[category][id];
}
