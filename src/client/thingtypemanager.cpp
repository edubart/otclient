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

#include "thingtypemanager.h"
#include "spritemanager.h"
#include "thing.h"
#include "thingtype.h"
#include "itemtype.h"
#include "creature.h"
#include "creatures.h"
#include "game.h"

#include <framework/core/resourcemanager.h>
#include <framework/core/filestream.h>
#include <framework/core/binarytree.h>
#include <framework/xml/tinyxml.h>
#include <framework/otml/otml.h>

ThingTypeManager g_things;

void ThingTypeManager::init()
{
    m_nullThingType = ThingTypePtr(new ThingType);
    m_nullItemType = ItemTypePtr(new ItemType);
    m_datSignature = 0;
    m_contentRevision = 0;
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
    m_reverseItemTypes.clear();
    m_nullThingType = nullptr;
    m_nullItemType = nullptr;
}

void ThingTypeManager::saveDat(std::string fileName)
{
    if(!m_datLoaded)
        stdext::throw_exception("failed to save, dat is not loaded");

    try {
        FileStreamPtr fin = g_resources.createFile(fileName);
        if(!fin)
            stdext::throw_exception(stdext::format("failed to open file '%s' for write", fileName));

        fin->cache();

        fin->addU32(m_datSignature);

        for(int category = 0; category < ThingLastCategory; ++category)
            fin->addU16(m_thingTypes[category].size() - 1);

        for(int category = 0; category < ThingLastCategory; ++category) {
            uint16 firstId = 1;
            if(category == ThingCategoryItem)
                firstId = 100;

            for(uint16 id = firstId; id < m_thingTypes[category].size(); ++id)
                m_thingTypes[category][id]->serialize(fin);
        }


        fin->flush();
        fin->close();
    } catch(std::exception& e) {
        g_logger.error(stdext::format("Failed to save '%s': %s", fileName, e.what()));
    }
}

bool ThingTypeManager::loadDat(std::string file)
{
    m_datLoaded = false;
    m_datSignature = 0;
    m_contentRevision = 0;
    try {
        file = g_resources.guessFilePath(file, "dat");

        FileStreamPtr fin = g_resources.openFile(file);

        m_datSignature = fin->getU32();
        m_contentRevision = static_cast<uint16_t>(m_datSignature);

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
        g_lua.callGlobalField("g_things", "onLoadDat", file);
        return true;
    } catch(stdext::exception& e) {
        g_logger.error(stdext::format("Failed to read dat '%s': %s'", file, e.what()));
        return false;
    }
}

bool ThingTypeManager::loadOtml(std::string file)
{
    try {
        file = g_resources.guessFilePath(file, "otml");

        OTMLDocumentPtr doc = OTMLDocument::parse(file);
        for(const OTMLNodePtr& node : doc->children()) {
            ThingCategory category;
            if(node->tag() == "creatures")
                category = ThingCategoryCreature;
            else if(node->tag() == "items")
                category = ThingCategoryItem;
            else if(node->tag() == "effects")
                category = ThingCategoryEffect;
            else if(node->tag() == "missiles")
                category = ThingCategoryMissile;
            else {
                throw OTMLException(node, "not a valid thing category");
            }

            for(const OTMLNodePtr& node2 : node->children()) {
                uint16 id = stdext::safe_cast<uint16>(node2->tag());
                ThingTypePtr type = getThingType(id, category);
                if(!type)
                    throw OTMLException(node2, "thing not found");
                type->unserializeOtml(node2);
            }
        }
        return true;
    } catch(std::exception& e) {
        g_logger.error(stdext::format("Failed to read dat otml '%s': %s'", file, e.what()));
        return false;
    }
}

void ThingTypeManager::loadOtb(const std::string& file)
{
    try {
        FileStreamPtr fin = g_resources.openFile(file);

        uint signature = fin->getU32();
        if(signature != 0)
            stdext::throw_exception("invalid otb file");

        BinaryTreePtr root = fin->getBinaryTree();
        root->skip(1); // otb first byte is always 0

        signature = root->getU32();
        if(signature != 0)
            stdext::throw_exception("invalid otb file");

        uint8 rootAttr = root->getU8();
        if(rootAttr == 0x01) { // OTB_ROOT_ATTR_VERSION
            uint16 size = root->getU16();
            if(size != 4 + 4 + 4 + 128)
                stdext::throw_exception("invalid otb root attr version size");

            m_otbMajorVersion = root->getU32();
            m_otbMinorVersion = root->getU32();
            root->skip(4); // buildNumber
            root->skip(128); // description
        }

        m_reverseItemTypes.clear();
        m_itemTypes.resize(root->getChildren().size() + 1, m_nullItemType);
        m_reverseItemTypes.resize(root->getChildren().size() + 1, m_nullItemType);

        for(const BinaryTreePtr& node : root->getChildren()) {
            ItemTypePtr itemType(new ItemType);
            itemType->unserialize(node);
            addItemType(itemType);

            uint16 clientId = itemType->getClientId();
            if(unlikely(clientId >= m_reverseItemTypes.size()))
                m_reverseItemTypes.resize(clientId + 1);
            m_reverseItemTypes[clientId] = itemType;
        }

        m_otbLoaded = true;
        g_lua.callGlobalField("g_things", "onLoadOtb", file);
    } catch(std::exception& e) {
        g_logger.error(stdext::format("Failed to load '%s' (OTB file): %s", file, e.what()));
    }
}

