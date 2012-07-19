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

#include "creatures.h"

#include <framework/xml/tinyxml.h>
#include <framework/core/resourcemanager.h>
#include <boost/filesystem.hpp>

void Creatures::loadMonsters(const std::string& file)
{
    TiXmlDocument doc;
    doc.Parse(g_resources.loadFile(file).c_str());
    if(doc.Error())
        stdext::throw_exception(stdext::format("cannot open monsters file '%s': '%s'", file, doc.ErrorDesc()));

    TiXmlElement* root = doc.FirstChildElement();
    if(!root || root->ValueStr() != "monsters")
        stdext::throw_exception("malformed monsters xml file");

    for(TiXmlElement* monster = root->FirstChildElement(); monster; monster = monster->NextSiblingElement()) {
        std::string fname = file.substr(0, file.find_last_of('/')) + '/' + monster->Attribute("file");
        if(fname.substr(fname.length() - 4) != ".xml")
            fname += ".xml";

        loadSingleCreature(fname);
    }

    doc.Clear();
    m_loaded = true;
}

void Creatures::loadSingleCreature(const std::string& file)
{
    return loadCreatureBuffer(g_resources.loadFile(file));
}

void Creatures::loadNpcs(const std::string &folder)
{
    boost::filesystem::path npcPath(folder);
    if(!boost::filesystem::exists(npcPath))
        stdext::throw_exception(stdext::format("NPCs folder '%s' was not found.", folder));

    for(boost::filesystem::directory_iterator it(npcPath), end; it != end; ++it) {
        std::string f = it->path().string();
        if(boost::filesystem::is_directory(it->status()) || ((f.size() > 4 ? f.substr(f.size() - 4) : "") != ".xml"))
            continue;

        loadCreatureBuffer(g_resources.loadFile(f));
    }
}

void Creatures::loadCreatureBuffer(const std::string &buffer)
{
    TiXmlDocument doc;
    doc.Parse(buffer.c_str());
    if(doc.Error())
        stdext::throw_exception(stdext::format("cannot load creature buffer: %s", doc.ErrorDesc()));

    TiXmlElement* root = doc.FirstChildElement();
    if(!root || root->ValueStr() != "npc")
        stdext::throw_exception("invalid root tag name");

    for(TiXmlElement* attrib = root->FirstChildElement(); attrib; attrib = attrib->NextSiblingElement()) {
        if(attrib->ValueStr() != "npc" && attrib->ValueStr() != "monster")
            stdext::throw_exception(stdext::format("invalid attribute '%s'", attrib->ValueStr()));

        CreatureTypePtr newType(nullptr);
        if(m_loadCreatureBuffer(attrib, newType))
            break;
    }

    doc.Clear();
}

bool Creatures::m_loadCreatureBuffer(TiXmlElement* attrib, CreatureTypePtr& m)
{
    if(m || std::find(m_creatures.begin(), m_creatures.end(), m) != m_creatures.end())
        return true;

    m = CreatureTypePtr(new CreatureType(stdext::trim(stdext::tolower(attrib->Attribute("name")))));
    Outfit out;

    int32 type;
    bool isTypeEx=false;
    if(!attrib->Attribute("type").empty())
        type = attrib->readType<int32>("type");
    else {
        type = attrib->readType<int32>("typeex");
        isTypeEx = true;
    }

    out.setId(type);

    if(!isTypeEx) {
        out.setHead(attrib->readType<int>(("head")));
        out.setBody(attrib->readType<int>(("body")));
        out.setLegs(attrib->readType<int>(("legs")));
        out.setFeet(attrib->readType<int>(("feet")));
        out.setAddons(attrib->readType<int>(("addons")));
        out.setMount(attrib->readType<int>(("mount")));
    }
    m->setOutfit(out);
    m_creatures.push_back(m);

    return type >= 0;
}

CreatureTypePtr Creatures::getCreature(const std::string& name)
{
    auto it = std::find_if(m_creatures.begin(), m_creatures.end(),
                           [=] (const CreatureTypePtr& m) -> bool { return m->getName() == stdext::trim(stdext::tolower(name)); });
    return it != m_creatures.end() ? *it : nullptr;
}

CreatureTypePtr Creatures::getCreature(const Position& pos)
{
    auto it = std::find_if(m_creatures.begin(), m_creatures.end(),
                           [=] (const CreatureTypePtr& m) -> bool { return m->getPos() == pos; });
    return it != m_creatures.end() ? *it : nullptr;
}
