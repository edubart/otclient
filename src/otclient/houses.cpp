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

#include "map.h"

House::House(uint32 hId, const std::string &name, const Position &pos)
    : m_id(hId), m_name(name)
{
    if(pos.isValid())
        m_doors.insert(std::make_pair(0, pos)); // first door
}

void House::addDoor(uint16 doorId, const Position& pos)
{
    if(m_doors.find(doorId) == m_doors.end())
        m_doors.insert(std::make_pair(doorId, pos));
}

void House::setTile(const TilePtr& tile)
{
    tile->setFlags(TILESTATE_HOUSE);
    if(std::find(m_tiles.begin(), m_tiles.end(), tile) == m_tiles.end())
        m_tiles.push_back(tile);
}

#define fugly_get(attrib, type) stdext::unsafe_cast<type>(elem->Attribute((attrib)))

void House::load(const TiXmlElement *elem)
{
    std::string name = elem->Attribute("name");
    if(name.empty())
        name = stdext::format("UnNamed house #%u", getId());

    uint32 rent = fugly_get("rent", uint32);
    m_rent = rent;

    uint32 townId = fugly_get("townid", uint32);
    if(!g_map.getTown(townId))
        stdext::throw_exception(stdext::format("invalid town id for house %d", townId));

    uint32 size = fugly_get("size", uint32);
    if(size == 0)
        size = 1;

    m_size = size;
    m_isGuildHall = fugly_get("guildhall", bool);
    addDoor(0, Position(fugly_get("entryx", uint16), fugly_get("entryy", uint16),
                        fugly_get("entryz", uint8)));
}

void Houses::addHouse(const HousePtr& house)
{
    if(findHouse(house->getId()) == m_houses.end())
        m_houses.push_back(house);
}

void Houses::removeHouse(uint32 houseId)
{
    auto it = findHouse(houseId);
    if(it != m_houses.end())
        m_houses.erase(it);
}

HousePtr Houses::getHouse(uint32 houseId)
{
    auto it = findHouse(houseId);
    if(it != m_houses.end())
        return *it;

    return nullptr;
}

void Houses::load(const std::string& fileName)
{
    TiXmlDocument doc(fileName.c_str());
    if(!doc.LoadFile())
        stdext::throw_exception(stdext::format("failed to load '%s' (House XML)", fileName));

    TiXmlElement *root = doc.FirstChildElement();
    if(!root || root->ValueTStr() != "houses")
        stdext::throw_exception("invalid root tag name");

    for (TiXmlElement *elem = root->FirstChildElement(); elem; elem = elem->NextSiblingElement()) {
        if(elem->ValueTStr() != "house")
            stdext::throw_exception("invalid house tag.");

        uint32 houseId = fugly_get("houseid", uint32);
        HousePtr house = getHouse(houseId);
        if(!house)
            house = HousePtr(new House(houseId)), addHouse(house);

        house->load(elem);
    }

    stdext::throw_exception("This has not been fully implemented yet.");
}

HouseList::iterator Houses::findHouse(uint32 houseId)
{
    return std::find_if(m_houses.begin(), m_houses.end(),
                           [=] (const HousePtr& house) -> bool { return house->getId() == houseId; });
}

