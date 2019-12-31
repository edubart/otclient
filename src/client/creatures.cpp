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

#include "creatures.h"
#include "creature.h"
#include "map.h"

#include <framework/xml/tinyxml.h>
#include <framework/core/resourcemanager.h>

CreatureManager g_creatures;

static bool isInZone(const Position& pos/* placePos*/,
                     const Position& centerPos,
                     int radius)
{
    if(radius == -1)
        return true;
    return ((pos.x >= centerPos.x - radius) && (pos.x <= centerPos.x + radius) &&
            (pos.y >= centerPos.y - radius) && (pos.y <= centerPos.y + radius)
           );
}

void CreatureManager::terminate()
{
    clearSpawns();
    clear();
    m_nullCreature = nullptr;
}

void Spawn::load(TiXmlElement* node)
{
    Position centerPos;
    centerPos.x = node->readType<int>("centerx");
    centerPos.y = node->readType<int>("centery");
    centerPos.z = node->readType<int>("centerz");

    setCenterPos(centerPos);
    setRadius(node->readType<int32>("radius"));

    CreatureTypePtr cType(nullptr);
    for(TiXmlElement* cNode = node->FirstChildElement(); cNode; cNode = cNode->NextSiblingElement()) {
        if(cNode->ValueStr() != "monster" && cNode->ValueStr() != "npc")
            stdext::throw_exception(stdext::format("invalid spawn-subnode %s", cNode->ValueStr()));

        std::string cName = cNode->Attribute("name");
        stdext::tolower(cName);
        stdext::trim(cName);
        stdext::ucwords(cName);

        if (!(cType = g_creatures.getCreatureByName(cName)))
            continue;

        cType->setSpawnTime(cNode->readType<int>("spawntime"));
        Otc::Direction dir = Otc::North;
        int16 dir_ = cNode->readType<int16>("direction");
        if(dir_ >= Otc::East && dir_ <= Otc::West)
            dir = (Otc::Direction)dir_;
        cType->setDirection(dir);

        Position placePos;
        placePos.x = centerPos.x + cNode->readType<int>("x");
        placePos.y = centerPos.y + cNode->readType<int>("y");
        placePos.z = cNode->readType<int>("z");

        cType->setRace(cNode->ValueStr() == "npc" ? CreatureRaceNpc : CreatureRaceMonster);
        addCreature(placePos, cType);
    }
}

void Spawn::save(TiXmlElement* node)
{
    const Position& c = getCenterPos();
    node->SetAttribute("centerx", c.x);
    node->SetAttribute("centery", c.y);
    node->SetAttribute("centerz", c.z);

    node->SetAttribute("radius", getRadius());

    TiXmlElement* creatureNode = nullptr;

    for(const auto& pair : m_creatures) {
        const CreatureTypePtr& creature = pair.second;
        if(!(creatureNode = new TiXmlElement(creature->getRace() == CreatureRaceNpc ? "npc" : "monster")))
            stdext::throw_exception("Spawn::save: Ran out of memory while allocating XML element!  Terminating now.");

        creatureNode->SetAttribute("name", creature->getName());
        creatureNode->SetAttribute("spawntime", creature->getSpawnTime());
        creatureNode->SetAttribute("direction", creature->getDirection());

        const Position& placePos = pair.first;
        assert(placePos.isValid());

        creatureNode->SetAttribute("x", placePos.x - c.x);
        creatureNode->SetAttribute("y", placePos.y - c.y);
        creatureNode->SetAttribute("z", placePos.z);

        node->LinkEndChild(creatureNode);
    }
}

void Spawn::addCreature(const Position& placePos, const CreatureTypePtr& cType)
{
    const Position& centerPos = getCenterPos();
    int m_radius = getRadius();
    if(!isInZone(placePos, centerPos, m_radius)) {
        g_logger.warning(stdext::format("cannot place creature at %s (spawn's center position: %s, spawn radius: %d) (increment radius)",
                                               stdext::to_string(placePos), stdext::to_string(centerPos),
                                               m_radius
                                              ));
        return;
    }

    g_map.addThing(cType->cast(), placePos, 4);
    m_creatures.insert(std::make_pair(placePos, cType));
}

