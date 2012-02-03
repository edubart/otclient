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

class Item : public Thing
{
public:
    Item();

    static ItemPtr create(int id);

    void draw(const Point& dest, float scaleFactor, bool animate);

    void setId(uint32 id);
    void setCountOrSubType(uint8 value) { m_countOrSubType = value; }
    void setCount(int count) { setCountOrSubType(count); }
    void setSubType(int subType) { setCountOrSubType(subType); }

    uint8 getCountOrSubType() { return m_countOrSubType; }
    int getSubType();
    int getCount();
    uint32 getId() { return m_id; }

    ItemPtr asItem() { return std::static_pointer_cast<Item>(shared_from_this()); }

private:
    uint16 m_id;
    uint8 m_countOrSubType;
};

#endif
