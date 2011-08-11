#ifndef THING_H
#define THING_H

#include <global.h>

class Item;

class Thing
{
public:
    Thing();

    enum Type {
        TYPE_NONE,
        TYPE_ITEM,
        TYPE_CREATURE
    };

    void setType(Type type) { m_type = type; }
    Type getType() const { return m_type; }

    virtual void draw(int, int) {}

    virtual Item* getItem() { return NULL; }
    virtual const Item *getItem() const { return NULL; }

protected:
    Type m_type;
};

#endif // THING_H
