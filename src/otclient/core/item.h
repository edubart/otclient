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

#ifndef ITEM_H
#define ITEM_H

#include <framework/global.h>
#include "thing.h"
#include "thingtypeotb.h"

enum AttrTypes_t {
    ATTR_END = 0,
    //ATTR_DESCRIPTION = 1,
    //ATTR_EXT_FILE = 2,
    ATTR_TILE_FLAGS = 3,
    ATTR_ACTION_ID = 4,
    ATTR_UNIQUE_ID = 5,
    ATTR_TEXT = 6,
    ATTR_DESC = 7,
    ATTR_TELE_DEST = 8,
    ATTR_ITEM = 9,
    ATTR_DEPOT_ID = 10,
    //ATTR_EXT_SPAWN_FILE = 11,
    ATTR_RUNE_CHARGES = 12,
    //ATTR_EXT_HPPOUSE_FILE = 13,
    ATTR_HPPOUSEDOORID = 14,
    ATTR_COUNT = 15,
    ATTR_DURATION = 16,
    ATTR_DECAYING_STATE = 17,
    ATTR_WRITTENDATE = 18,
    ATTR_WRITTENBY = 19,
    ATTR_SLEEPERGUID = 20,
    ATTR_SLEEPSTART = 21,
    ATTR_CHARGES = 22,
    ATTR_CONTAINER_ITEMS = 23,
    ATTR_NAME = 30,
    ATTR_PLURALNAME = 31,
    ATTR_ATTACK = 33,
    ATTR_EXTRAATTACK = 34,
    ATTR_DEFENSE = 35,
    ATTR_EXTRADEFENSE = 36,
    ATTR_ARMOR = 37,
    ATTR_ATTACKSPEED = 38,
    ATTR_HPPITCHANCE = 39,
    ATTR_SHOOTRANGE = 40,
    ATTR_ARTICLE = 41,
    ATTR_SCRIPTPROTECTED = 42,
    ATTR_DUALWIELD = 43,
    ATTR_ATTRIBUTE_MAP = 128
};

// @bindclass
class Item : public Thing
{
public:
    Item();

    static ItemPtr create(int id);
    static ItemPtr createFromOtb(int id);

    void draw(const Point& dest, float scaleFactor, bool animate);
    void drawLight(const Point& dest, float scaleFactor, bool animate, MapView* mapview);

    void setId(uint32 id);
    void setOtbId(uint16 id);
    void setCountOrSubType(int value) { m_countOrSubType = value; }
    void setCount(int count) { m_countOrSubType = count; }
    void setSubType(int subType) { m_countOrSubType = subType; }
    void setActionId(int actionId) { m_actionId = actionId; }
    void setUniqueId(int uniqueId) { m_uniqueId = uniqueId; }
    void setName(const std::string &name) { m_name = name; }
    void setText(const std::string &text) { m_text = text; }
    void setDescription(const std::string &description) { m_description = description; }

    Color getLightColor(){ return Color::from8bit(m_datType->getLightColor());}
    int getLightLevel(){ return (int)m_datType->getLightLevel();}

    int getCountOrSubType() { return m_countOrSubType; }
    int getSubType() { return m_countOrSubType; }
    int getCount() { return m_countOrSubType; }
    uint32 getId() { return m_id; }
    std::string getName() { return m_name; }
    bool isValid();

    ItemPtr asItem() { return std::static_pointer_cast<Item>(shared_from_this()); }
    bool isItem() { return true; }

    // TODO: These should be abstract and declared in i.e containers, doors, etc.
    bool unserializeAttr(const BinaryTreePtr &fin);
    bool unserializeItemNode(const BinaryTreePtr &fin) { return unserializeAttr(fin); }
    void readAttr(AttrTypes_t attrType, const BinaryTreePtr &fin);
    bool isMoveable();

private:
    uint16 m_id;
    uint8 m_countOrSubType;
    uint32 m_actionId, m_uniqueId;
    std::string m_name, m_text, m_description;
    PainterShaderProgramPtr m_shaderProgram;
    ThingTypeOtbPtr m_otbType;
};

#endif
