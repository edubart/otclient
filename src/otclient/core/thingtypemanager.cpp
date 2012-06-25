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
#include "thingtypedat.h"
#include "thingtypeotb.h"

#include <framework/core/resourcemanager.h>
#include <framework/core/filestream.h>
#include <framework/core/binarytree.h>

#define TIXML_USE_STL // use STL strings instead.
#include <framework/thirdparty/tinyxml.h>

ThingTypeManager g_things;

void ThingTypeManager::init()
{
    m_nullDatType = ThingTypeDatPtr(new ThingTypeDat);
    m_nullOtbType = ThingTypeOtbPtr(new ThingTypeOtb);
    m_datSignature = 0;
    m_otbMinorVersion = 0;
    m_otbMajorVersion = 0;
    m_datLoaded = false;
    m_xmlLoaded = false;
    m_otbLoaded = false;
}

void ThingTypeManager::terminate()
{
    for(int i = 0; i < DatLastCategory; ++i)
        m_datTypes[i].clear();
    m_otbTypes.clear();
    m_nullDatType = nullptr;
    m_nullOtbType = nullptr;
}

bool ThingTypeManager::loadDat(const std::string& file)
{
    try {
        FileStreamPtr fin = g_resources.openFile(file);
        if(!fin)
            stdext::throw_exception("unable to open file");

        m_datSignature = fin->getU32();

        int numThings[DatLastCategory];
        for(int category = 0; category < DatLastCategory; ++category) {
            int count = fin->getU16() + 1;
            m_datTypes[category].resize(count, m_nullDatType);
        }

        for(int category = 0; category < DatLastCategory; ++category) {
            uint16 firstId = 1;
            if(category == DatItemCategory)
                firstId = 100;
            for(uint16 id = firstId; id < m_datTypes[category].size(); ++id) {
                ThingTypeDatPtr type(new ThingTypeDat);
                type->unserialize(id, (DatCategory)category, fin);
                m_datTypes[category][id] = type;
            }
        }

        m_datLoaded = true;
        return true;
    } catch(stdext::exception& e) {
        g_logger.error(stdext::format("Failed to read dat '%s': %s'", file, e.what()));
        return false;
    }
}

bool ThingTypeManager::loadOtb(const std::string& file)
{
    try {
        FileStreamPtr fin = g_resources.openFile(file);
        if(!fin)
            stdext::throw_exception("unable to open file");

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

        for(const BinaryTreePtr& node : root->getChildren()) {
            ThingTypeOtbPtr otbType(new ThingTypeOtb);
            otbType->unserialize(node);
        }

        m_otbLoaded = true;
        return true;
    } catch(stdext::exception& e) {
        g_logger.error(stdext::format("failed to load otb '%s': %s", file, e.what()));
        return false;
    }
}

bool ThingTypeManager::loadXml(const std::string& file)
{
    /*
    try {
        TiXmlDocument doc(file.c_str());
        if (!doc.LoadFile()) {
            g_logger.error(stdext::format("failed to load xml '%s'", file));
            return false;
        }

        TiXmlElement* root = doc.FirstChildElement();
        if (!root) {
            g_logger.error("invalid xml root");
            return false;
        }

        if (root->ValueTStr() != "items") {
            g_logger.error("invalid xml tag name, should be 'items'");
            return false;
        }

        for (TiXmlElement *element = root->FirstChildElement(); element; element = element->NextSiblingElement()) {
            if (element->ValueTStr() != "item")
                continue;

            std::string name = element->Attribute("id");
            if (name.empty())
                continue;

            uint16 id = stdext::unsafe_cast<uint16>(element->Attribute("id"));
            uint16 idEx = 0;
            if (!id) {
                bool found = false;
                // fallback into reading fromid and toid
                uint16 fromid = stdext::unsafe_cast<uint16>(element->Attribute("fromid"));
                uint16 toid = stdext::unsafe_cast<uint16>(element->Attribute("toid"));
                ThingTypeOtbPtr iType;
                for (int __id = fromid; __id < toid; ++__id) {
                    if (!(iType = getType(__id)))
                        continue;

                    iType->name = name;
                    idEx = iType->id == fromid ? fromid : toid;
                    found = true;
                }

                if (!found)
                    continue;
            }

            ThingTypeOtbPtr otbType = getType(id);
            if (!otbType) {
                otbType = ThingTypeOtbPtr(new ItemData);
                otbType->id = idEx ? idEx : id;
                otbType->name = name;
                addType(otbType->id, otbType);
            }

            otbType->name = name;

            for (TiXmlElement *attr = element->FirstChildElement(); attr; attr = attr->NextSiblingElement()) {
                if (attr->ValueTStr() != "attribute")
                    continue;

                std::string key = attr->Attribute("key");
                std::string value = attr->Attribute("value");
                if (key == "type") {
                    if (value == "magicfield")
                        otbType->category = IsMagicField;
                    else if (value == "key")
                        otbType->category = IsKey;
                    else if (value == "depot")
                        otbType->isDepot = true;
                    else if (value == "teleport")
                        otbType->category = IsTeleport;
                    else if (value == "bed")
                        otbType->isBed = true;
                    else if (value == "door")
                        otbType->category = IsDoor;
                } else if (key == "name") {
                    otbType->name = value;
                } else if (key == "description") {
                    otbType->description = value;
                } else if (key == "weight") {
                    otbType->weight = stdext::unsafe_cast<double>(stdext::unsafe_cast<double>(value) / 100.f);
                } else if (key == "containerSize") {
                    int containerSize = stdext::unsafe_cast<int>(value);
                    if (containerSize)
                        otbType->containerSize = containerSize;
                    otbType->category = IsContainer;
                } else if (key == "writeable") {
                    if (!value.empty())
                        otbType->category = IsWritable;
                } else if (key == "maxTextLen") {
                    otbType->maxTextLength = stdext::unsafe_cast<int>(value);
                } else if (key == "charges") {
                    otbType->charges = stdext::unsafe_cast<int>(value);
                }
            }
        }

        doc.Clear();
    } catch(stdext::exception& e) {

    }
    return true;
    */
    return false;
}

void ThingTypeManager::addOtbType(const ThingTypeOtbPtr& otbType)
{
    uint16 id = otbType->getServerId();
    if(m_otbTypes.size() <= id)
        m_otbTypes.resize(id+1, m_nullOtbType);
    m_otbTypes[id] = otbType;
}

const ThingTypeOtbPtr& ThingTypeManager::findOtbForClientId(uint16 id)
{
    if(m_otbTypes.empty())
        return m_nullOtbType;

    for(const ThingTypeOtbPtr& otbType : m_otbTypes) {
        if(otbType->getClientId() == id)
            return otbType;
    }

    return m_nullOtbType;
}

const ThingTypeDatPtr& ThingTypeManager::getDatType(uint16 id, DatCategory category)
{
    if(category >= DatLastCategory || id >= m_datTypes[category].size()) {
        g_logger.error(stdext::format("invalid thing type client id %d in category %d", id, category));
        return m_nullDatType;
    }
    return m_datTypes[category][id];
}

const ThingTypeOtbPtr& ThingTypeManager::getOtbType(uint16 id)
{
    if(id >= m_otbTypes.size()) {
        g_logger.error(stdext::format("invalid thing type server id %d", id));
        return m_nullOtbType;
    }
    return m_otbTypes[id];
}