void Spawn::removeCreature(const Position& pos)
{
    auto iterator = m_creatures.find(pos);
    if(iterator != m_creatures.end()) {
        assert(iterator->first.isValid());
        assert(g_map.removeThingByPos(iterator->first, 4));
        m_creatures.erase(iterator);
    }
}

std::vector<CreatureTypePtr> Spawn::getCreatures()
{
    std::vector<CreatureTypePtr> creatures;
    for (auto p : m_creatures)
        creatures.push_back(p.second);
    return creatures;
}

CreaturePtr CreatureType::cast()
{
    CreaturePtr ret(new Creature);

    std::string cName = getName();
    stdext::tolower(cName);
    stdext::trim(cName);
    stdext::ucwords(cName);
    ret->setName(cName);

    ret->setDirection(getDirection());
    ret->setOutfit(getOutfit());
    return ret;
}

CreatureManager::CreatureManager()
{
    m_nullCreature = CreatureTypePtr(new CreatureType);
}

void CreatureManager::clearSpawns()
{
    for(auto pair : m_spawns)
        pair.second->clear();
    m_spawns.clear();
}

void CreatureManager::loadMonsters(const std::string& file)
{
    TiXmlDocument doc;
    doc.Parse(g_resources.readFileContents(file).c_str());
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

void CreatureManager::loadSingleCreature(const std::string& file)
{
    loadCreatureBuffer(g_resources.readFileContents(file));
}

void CreatureManager::loadNpcs(const std::string& folder)
{
    std::string tmp = folder;
    if(!stdext::ends_with(tmp, "/"))
        tmp += "/";

    if(!g_resources.directoryExists(tmp))
        stdext::throw_exception(stdext::format("NPCs folder '%s' was not found.", folder));

    const auto& fileList = g_resources.listDirectoryFiles(tmp);
    for(const std::string& file : fileList)
        loadCreatureBuffer(g_resources.readFileContents(tmp + file));
}

void CreatureManager::loadSpawns(const std::string& fileName)
{
    if(!isLoaded()) {
        g_logger.warning("creatures aren't loaded yet to load spawns.");
        return;
    }

    if(m_spawnLoaded) {
        g_logger.warning("attempt to reload spawns.");
        return;
    }

    try {
        TiXmlDocument doc;
        doc.Parse(g_resources.readFileContents(fileName).c_str());
        if(doc.Error())
            stdext::throw_exception(stdext::format("cannot load spawns xml file '%s: '%s'", fileName, doc.ErrorDesc()));

        TiXmlElement* root = doc.FirstChildElement();
        if(!root || root->ValueStr() != "spawns")
            stdext::throw_exception("malformed spawns file");

        for(TiXmlElement* node = root->FirstChildElement(); node; node = node->NextSiblingElement()) {
            if(node->ValueTStr() != "spawn")
                stdext::throw_exception("invalid spawn node");

            SpawnPtr spawn(new Spawn);
            spawn->load(node);
            m_spawns.insert(std::make_pair(spawn->getCenterPos(), spawn));
        }
        doc.Clear();
        m_spawnLoaded = true;
    } catch(std::exception& e) {
        g_logger.error(stdext::format("Failed to load '%s': %s", fileName, e.what()));
    }
}

void CreatureManager::saveSpawns(const std::string& fileName)
{
    try {
        TiXmlDocument doc;
        doc.SetTabSize(2);

        TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "UTF-8", "");
        doc.LinkEndChild(decl);

        TiXmlElement* root = new TiXmlElement("spawns");
        doc.LinkEndChild(root);

        for(auto pair : m_spawns) {
            TiXmlElement* elem = new TiXmlElement("spawn");
            pair.second->save(elem);
            root->LinkEndChild(elem);
        }

        if(!doc.SaveFile("data"+fileName))
            stdext::throw_exception(stdext::format("failed to save spawns XML %s: %s", fileName, doc.ErrorDesc()));
    } catch(std::exception& e) {
        g_logger.error(stdext::format("Failed to save '%s': %s", fileName, e.what()));
    }
}

