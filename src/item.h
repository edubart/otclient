#ifndef ITEM_H
#define ITEM_H

#include <global.h>
#include "thing.h"

enum ItemGroup {
    ITEM_GROUP_NONE = 0,
    ITEM_GROUP_GROUND,
    ITEM_GROUP_CONTAINER,
    ITEM_GROUP_WEAPON,
    ITEM_GROUP_AMMUNITION,
    ITEM_GROUP_ARMOR,
    ITEM_GROUP_RUNE,
    ITEM_GROUP_TELEPORT,
    ITEM_GROUP_MAGICFIELD,
    ITEM_GROUP_WRITEABLE,
    ITEM_GROUP_KEY,
    ITEM_GROUP_SPLASH,
    ITEM_GROUP_FLUID,
    ITEM_GROUP_DOOR,
    ITEM_GROUP_LAST
};

class ItemAttributes
{
public:
    ItemAttributes();
    ~ItemAttributes();

    bool stackable, alwaysOnTop, useable, readable, moveable, blockSolid, blockProjectile, blockPathFind, pickupable,
         isHangable, isHorizontal, isVertical, rotable, hasHeight, lookThrough, hasMiniMapColor;
    uint8 alwaysOnTopOrder, width, height, blendframes, xdiv, ydiv, zdiv, animcount;
    uint16 id, speed, subParam07, subParam08, lightLevel, lightColor, uheight, miniMapColor;
    uint16 *sprites;
    ItemGroup group;
};

class Item : virtual public Thing
{
public:
    Item();

    void draw(int x, int y);

    void setId(uint16 id) { m_id = id; }
    uint16 getId() const { return m_id; }

    void setCount(uint8 count) { m_count = count; }

    virtual Item* getItem() { return this; }
    virtual const Item* getItem() const { return this; }

private:
    uint16 m_id;
    uint8 m_count;
};

#endif
