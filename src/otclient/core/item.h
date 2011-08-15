#ifndef ITEM_H
#define ITEM_H

#include <framework/global.h>
#include "thing.h"

class Item : public Thing
{
public:
    Item();

    virtual ThingAttributes *getAttributes();
    void draw(int x, int y);

    void setCount(uint8 count) { m_count = count; }

    virtual Item* getItem() { return this; }
    virtual const Item* getItem() const { return this; }

private:
    uint8 m_count;
};

#endif // ITEM_H
