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

#ifndef DATMANAGER_H
#define DATMANAGER_H

#include <framework/global.h>
#include "thingtype.h"

class ThingsType
{
public:
    bool load(const std::string& file);
    void unload();

    void parseThingType(std::stringstream& fin, ThingType& thingType);

    ThingType& getItemType(uint16 id) { return m_itemsType[id - 100]; }
    ThingType& getCreatureType(uint16 id)  { return m_creaturesType[id]; }
    ThingType& getEffectType(uint16 id) { return m_effectsType[id]; }
    ThingType& getShotType(uint16 id) { return m_shotsType[id]; }

    uint32 getSignature() { return m_signature; }

private:
    uint32 m_signature;

    ThingTypeList m_itemsType;
    ThingTypeList m_creaturesType;
    ThingTypeList m_effectsType;
    ThingTypeList m_shotsType;
};

extern ThingsType g_thingsType;

#endif
