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

#define TIXML_USE_STL // use STL strings instead.
#include <framework/thirdparty/tinyxml.h>

ThingTypeManager g_things;

void ThingTypeManager::init()
{
    m_nullDatType = ThingTypeDatPtr(new ThingTypeDat);
    m_nullOtbType = ThingTypeOtbPtr(new ThingTypeOtb);
    m_otbVersion = 0;
    m_datSignature = 0;
    m_otbVersion = 0;
    m_otbMinorVersion = 0;
    m_otbMajorVersion = 0;
    m_datSignature = 0;
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
        if (!fin)
            stdext::throw_exception("unable to open file");

        m_otbVersion = fin->getU32();

        uint32 type = 0;
        uint8 node = fin->readFirstNode(type);

        fin->getU32(); // flags

        uint8 version = fin->getU8();
        if(fin->getU8() == 0x01) { // version
            fin->getU8(); // length
            m_otbMajorVersion = fin->getU32();
            m_otbMinorVersion = fin->getU32();
            fin->getU32(); // build number
        }

        while((node = fin->readNextNode(node, type))) {
            ThingTypeOtbPtr otbType(new ThingTypeOtb);
            otbType->unserialize((OtbCategory)type, fin);
            addOtbType(otbType);
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
    TiXmlDocument doc(name.c_str());
    if (!doc.LoadFile()) {
        g_logger.error(stdext::format("failed to load xml '%s'", name));
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

        ThingTypeOtbPtr iType = getType(id);
        if (!iType) {
            iType = ThingTypeOtbPtr(new ItemData);
            iType->id = idEx ? idEx : id;
            iType->name = name;
            addType(iType->id, iType);
        }

        iType->name = name;

        for (TiXmlElement *attr = element->FirstChildElement(); attr; attr = attr->NextSiblingElement()) {
            if (attr->ValueTStr() != "attribute")
                continue;

            std::string key = attr->Attribute("key");
            std::string value = attr->Attribute("value");
            if (key == "type") {
                if (value == "magicfield")
                    iType->category = IsMagicField;
                else if (value == "key")
                    iType->category = IsKey;
                else if (value == "depot")
                    iType->isDepot = true;
                else if (value == "teleport")
                    iType->category = IsTeleport;
                else if (value == "bed")
                    iType->isBed = true;
                else if (value == "door")
                    iType->category = IsDoor;
            } else if (key == "name") {
                iType->name = value;
            } else if (key == "description") {
                iType->description = value;
            } else if (key == "weight") {
                iType->weight = stdext::unsafe_cast<double>(stdext::unsafe_cast<double>(value) / 100.f);
            } else if (key == "containerSize") {
                int containerSize = stdext::unsafe_cast<int>(value);
                if (containerSize)
                    iType->containerSize = containerSize;
                iType->category = IsContainer;
            } else if (key == "writeable") {
                if (!value.empty())
                    iType->category = IsWritable;
            } else if (key == "maxTextLen") {
                iType->maxTextLength = stdext::unsafe_cast<int>(value);
            } else if (key == "charges") {
                iType->charges = stdext::unsafe_cast<int>(value);
            }
        }
    }

    doc.Clear();
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

ThingTypeDatPtr ThingTypeManager::getDatType(uint16 id, DatCategory category)
{
    if(category >= DatLastCategory || id >= m_datTypes[category].size()) {
        g_logger.error(stdext::format("invalid thing type client id %d in category %d", id, category));
        return m_nullDatType;
    }
    return m_datTypes[category][id];
}

ThingTypeOtbPtr ThingTypeManager::getOtbType(uint16 id)
{
    if(id >= m_otbTypes.size()) {
        g_logger.error(stdext::format("invalid thing type server id %d", id));
        return m_nullOtbType;
    }
    return m_otbTypes[id];
}
