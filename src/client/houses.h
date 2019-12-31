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

#ifndef HOUSES_H
#define HOUSES_H

#include "declarations.h"
#include "tile.h"

#include <framework/luaengine/luaobject.h>

enum HouseAttr : uint8
{
    HouseAttrId,
    HouseAttrName,
    HouseAttrTown,
    HouseAttrEntry,
    HouseAttrSize,
    HouseAttrRent
};

class House : public LuaObject
{
public:
    House();
    House(uint32 hId, const std::string& name = "", const Position& pos=Position());
    ~House() { m_tiles.clear(); }

    void setTile(const TilePtr& tile);
    TilePtr getTile(const Position& pos);

    void setName(const std::string& name) { m_attribs.set(HouseAttrName, name); }
    std::string getName() { return m_attribs.get<std::string>(HouseAttrName); }

    void setId(uint32 hId) { m_attribs.set(HouseAttrId, hId); }
    uint32 getId() { return m_attribs.get<uint32>(HouseAttrId); }

    void setTownId(uint32 tid) { m_attribs.set(HouseAttrTown, tid); }
    uint32 getTownId() { return m_attribs.get<uint32>(HouseAttrTown); }

    void setSize(uint32 s) { m_attribs.set(HouseAttrSize, s); }
    uint32 getSize() { return m_attribs.get<uint32>(HouseAttrSize); }

    void setRent(uint32 r) { m_attribs.set(HouseAttrRent, r); }
    uint32 getRent() { return m_attribs.get<uint32>(HouseAttrRent); }

    void setEntry(const Position& p) { m_attribs.set(HouseAttrEntry, p); }
    Position getEntry() { return m_attribs.get<Position>(HouseAttrEntry); }

    void addDoor(const ItemPtr& door);
    void removeDoor(const ItemPtr& door) { removeDoorById(door->getDoorId()); }
    void removeDoorById(uint32 doorId);

protected:
    void load(const TiXmlElement* elem);
    void save(TiXmlElement* elem);

private:
    stdext::packed_storage<uint8> m_attribs;
    TileMap m_tiles;
    ItemVector m_doors;
    uint32 m_lastDoorId;
    stdext::boolean<false> m_isGuildHall;

    friend class HouseManager;
};

class HouseManager {
public:
    HouseManager();

    void addHouse(const HousePtr& house);
    void removeHouse(uint32 houseId);
    HousePtr getHouse(uint32 houseId);
    HousePtr getHouseByName(std::string name);

    void load(const std::string& fileName);
    void save(const std::string& fileName);

    void sort();
    void clear() { m_houses.clear(); }
    HouseList getHouseList() { return m_houses; }
    HouseList filterHouses(uint32 townId);

private:
    HouseList m_houses;

protected:
    HouseList::iterator findHouse(uint32 houseId);
};

extern HouseManager g_houses;

#endif
