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

#include "monsters.h"
#include "creature.h"

#include <framework/xml/tinyxml.h>

void Monsters::loadMonsters(const std::string& file)
{
	TiXmlDocument doc(file);
	if(!doc.LoadFile())
		stdext::throw_exception(stdext::format("cannot open monsters file '%s'", file));

	TiXmlElement* root = doc.FirstChildElement();
	if(!root || root->ValueStr() != "monsters")
		stdext::throw_exception("malformed monsters xml file");

	for(TiXmlElement* monster = root->FirstChildElement(); root; root = root->NextSiblingElement()) {
        MonsterPtr newMonster(new Monster);
		newMonster->setName(monster->Attribute("name"));

        loadSingleMonster(file.substr(0, file.find_last_of('/')) + '/' + monster->Attribute("file"), newMonster);
	}

	doc.Clear();
	m_loaded = true;
}

void Monsters::loadSingleMonster(const std::string& file, const MonsterPtr& m)
{
    if (!m || std::find(m_monsters.begin(), m_monsters.end(), m) != m_monsters.end())
		stdext::throw_exception("reloading monsters is not supported yet.");

	TiXmlDocument doc(file);
	if(!doc.LoadFile())
		stdext::throw_exception(stdext::format("cannot load single monster file '%s'", file));

	TiXmlElement* root = doc.FirstChildElement();
	if(!root || root->ValueStr() != "monster")
		stdext::throw_exception(stdext::format("malformed monster xml file: %s", file));

	for(TiXmlElement* attrib = root->FirstChildElement(); attrib; attrib = attrib->NextSiblingElement()) {
		if(attrib->ValueStr() == "look") {
			Outfit out;

            int type = attrib->readType<int>("type");
            if (type <= 0)
                type = attrib->readType<int>("typeex");
            if(type) {
				out.setId(type);
                {
                    out.setHead(attrib->readType<int>(("head")));
                    out.setBody(attrib->readType<int>(("body")));
                    out.setLegs(attrib->readType<int>(("legs")));
                    out.setFeet(attrib->readType<int>(("feet")));
                    out.setAddons(attrib->readType<int>(("addons")));
                    out.setMount(attrib->readType<int>(("mount")));
                }
			} else
				stdext::throw_exception(stdext::format("invalid look type/typeex for monster %s", m->getName()));
            m->setOutfit(out);
		}
	}

	m_monsters.push_back(m);
}

MonsterPtr Monsters::getMonster(const std::string& name)
{
    auto it = std::find_if(m_monsters.begin(), m_monsters.end(),
                           [=] (const MonsterPtr& m) -> bool { return m->getName() == name; });
    return it != m_monsters.end() ? *it : nullptr;
}

MonsterPtr Monsters::getMonsterByPos(const Position& pos)
{
    auto it = std::find_if(m_monsters.begin(), m_monsters.end(),
                           [=] (const MonsterPtr& m) -> bool { return m->getPosition() == pos; });
    return it != m_monsters.end() ? *it : nullptr;
}
