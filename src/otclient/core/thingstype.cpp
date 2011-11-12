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

        int numThings[LastCategory];
        for(int i = 0; i < LastCategory; ++i)
            numThings[i] = Fw::getU16(fin);

        numThings[Item] -= 100;

        for(int i = 0; i < LastCategory; ++i) {
            m_things[i].resize(numThings[i]);
            for(int id = 0; id < numThings[i]; ++id)
                parseThingType(fin, m_things[i][id]);
        }

        return true;
    } catch(std::exception& e) {
        logError("failed to load dat from '", file, "': ", e.what());
        return false;
    }
}

void ThingsType::unload()
{
    for(int i = 0; i < LastCategory; ++i)
        m_things[i].clear();
}

void ThingsType::parseThingType(std::stringstream& fin, ThingType& thingType)
{
    assert(fin.good());

    while(true) {
        int property = Fw::getU8(fin);
        if(property == ThingType::LastPropertyValue)
            break;

        thingType.properties[property] = true;

        if(property == ThingType::IsGround)
            thingType.parameters[ThingType::GroundSpeed] = Fw::getU16(fin);
        else if(property == ThingType::IsWritable || property == ThingType::IsWritableOnce)
            thingType.parameters[ThingType::MaxTextLenght] = Fw::getU16(fin);
        else if(property == ThingType::HasLight) {
            thingType.parameters[ThingType::LightLevel] = Fw::getU16(fin);
            thingType.parameters[ThingType::LightColor] = Fw::getU16(fin);
        }
        else if(property == ThingType::HasDisplacement) {
            thingType.parameters[ThingType::DisplacementX] = Fw::getU16(fin);
            thingType.parameters[ThingType::DisplacementY] = Fw::getU16(fin);
        }
        else if(property == ThingType::HasElevation)
            thingType.parameters[ThingType::Elevation] = Fw::getU16(fin);
        else if(property == ThingType::MiniMap)
            thingType.parameters[ThingType::MiniMapColor] = Fw::getU16(fin);
        else if(property == ThingType::LensHelp)
            thingType.parameters[ThingType::LensHelpParameter] = Fw::getU16(fin);
        else if(property == ThingType::Cloth)
            thingType.parameters[ThingType::ClothSlot] = Fw::getU16(fin);
    }

    int totalSprites = 1;
    for(int i = 0; i < ThingType::LastDimension; ++i) {
        if(i == ThingType::ExactSize && thingType.dimensions[ThingType::Width] <= 1 && thingType.dimensions[ThingType::Height] <= 1) {
            thingType.dimensions[i] = 32;
            continue;
        }

        thingType.dimensions[i] = Fw::getU8(fin);

        if(i != ThingType::ExactSize)
            totalSprites *= thingType.dimensions[i];
    }

    thingType.sprites.resize(totalSprites);
    for(int i = 0; i < totalSprites; i++)
        thingType.sprites[i] = Fw::getU16(fin);
}

ThingType& ThingsType::getThingType(uint16 id, Categories category)
{
    if(category == Item)
        id -= 100;

    assert(id < m_things[category].size());

    return m_things[category][id];
}