void CreatureManager::loadCreatureBuffer(const std::string& buffer)
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
    stdext::ucwords(cName);

    CreatureTypePtr newType(new CreatureType(cName));
    for(TiXmlElement* attrib = root->FirstChildElement(); attrib; attrib = attrib->NextSiblingElement()) {
        if(attrib->ValueStr() != "look")
            continue;

        internalLoadCreatureBuffer(attrib, newType);
        break;
    }

    doc.Clear();
}

void CreatureManager::internalLoadCreatureBuffer(TiXmlElement* attrib, const CreatureTypePtr& m)
{
    if(std::find(m_creatures.begin(), m_creatures.end(), m) != m_creatures.end())
        return;

    Outfit out;

    int32 type = attrib->readType<int32>("type");
    if(type > 0) {
        out.setCategory(ThingCategoryCreature);
        out.setId(type);
    } else {
        out.setCategory(ThingCategoryItem);
        out.setAuxId(attrib->readType<int32>("typeex"));
    }

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
}

const CreatureTypePtr& CreatureManager::getCreatureByName(std::string name)
{
    stdext::tolower(name);
    stdext::trim(name);
    stdext::ucwords(name);
    auto it = std::find_if(m_creatures.begin(), m_creatures.end(),
                           [=] (const CreatureTypePtr& m) -> bool { return m->getName() == name; });
    if(it != m_creatures.end())
        return *it;
    g_logger.warning(stdext::format("could not find creature with name: %s", name));
    return m_nullCreature;
}

const CreatureTypePtr& CreatureManager::getCreatureByLook(int look)
{
    auto findFun = [=] (const CreatureTypePtr& c) -> bool
    {
        const Outfit& o = c->getOutfit();
        return o.getId() == look || o.getAuxId() == look;
    };
    auto it = std::find_if(m_creatures.begin(), m_creatures.end(), findFun);
    if(it != m_creatures.end())
        return *it;
    g_logger.warning(stdext::format("could not find creature with looktype: %d", look));
    return m_nullCreature;
}

SpawnPtr CreatureManager::getSpawn(const Position& centerPos)
{
    auto it = m_spawns.find(centerPos);
    if(it != m_spawns.end())
        return it->second;
    g_logger.debug(stdext::format("failed to find spawn at center %s",stdext::to_string(centerPos)));
    return nullptr;
}

SpawnPtr CreatureManager::getSpawnForPlacePos(const Position& pos)
{
    for (const auto& pair : m_spawns) {
        const Position& centerPos = pair.first;
        const SpawnPtr& spawn = pair.second;

        if (isInZone(pos, centerPos, spawn->getRadius()))
            return spawn;
    }

    return nullptr;
}

SpawnPtr CreatureManager::addSpawn(const Position& centerPos, int radius)
{
    auto iter = m_spawns.find(centerPos);
    if(iter != m_spawns.end()) {
        if(iter->second->getRadius() != radius)
            iter->second->setRadius(radius);
        return iter->second;
    }

    SpawnPtr ret(new Spawn);

    ret->setRadius(radius);
    ret->setCenterPos(centerPos);

    m_spawns.insert(std::make_pair(centerPos, ret));
    return ret;
}

void CreatureManager::deleteSpawn(const SpawnPtr& spawn)
{
    const Position& centerPos = spawn->getCenterPos();
    auto it = m_spawns.find(centerPos);
    if(it != m_spawns.end())
        m_spawns.erase(it);
}

std::vector<SpawnPtr> CreatureManager::getSpawns()
{
    std::vector<SpawnPtr> spawns;
    for (auto p : m_spawns)
        spawns.push_back(p.second);
    return spawns;
}

/* vim: set ts=4 sw=4 et: */
