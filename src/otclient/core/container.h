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

#ifndef CONTAINER_H
#define CONTAINER_H

#include "declarations.h"

#include <framework/luascript/luaobject.h>

class Container : public LuaObject
{
public:
    Container();

    void open(const ContainerPtr& previousContainer);
    void close();

    void addItem(const ItemPtr& item);
    void addItems(const std::vector<ItemPtr>& items);
    void updateItem(int slot, const ItemPtr& item);
    void removeItem(int slot);
    ItemPtr getItem(int slot);

    Position getSlotPosition(int slot) { return Position(0xffff, m_id | 0x40, slot); }

    void setId(int id) { m_id = id; }
    void setCapacity(int capacity) { m_capacity = capacity; }
    void setName(std::string name) { m_name = name; }
    void setItemId(uint16 itemId) { m_itemId = itemId; }
    void setHasParent(bool hasParent) { m_hasParent = hasParent; }

    std::string getName() { return m_name; }
    int getId() { return m_id; }
    int getCapacity() { return m_capacity; }
    int getItemsCount() { return m_items.size(); }
    uint16 getItemId() { return m_itemId; }
    bool hasParent() { return m_hasParent; }

private:
    void updateItemsPositions();

    int m_id;
    int m_capacity;
    uint16 m_itemId;
    std::string m_name;
    bool m_hasParent;
    std::deque<ItemPtr> m_items;
};

#endif
