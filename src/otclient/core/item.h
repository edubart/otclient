#ifndef ITEM_H
#define ITEM_H

#include <framework/global.h>
#include "thing.h"

class Item : public Thing
{
public:
    Item();

    void draw(int x, int y);

    void setCount(int count) { m_count = count; }

    int getCount() { return m_count; }
    const ThingAttributes& getAttributes();

    ItemPtr asItem() { return std::static_pointer_cast<Item>(shared_from_this()); }

private:
    int m_count;
};

#endif
