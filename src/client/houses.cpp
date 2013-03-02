/*
 * Copyright (c) 2010-2013 OTClient <https://github.com/edubart/otclient>
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

#include "map.h"

#include <framework/core/resourcemanager.h>

HouseManager g_houses;

House::House()
{
    m_nullTile = TilePtr(new Tile(Position()));
}

House::House(uint32 hId, const std::string &name, const Position &pos)
{
    m_nullTile = TilePtr(new Tile(Position()));
    setId(hId);
    setName(name);
    if(pos.isValid())
        setEntry(pos);
}

void House::setTile(const TilePtr& tile)
{
    tile->setFlags(TILESTATE_HOUSE);
    m_tiles.insert(std::make_pair(tile->getPosition(), tile));
}

const TilePtr& House::getTile(const Position& position)
{
    TileMap::const_iterator iter = m_tiles.find(position);
    if(iter != m_tiles.end())
        return iter->second;
    return m_nullTile;
}

void House::load(const TiXmlElement *elem)
{
    std::string name = elem->Attribute("name");
    if(name.empty())
        name = stdext::format("Unnamed house #%lu", getId());

    setName(name);
    setRent(elem->readType<uint32>("rent"));
    setSize(elem->readType<uint32>("size"));
    setTownId(elem->readType<uint32>("townid"));
    m_isGuildHall = elem->readType<bool>("guildhall");

    Position entryPos;
    entryPos.x = elem->readType<uint16>("entryx");
    entryPos.y = elem->readType<uint16>("entryy");
    entryPos.z = elem->readType<uint8>("entryz");
    setEntry(entryPos);
}

void House::save(TiXmlElement*& elem)
{
    elem = new TiXmlElement("house");

    elem->SetAttribute("name", getName());
    elem->SetAttribute("houseid", getId());

    Position entry = getEntry();
    elem->SetAttribute("entryx", entry.x);
    elem->SetAttribute("entryy", entry.y);
    elem->SetAttribute("entryz", entry.z);

    elem->SetAttribute("rent", getRent());
    elem->SetAttribute("townid", getTownId());
    elem->SetAttribute("size", getSize());
    elem->SetAttribute("guildhall", (int)m_isGuildHall);
}

HouseManager::HouseManager()
{
    m_nullHouse = HousePtr(new House);
}

void HouseManager::addHouse(const HousePtr& house)
{
    if(findHouse(house->getId()) == m_houses.end())
        m_houses.push_back(house);
}

void HouseManager::removeHouse(uint32 houseId)
{
    auto it = findHouse(houseId);
    if(it != m_houses.end())
        m_houses.erase(it);
}

const HousePtr& HouseManager::getHouse(uint32 houseId)
{
    auto it = std::find_if(m_houses.begin(), m_houses.end(),
                           [=] (const HousePtr& house) -> bool { return house->getId() == houseId; });
    return it != m_houses.end() ? *it : m_nullHouse;
}

void HouseManager::load(const std::string& fileName)
{
    TiXmlDocument doc;
    doc.Parse(g_resources.readFileContents(fileName).c_str());
    if(doc.Error())
        stdext::throw_exception(stdext::format("failed to load '%s': %s (House XML)", fileName, doc.ErrorDesc()));

    TiXmlElement *root = doc.FirstChildElement();
    if(!root || root->ValueTStr() != "houses")
        stdext::throw_exception("invalid root tag name");

    for(TiXmlElement *elem = root->FirstChildElement(); elem; elem = elem->NextSiblingElement()) {
        if(elem->ValueTStr() != "house")
            stdext::throw_exception("invalid house tag.");

        uint32 houseId = elem->readType<uint32>("houseid");
        HousePtr house = getHouse(houseId);
        if(!house)
            house = HousePtr(new House(houseId)), addHouse(house);

        house->load(elem);
    }
}

void HouseManager::save(const std::string& fileName)
{
    TiXmlDocument doc;
    doc.SetTabSize(2);

    TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "UTF-8", "");
    doc.LinkEndChild(decl);

    TiXmlElement* root = new TiXmlElement("houses");
    doc.LinkEndChild(root);

    for(auto house : m_houses) {
        TiXmlElement *elem;
        house->save(elem);
        root->LinkEndChild(elem);
    }

    if(!doc.SaveFile(fileName))
        stdext::throw_exception(stdext::format("failed to save houses XML %s: %s", fileName, doc.ErrorDesc()));
}

HouseList::iterator HouseManager::findHouse(uint32 houseId)
{
    return std::find_if(m_houses.begin(), m_houses.end(),
                           [=] (const HousePtr& house) -> bool { return house->getId() == houseId; });
}