void ThingTypeManager::loadXml(const std::string& file)
{
    try {
        if(!isOtbLoaded())
            stdext::throw_exception("OTB must be loaded before XML");

        TiXmlDocument doc;
        doc.Parse(g_resources.readFileContents(file).c_str());
        if(doc.Error())
            stdext::throw_exception(stdext::format("failed to parse '%s': '%s'", file, doc.ErrorDesc()));

        TiXmlElement* root = doc.FirstChildElement();
        if(!root || root->ValueTStr() != "items")
            stdext::throw_exception("invalid root tag name");

        for(TiXmlElement *element = root->FirstChildElement(); element; element = element->NextSiblingElement()) {
            if(unlikely(element->ValueTStr() != "item"))
                continue;

            uint16 id = element->readType<uint16>("id");
            if(id != 0) {
                std::vector<std::string> s_ids = stdext::split(element->Attribute("id"), ";");
                for(const std::string& s : s_ids) {
                    std::vector<int32> ids = stdext::split<int32>(s, "-");
                    if(ids.size() > 1) {
                        int32 i = ids[0];
                        while(i <= ids[1])
                            parseItemType(i++, element);
                    } else
                        parseItemType(atoi(s.c_str()), element);
                }
            } else {
                std::vector<int32> begin = stdext::split<int32>(element->Attribute("fromid"), ";");
                std::vector<int32> end   = stdext::split<int32>(element->Attribute("toid"), ";");
                if(begin[0] && begin.size() == end.size()) {
                    size_t size = begin.size();
                    for(size_t i = 0; i < size; ++i)
                        while(begin[i] <= end[i])
                            parseItemType(begin[i]++, element);
                }
            }
        }

        doc.Clear();
        m_xmlLoaded = true;
        g_logger.debug("items.xml read successfully.");
    } catch(std::exception& e) {
        g_logger.error(stdext::format("Failed to load '%s' (XML file): %s", file, e.what()));
    }
}

void ThingTypeManager::parseItemType(uint16 serverId, TiXmlElement* elem)
{
    ItemTypePtr itemType = nullptr;

    bool s;
    int d;

    if(g_game.getClientVersion() < 960) {
        s = serverId > 20000 && serverId < 20100;
        d = 20000;
    } else {
        s = serverId > 30000 && serverId < 30100;
        d = 30000;
    }

    if(s) {
        serverId -= d;
        itemType = ItemTypePtr(new ItemType);
        itemType->setServerId(serverId);
        addItemType(itemType);
    } else
        itemType = getItemType(serverId);

    itemType->setName(elem->Attribute("name"));
    for(TiXmlElement* attrib = elem->FirstChildElement(); attrib; attrib = attrib->NextSiblingElement()) {
        std::string key = attrib->Attribute("key");
        if(key.empty())
            continue;

        stdext::tolower(key);
        if(key == "description")
            itemType->setDesc(attrib->Attribute("value"));
        else if(key == "weapontype")
            itemType->setCategory(ItemCategoryWeapon);
        else if(key == "ammotype")
            itemType->setCategory(ItemCategoryAmmunition);
        else if(key == "armor")
            itemType->setCategory(ItemCategoryArmor);
        else if(key == "charges")
            itemType->setCategory(ItemCategoryCharges);
        else if(key == "type") {
            std::string value = attrib->Attribute("value");
            stdext::tolower(value);

            if(value == "key")
                itemType->setCategory(ItemCategoryKey);
            else if(value == "magicfield")
                itemType->setCategory(ItemCategoryMagicField);
            else if(value == "teleport")
                itemType->setCategory(ItemCategoryTeleport);
            else if(value == "door")
                itemType->setCategory(ItemCategoryDoor);
        }
    }
}

void ThingTypeManager::addItemType(const ItemTypePtr& itemType)
{
    uint16 id = itemType->getServerId();
    if(unlikely(id >= m_itemTypes.size()))
        m_itemTypes.resize(id + 1, m_nullItemType);
    m_itemTypes[id] = itemType;
}

const ItemTypePtr& ThingTypeManager::findItemTypeByClientId(uint16 id)
{
    if(id == 0 || id >= m_reverseItemTypes.size())
        return m_nullItemType;

    if(m_reverseItemTypes[id])
        return m_reverseItemTypes[id];
    else
        return m_nullItemType;
}

const ItemTypePtr& ThingTypeManager::findItemTypeByName(std::string name)
{
    for(const ItemTypePtr& it : m_itemTypes)
        if(it->getName() == name)
            return it;
    return m_nullItemType;
}

ItemTypeList ThingTypeManager::findItemTypesByName(std::string name)
{
    ItemTypeList ret;
    for(const ItemTypePtr& it : m_itemTypes)
        if(it->getName() == name)
            ret.push_back(it);
    return ret;
}

ItemTypeList ThingTypeManager::findItemTypesByString(std::string name)
{
    ItemTypeList ret;
    for(const ItemTypePtr& it : m_itemTypes)
        if(it->getName().find(name) != std::string::npos)
            ret.push_back(it);
    return ret;
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
    if(id >= m_itemTypes.size() || m_itemTypes[id] == m_nullItemType) {
        g_logger.error(stdext::format("invalid thing type, server id: %d", id));
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

/* vim: set ts=4 sw=4 et: */
