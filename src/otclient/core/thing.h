#ifndef THING_H
#define THING_H

#include <otclient/global.h>
#include "declarations.h"

struct ThingAttributes
{
    ThingAttributes();

    bool stackable, alwaysOnTop, useable, readable, moveable, blockSolid, blockProjectile, blockPathFind, pickupable,
         isHangable, isHorizontal, isVertical, rotable, hasHeight, lookThrough, hasMiniMapColor;
    uint8 alwaysOnTopOrder, width, height, blendframes, xdiv, ydiv, zdiv, animcount, xOffset, yOffset;
    uint16 speed, subParam07, subParam08, lightLevel, lightColor, miniMapColor;
    std::vector<int> sprites;
    ThingAttributesGroup group;
};

class Thing
{
public:
    Thing();

    enum Type {
        TYPE_NONE,
        TYPE_ITEM,
        TYPE_CREATURE,
        TYPE_EFFECT,
        TYPE_SHOT
    };

    void setId(uint32 id) { m_id = id; }
    uint32 getId() { return m_id; }

    void setType(Type type) { m_type = type; }
    Type getType() const { return m_type; }

    void setPosition(const Position& position) { m_position = position; }
    Position *getPosition() { return &m_position; }

    virtual void draw(int, int) {}
    virtual const ThingAttributes& getAttributes() = 0;

    virtual Item* getItem() { return NULL; }
    virtual const Item *getItem() const { return NULL; }
    virtual Creature *getCreature() { return NULL; }
    virtual const Creature *getCreature() const { return NULL; }

protected:
    void internalDraw(int x, int y, int blendframes, int xdiv, int ydiv, int zdiv, int anim);

    uint32 m_id;
    Type m_type;
    Position m_position;

};

#endif
