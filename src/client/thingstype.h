/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
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
#include <framework/core/declarations.h>
#include "thingtype.h"

//@bindsingleton g_thingsType
class ThingsType
{
public:

    enum Categories {
        Item = 0,
        Creature,
        Effect,
        Missile,
        LastCategory
    };

    bool load(const std::string& file);
    void unload();

    bool parseThingType(const FileStreamPtr& fin, ThingType& thingType);

    ThingType *getEmptyThingType() { return &m_emptyThingType; }
    ThingType *getThingType(uint16 id, Categories category);

    uint32 getSignature() { return m_signature; }
    bool isLoaded() { return m_loaded; }

    uint16 getFirstItemId() { return 100; }
    uint16 getMaxItemid() { return m_things[Item].size() + 99; }
    bool isValidItemId(int id) { return id >= getFirstItemId() && id <= getMaxItemid(); }

private:
    uint32 m_signature;
    stdext::boolean<false> m_loaded;
    ThingTypeList m_things[LastCategory];
    static ThingType m_emptyThingType;
};

extern ThingsType g_thingsType;

#endif
