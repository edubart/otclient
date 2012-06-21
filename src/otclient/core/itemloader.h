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

#ifndef OTBLOADER_H
#define OTBLOADER_H

#include "declarations.h"

enum ItemGroup {
    IsGround,
    IsContainer,
    IsWeapon,
    IsAmmunition,
    IsArmor,
    IsCharges,
    IsTeleport,
    IsMagicField,
    IsWritable,
    IsKey,
    IsSplash,
    IsFluid,
    IsDoor,
    InvalidGroup
};

enum ItemAttrib {
    First = 0x10,
    ServerId = First,
    ClientId,
    Name,             /* deprecated */
    Desc,            /*deprecated*/
    Speed,
    Slot,                /*deprecated*/
    MaxItems,            /*deprecated*/
    Weight,            /*deprecated*/
    Weapon,            /*deprecated*/
    Amu,                /*deprecated*/
    Armor,            /*deprecated*/
    MagLevel,            /*deprecated*/
    MagicField,        /*deprecated*/
    Writable,        /*deprecated*/
    RotateTo,            /*deprecated*/
    Decay,            /*deprecated*/
    SpriteHash,
    MinimapColor,
    Attr07,
    Attr08,
    Light,

    // 1-byte aligned
    Decay2,            /*deprecated*/
    Weapon2,            /*deprecated*/
    Amu2,                /*deprecated*/
    Armor2,            /*deprecated*/
    Writable2,        /*deprecated*/
    Light2,
    TopOrder,
    Wrtiable3        /*deprecated*/
};

struct ItemData {
    uint16 id;
    uint16 clientId;
    std::string name, description;
    int containerSize;
    ItemGroup group;
    double weight;

    // xml stuff.
    bool isDepot;
    bool isBed;
    uint32 maxTextLength;
    uint32 charges;
};

class ItemLoader
{
public:
    uint32 dwMajorVersion;
    uint32 dwMinorVersion;
    uint32 dwBuildNumber;

    ItemLoader() : m_otbLoaded(false), m_xmlLoaded(false) { }
    ~ItemLoader();

    bool loadOtb(const std::string &name);
    bool loadXML(const std::string &name);

    bool isLoaded() { return m_otbLoaded && m_xmlLoaded; }
    ItemDataPtr getType(uint16 id) const;
    void addType(uint16 id, ItemDataPtr type);

private:
    ItemDataList m_items;
    bool m_otbLoaded, m_xmlLoaded;
};

extern ItemLoader g_itemLoader;

#endif
