/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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

#ifndef THINGTYPEMANAGER_H
#define THINGTYPEMANAGER_H

#include <framework/global.h>
#include <framework/core/declarations.h>

#include "thingtype.h"
#include "itemtype.h"

class ThingTypeManager
{
public:
    void init();
    void terminate();

    bool loadDat(std::string file);
    bool loadOtml(std::string file);
    void loadOtb(const std::string& file);
    void loadXml(const std::string& file);
    void parseItemType(uint16 id, TiXmlElement *elem);

    void saveDat(std::string fileName);

    void addItemType(const ItemTypePtr& itemType);
    const ItemTypePtr& findItemTypeByClientId(uint16 id);
    const ItemTypePtr& findItemTypeByName(std::string name);
    ItemTypeList findItemTypesByName(std::string name);
    ItemTypeList findItemTypesByString(std::string name);

    const ThingTypePtr& getNullThingType() { return m_nullThingType; }
    const ItemTypePtr& getNullItemType() { return m_nullItemType; }

    const ThingTypePtr& getThingType(uint16 id, ThingCategory category);
    const ItemTypePtr& getItemType(uint16 id);
    ThingType* rawGetThingType(uint16 id, ThingCategory category) { return m_thingTypes[category][id].get(); }
    ItemType* rawGetItemType(uint16 id) { return m_itemTypes[id].get(); }

    ThingTypeList findThingTypeByAttr(ThingAttr attr, ThingCategory category);
    ItemTypeList findItemTypeByCategory(ItemCategory category);

    const ThingTypeList& getThingTypes(ThingCategory category);
    const ItemTypeList& getItemTypes() { return m_itemTypes; }

    uint32 getDatSignature() { return m_datSignature; }
    uint32 getOtbMajorVersion() { return m_otbMajorVersion; }
    uint32 getOtbMinorVersion() { return m_otbMinorVersion; }
    uint16 getContentRevision() { return m_contentRevision; }

    bool isDatLoaded() { return m_datLoaded; }
    bool isXmlLoaded() { return m_xmlLoaded; }
    bool isOtbLoaded() { return m_otbLoaded; }

    bool isValidDatId(uint16 id, ThingCategory category) { return id >= 1 && id < m_thingTypes[category].size(); }
    bool isValidOtbId(uint16 id) { return id >= 1 && id < m_itemTypes.size(); }

private:
    ThingTypeList m_thingTypes[ThingLastCategory];
    ItemTypeList m_reverseItemTypes;
    ItemTypeList m_itemTypes;

    ThingTypePtr m_nullThingType;
    ItemTypePtr m_nullItemType;

    bool m_datLoaded;
    bool m_xmlLoaded;
    bool m_otbLoaded;

    uint32 m_otbMinorVersion;
    uint32 m_otbMajorVersion;
    uint32 m_datSignature;
    uint16 m_contentRevision;
};

extern ThingTypeManager g_things;

#endif
