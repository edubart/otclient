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
#include "creature.h"

#include <framework/xml/tinyxml.h>
#include <framework/core/resourcemanager.h>
#include <boost/filesystem.hpp>

Creatures g_creatures;

CreaturePtr CreatureType::cast()
{
    CreaturePtr ret(new Creature);
    ret->setName(getName());
    ret->setOutfit(getOutfit());
    return ret;
}

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
    loadCreatureBuffer(g_resources.loadFile(file));
}

void Creatures::loadNpcs(const std::string& folder)
{
    boost::filesystem::path npcPath(boost::filesystem::current_path().generic_string() + folder);
    if(!boost::filesystem::exists(npcPath))
        stdext::throw_exception(stdext::format("NPCs folder '%s' was not found.", folder));

    for(boost::filesystem::directory_iterator it(npcPath), end; it != end; ++it) {
        std::string f = it->path().filename().string();
        if(boost::filesystem::is_directory(it->status()))
            continue;

        std::string tmp = folder;
        if(!stdext::ends_with(tmp, "/"))
            tmp += "/";
        loadCreatureBuffer(g_resources.loadFile(tmp + f));
    }
}

void Creatures::loadCreatureBuffer(const std::string &buffer)
{
    TiXmlDocument doc;
    doc.Parse(buffer.c_str());
    if(doc.Error())
        stdext::throw_exception(stdext::format("cannot load creature buffer: %s", doc.ErrorDesc()));

    TiXmlElement* root = doc.FirstChildElement();
    if(!root || (root->ValueStr() != "monster" && root->ValueStr() != "npc"))
        stdext::throw_exception("invalid root tag name");

    std::string cName = root->Attribute("name");
    stdext::tolower(cName);
    stdext::trim(cName);

    CreatureTypePtr newType(new CreatureType(cName));
    for(TiXmlElement* attrib = root->FirstChildElement(); attrib; attrib = attrib->NextSiblingElement()) {
        if(attrib->ValueStr() != "look")
            continue;

        if(m_loadCreatureBuffer(attrib, newType))
            break;
    }

    doc.Clear();
}

bool Creatures::m_loadCreatureBuffer(TiXmlElement* attrib, const CreatureTypePtr& m)
{
    if(std::find(m_creatures.begin(), m_creatures.end(), m) != m_creatures.end())
        return true;

    Outfit out;
    out.setCategory(ThingCategoryCreature);
    if(!attrib->Attribute("type").empty())
        out.setId(attrib->readType<int32>("type"));
    else
        out.setAuxId(attrib->readType<int32>("typeex"));

    {
        out.setHead(attrib->readType<int>(("head")));
        out.setBody(attrib->readType<int>(("body")));
        out.setLegs(attrib->readType<int>(("legs")));
        out.setFeet(attrib->readType<int>(("feet")));
        out.setAddons(attrib->readType<int>(("addons")));
        out.setMount(attrib->readType<int>(("mount")));
    }

    m->setOutfit(out);
    m_creatures.push_back(m);
    return true;
}

CreatureTypePtr Creatures::getCreatureByName(std::string name)
{
    stdext::tolower(name);
    stdext::trim(name);
    auto it = std::find_if(m_creatures.begin(), m_creatures.end(),
                           [=] (const CreatureTypePtr& m) -> bool { return m->getName() == name; });
    return it != m_creatures.end() ? *it : nullptr;
}

CreatureTypePtr Creatures::getCreatureByLook(int look)
{
    auto findFun = [=] (const CreatureTypePtr& c) -> bool
    {
        const Outfit& o = c->getOutfit();
        return o.getId() == look || o.getAuxId() == look;
    };
    auto it = std::find_if(m_creatures.begin(), m_creatures.end(), findFun);
    return it != m_creatures.end() ? *it : nullptr;
}
