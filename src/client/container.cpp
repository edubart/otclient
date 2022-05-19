/*
 * Copyright (c) 2010-2022 OTClient <https://github.com/edubart/otclient>
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

#include "container.h"
#include "item.h"

ItemPtr Container::getItem(int slot)
{
    if (slot < 0 || slot >= static_cast<int>(m_items.size()))
        return nullptr;
    return m_items[slot];
}

void Container::onOpen(const ContainerPtr& previousContainer)
{
    callLuaField("onOpen", previousContainer);
}

void Container::onClose()
{
    m_closed = true;
    callLuaField("onClose");
}

void Container::onAddItem(const ItemPtr& item, int slot)
{
    slot -= m_firstIndex;

    ++m_size;
    // indicates that there is a new item on next page
    if (m_hasPages && slot > m_capacity) {
        callLuaField("onSizeChange", m_size);
        return;
    }

    if (slot == 0)
        m_items.push_front(item);
    else
        m_items.push_back(item);
    updateItemsPositions();

    callLuaField("onSizeChange", m_size);
    callLuaField("onAddItem", slot, item);
}

ItemPtr Container::findItemById(uint itemId, int subType)
{
    for (const ItemPtr& item : m_items)
        if (item->getId() == itemId && (subType == -1 || item->getSubType() == subType))
            return item;
    return nullptr;
}

void Container::onAddItems(const std::vector<ItemPtr>& items)
{
    for (const ItemPtr& item : items)
        m_items.push_back(item);
    updateItemsPositions();
}

void Container::onUpdateItem(int slot, const ItemPtr& item)
{
    slot -= m_firstIndex;
    if (slot < 0 || slot >= static_cast<int>(m_items.size())) {
        g_logger.traceError("slot not found");
        return;
    }

    const ItemPtr oldItem = m_items[slot];
    m_items[slot] = item;
    item->setPosition(getSlotPosition(slot));

    callLuaField("onUpdateItem", slot, item, oldItem);
}

void Container::onRemoveItem(int slot, const ItemPtr& lastItem)
{
    slot -= m_firstIndex;
    if (m_hasPages && slot >= static_cast<int>(m_items.size())) {
        --m_size;
        callLuaField("onSizeChange", m_size);
        return;
    }

    if (slot < 0 || slot >= static_cast<int>(m_items.size())) {
        g_logger.traceError("slot not found");
        return;
    }

    const ItemPtr item = m_items[slot];
    m_items.erase(m_items.begin() + slot);

    if (lastItem) {
        onAddItem(lastItem, m_firstIndex + m_capacity - 1);
        --m_size;
    }

    --m_size;

    updateItemsPositions();

    callLuaField("onSizeChange", m_size);
    callLuaField("onRemoveItem", slot, item);
}

void Container::updateItemsPositions()
{
    for (int slot = 0; slot < static_cast<int>(m_items.size()); ++slot)
        m_items[slot]->setPosition(getSlotPosition(slot));
}
