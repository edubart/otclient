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

#ifndef TOWNS_H
#define TOWNS_H

#include "declarations.h"
#include <framework/luaengine/luaobject.h>

class Town : public LuaObject
{
public:
    Town() { }
    Town(uint32 tid, const std::string& name, const Position& pos=Position());

    void setId(uint32 tid) { m_id = tid; }
    void setName(const std::string& name) { m_name = name; }
    void setPos(const Position& pos) { m_pos = pos; }

    uint32 getId() { return m_id; }
    std::string getName() { return m_name; }
    Position getPos() { return m_pos; }

private:
    uint32 m_id;
    std::string m_name;
    Position m_pos; // temple pos
};

class TownManager
{
public:
    TownManager();

    void addTown(const TownPtr& town);
    void removeTown(uint32 townId);
    const TownPtr& getTown(uint32 townId);
    const TownPtr& getTownByName(std::string name);

    void sort();
    TownList getTowns() { return m_towns; }
    void clear() { m_towns.clear(); m_nullTown = nullptr; }

private:
    TownList m_towns;
    TownPtr m_nullTown;

protected:
    TownList::iterator findTown(uint32 townId);
};

extern TownManager g_towns;

#endif
