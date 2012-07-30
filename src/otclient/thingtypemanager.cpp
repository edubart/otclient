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

#include "thingtypemanager.h"
#include "spritemanager.h"
#include "thing.h"
#include "thingtype.h"
#include "itemtype.h"

#include <framework/core/resourcemanager.h>
#include <framework/core/filestream.h>
#include <framework/core/binarytree.h>
#include <framework/xml/tinyxml.h>

ThingTypeManager g_things;

void ThingTypeManager::init()
{
    m_nullThingType = ThingTypePtr(new ThingType);
    m_nullItemType = ItemTypePtr(new ItemType);
    m_datSignature = 0;
    m_otbMinorVersion = 0;
    m_otbMajorVersion = 0;
    m_datLoaded = false;
    m_xmlLoaded = false;
    m_otbLoaded = false;
    for(int i = 0; i < ThingLastCategory; ++i)
        m_thingTypes[i].resize(1, m_nullThingType);
    m_itemTypes.resize(1, m_nullItemType);
}

void ThingTypeManager::terminate()
{
    for(int i = 0; i < ThingLastCategory; ++i)
        m_thingTypes[i].clear();
    m_itemTypes.clear();
    m_nullThingType = nullptr;
    m_nullItemType = nullptr;
}

bool ThingTypeManager::loadDat(const std::string& file)
{
    m_datLoaded = false;
    m_datSignature = 0;
    try {
        FileStreamPtr fin = g_resources.openFile(file);

        m_datSignature = fin->getU32();

        int numThings[ThingLastCategory];
        for(int category = 0; category < ThingLastCategory; ++category) {
            int count = fin->getU16() + 1;
            m_thingTypes[category].clear();
            m_thingTypes[category].resize(count, m_nullThingType);
        }

        for(int category = 0; category < ThingLastCategory; ++category) {
            uint16 firstId = 1;
            if(category == ThingCategoryItem)
                firstId = 100;
            for(uint16 id = firstId; id < m_thingTypes[category].size(); ++id) {
                ThingTypePtr type(new ThingType);
                type->unserialize(id, (ThingCategory)category, fin);
                m_thingTypes[category][id] = type;
            }
        }

        m_datLoaded = true;
        return true;
    } catch(stdext::exception& e) {
        g_logger.error(stdext::format("Failed to read dat '%s': %s'", file, e.what()));
        return false;
    }
}

void ThingTypeManager::loadOtb(const std::string& file)
{
    FileStreamPtr fin = g_resources.openFile(file);

    uint signature = fin->getU32();
    if(signature != 0)
        stdext::throw_exception("invalid otb file");

    BinaryTreePtr root = fin->getBinaryTree();

    signature = root->getU32();
    if(signature != 0)
        stdext::throw_exception("invalid otb file");

    root->getU32(); // flags

    m_otbMajorVersion = root->getU32();
    m_otbMinorVersion = root->getU32();
    root->getU32(); // build number
    root->skip(128); // description

    m_itemTypes.resize(root->getChildren().size(), m_nullItemType);
    for(const BinaryTreePtr& node : root->getChildren()) {
        ItemTypePtr itemType(new ItemType);
        itemType->unserialize(node);
        addItemType(itemType);
    }

    m_otbLoaded = true;
}

void ThingTypeManager::loadXml(const std::string& file)
{
    /// Read XML
    TiXmlDocument doc;
    doc.Parse(g_resources.loadFile(file).c_str());
    if(doc.Error())
        stdext::throw_exception(stdext::format("failed to parse '%s': '%s'", file, doc.ErrorDesc()));

    TiXmlElement* root = doc.FirstChildElement();
    if(!root || root->ValueTStr() != "items")
        stdext::throw_exception("invalid root tag name");

    for (TiXmlElement *element = root->FirstChildElement(); element; element = element->NextSiblingElement()) {
        if(element->ValueTStr() != "item")
            continue;

        uint16 id = element->readType<uint16>("id");
        if(id > 20000 && id < 20100) {
            id -= 20000;
            ItemTypePtr newType(new ItemType);
            newType->setServerId(id);
            addItemType(newType);
        }

        if(id != 0)
            parseItemType(id, element);
        else {
            uint16 fromId = element->readType<uint16>("fromid"), toId = element->readType<uint16>("toid");
            for(uint16 i = fromId; i < toId; ++i)
                parseItemType(i, element);
        }
    }

    doc.Clear();
    m_xmlLoaded = true;
    g_logger.debug("items.xml read successfully.");
}

void ThingTypeManager::parseItemType(uint16 id, TiXmlElement* elem)
{
    uint16 serverId = id;
    if(serverId > 20000 && id < 20100) {
        serverId -= 20000;

        ItemTypePtr newType(new ItemType);
        newType->setServerId(serverId);
        addItemType(newType);
    }

    ItemTypePtr itemType = getItemType(serverId);
    itemType->setName(elem->Attribute("name"));
    for(TiXmlElement* attrib = elem->FirstChildElement(); attrib; attrib = attrib->NextSiblingElement()) {
        if(attrib->ValueStr() != "attribute")
            break;

        if(attrib->Attribute("key") == "description") {
            itemType->setDesc(attrib->Attribute("value"));
            break;
        }
    }
}

void ThingTypeManager::addItemType(const ItemTypePtr& itemType)
{
    uint16 id = itemType->getServerId();
    if(m_itemTypes.size() <= id)
        m_itemTypes.resize(id+1, m_nullItemType);
    m_itemTypes[id] = itemType;
}

const ItemTypePtr& ThingTypeManager::findItemTypeByClientId(uint16 id)
{
    if(m_itemTypes.empty())
        return m_nullItemType;

    for(const ItemTypePtr& itemType : m_itemTypes) {
        if(itemType->getClientId() == id)
            return itemType;
    }

    return m_nullItemType;
}

const ThingTypePtr& ThingTypeManager::getThingType(uint16 id, ThingCategory category)
{
    if(category >= ThingLastCategory || id >= m_thingTypes[category].size()) {
        g_logger.error(stdext::format("invalid thing type client id %d in category %d", id, category));
        return m_nullThingType;
    }
    return m_thingTypes[category][id];
}

const ItemTypePtr& ThingTypeManager::getItemType(uint16 id)
{
    if(id >= m_itemTypes.size()) {
        g_logger.error(stdext::format("invalid thing type server id %d", id));
        return m_nullItemType;
    }
    return m_itemTypes[id];
}

ThingTypeList ThingTypeManager::findThingTypeByAttr(ThingAttr attr, ThingCategory category)
{
    ThingTypeList ret;
    for(const ThingTypePtr& type : m_thingTypes[category])
        if(type->hasAttr(attr))
            ret.push_back(type);
    return ret;
}

ItemTypeList ThingTypeManager::findItemTypeByCategory(ItemCategory category)
{
    ItemTypeList ret;
    for(const ItemTypePtr& type : m_itemTypes)
        if(type->getCategory() == category)
            ret.push_back(type);
    return ret;
}

const ThingTypeList& ThingTypeManager::getThingTypes(ThingCategory category)
{
    ThingTypeList ret;
    if(category >= ThingLastCategory)
        stdext::throw_exception(stdext::format("invalid thing type category %d", category));
    return m_thingTypes[category];
}
