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

#ifndef CREATURES_H
#define CREATURES_H

#include "declarations.h"
#include <framework/luaengine/luaobject.h>
#include <framework/util/attribstorage.h>
#include "outfit.h"

enum CreatureAttributes : unsigned char
{
    CreatureAttribPos,
    CreatureAttribName,
    CreatureAttribOutfit,
    CreatureAttribSpawnTime
};

class CreatureType : public LuaObject
{
public:
    CreatureType() { }
    CreatureType(const std::string& name) { setName(name); }

    void setPos(const Position& pos) { m_attribs.set(CreatureAttribPos, pos); }
    void setName(const std::string& name) { m_attribs.set(CreatureAttribName, name); }
    void setOutfit(const Outfit& o) { m_attribs.set(CreatureAttribOutfit, o); }
    void setSpawnTime(int spawnTime) { m_attribs.set(CreatureAttribSpawnTime, spawnTime); }

    std::string getName() { return m_attribs.get<std::string>(CreatureAttribName); }
    Position getPos() { return m_attribs.get<Position>(CreatureAttribPos); }
    Outfit getOutfit() { return m_attribs.get<Outfit>(CreatureAttribOutfit); }
    int getSpawnTime() { return m_attribs.get<int>(CreatureAttribSpawnTime); }

private:
    AttribStorage m_attribs;
};

class Creatures
{
public:
    void clear() { m_creatures.clear(); }

    void loadMonsters(const std::string& file);
    void loadSingleCreature(const std::string& file);
    void loadNpcs(const std::string& folder);
    void loadCreatureBuffer(const std::string& buffer);

    CreatureTypePtr getCreature(const std::string& name);
    CreatureTypePtr getCreature(const Position& pos);

    bool isLoaded() const { return m_loaded; }

protected:
    bool m_loadCreatureBuffer(TiXmlElement* elem, CreatureTypePtr& m);

private:
    std::vector<CreatureTypePtr> m_creatures;
    Boolean<false> m_loaded;
};

#endif
