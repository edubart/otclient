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


#ifndef THINGTYPEOTB_H
#define THINGTYPEOTB_H

#include <framework/core/declarations.h>
#include <framework/luaengine/luaobject.h>
#include <framework/xml/tinyxml.h>
#include <framework/util/attribstorage.h>


enum OtbCategory {
    OtbInvalidCateogry = 0,
    OtbGround,
    OtbContainer,
    OtbWeapon,
    OtbAmmunition,
    OtbArmor,
    OtbCharges,
    OtbTeleport,
    OtbMagicField,
    OtbWritable,
    OtbKey,
    OtbSplash,
    OtbFluid,
    OtbDoor,
    OtbLastCategory
};

enum OtbAttrib {
    OtbAttribFirst = 0x10,
    OtbAttribServerId = OtbAttribFirst,
    OtbAttribClientId,
    OtbAttribName,               // deprecated
    OtbAttribDesc,               // deprecated
    OtbAttribSpeed,
    OtbAttribSlot,               // deprecated
    OtbAttribMaxItems,           // deprecated
    OtbAttribWeight,             // deprecated
    OtbAttribWeapon,             // deprecated
    OtbAttribAmu,                // deprecated
    OtbAttribArmor,              // deprecated
    OtbAttribMagLevel,           // deprecated
    OtbAttribMagicField,         // deprecated
    OtbAttribWritable,           // deprecated
    OtbAttribRotateTo,           // deprecated
    OtbAttribDecay,              // deprecated
    OtbAttribSpriteHash,
    OtbAttribMinimapColor,
    OtbAttrib07,
    OtbAttrib08,
    OtbAttribLight,
    OtbAttribDecay2,             // deprecated
    OtbAttribWeapon2,            // deprecated
    OtbAttribAmu2,               // deprecated
    OtbAttribArmor2,             // deprecated
    OtbAttribWritable2,          // deprecated
    OtbAttribLight2,
    OtbAttribTopOrder,
    OtbAttribWrtiable3,          // deprecated
    OtbLastAttrib
};

class ThingTypeOtb : public LuaObject
{
public:
    ThingTypeOtb();

    void unserialize(const BinaryTreePtr& node);

    uint16 getServerId() { return m_attribs.get<uint16>(OtbAttribServerId); }
    uint16 getClientId() { return m_attribs.get<uint16>(OtbAttribClientId); }
    OtbCategory getCategory() { return m_category; }
    std::string getName() { return m_attribs.get<std::string>(OtbAttribName); }
    std::string getDesc() { return m_attribs.get<std::string>(OtbAttribDesc); }

    bool isNull() { return m_null; }

    void setClientId(uint16 clientId) { m_attribs.set(OtbAttribClientId, clientId); }
    void setServerId(uint16 serverId) { m_attribs.set(OtbAttribServerId, serverId); }
    void setName(const std::string& name) { m_attribs.set(OtbAttribName, name); }
    void setDesc(const std::string& desc) { m_attribs.set(OtbAttribDesc, desc); }

private:
    OtbCategory m_category;
    Boolean<true> m_null;

    AttribStorage m_attribs;
};

#endif
