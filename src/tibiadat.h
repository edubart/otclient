#ifndef TIBIADAT_H
#define TIBIADAT_H

#include <global.h>

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

struct Item
{
    Item() {
        group = ITEM_GROUP_NONE;
        blockSolid = false;
        hasHeight = false;
        blockPathFind = false;
        blockProjectile = false;
        alwaysOnTop = false;
        alwaysOnTopOrder = 0;
        stackable = false;
        useable = false;
        moveable = true;
        pickupable = false;
        rotable = false;
        readable = false;
        lookThrough = false;
        speed = 0;
        lightLevel = 0;
        lightColor = 0;
        isVertical = false;
        isHorizontal = false;
        isHangable = false;
        miniMapColor = 0;
        hasMiniMapColor = false;
        subParam07 = 0;
        subParam08 = 0;
        sprites = NULL;
    }
    ~Item() {
        if(sprites)
            delete []sprites;
    }

    bool stackable, alwaysOnTop, useable, readable, moveable, blockSolid, blockProjectile, blockPathFind, pickupable,
         isHangable, isHorizontal, isVertical, rotable, hasHeight, lookThrough, hasMiniMapColor;
    uint8 alwaysOnTopOrder, width, height, blendframes, xdiv, ydiv, zdiv, animcount;
    uint16 id, speed, subParam07, subParam08, lightLevel, lightColor, uheight, miniMapColor;
    uint16 *sprites;
    ItemGroup group;
};

class TibiaDat
{
public:
    bool load(const std::string& filename);

    Item *getItem(uint16 id);

    uint16 getGroupCount(int i) { return m_groupCount[i]; }

    uint32 getSignature() { return m_signature; }
    uint16 getTotalCount() { return m_totalCount; }

private:
    uint32 m_signature, m_totalCount;
    uint16 m_groupCount[4];

    Item **m_items;
};

extern TibiaDat g_tibiaDat;

#endif // TIBIADAT_H
