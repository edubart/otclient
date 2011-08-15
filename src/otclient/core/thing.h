#ifndef THING_H
#define THING_H

#include "declarations.h"
#include "thingattributes.h"
#include <framework/luascript/luaobject.h>

class Thing : public LuaObject
{
public:
    Thing(ThingType type) : m_id(0), m_type(type) { }
    virtual ~Thing() { }

    virtual void draw(int x, int y) = 0;

    void setId(uint32 id) { m_id = id; }
    void setPosition(const Position& position) { m_position = position; }

    uint32 getId() const { return m_id; }
    ThingType getType() const { return m_type; }
    Position getPosition() const { return m_position; }
    virtual const ThingAttributes& getAttributes() = 0;

    ThingPtr asThing() { return std::static_pointer_cast<Thing>(shared_from_this()); }
    virtual ItemPtr asItem() { return nullptr; }
    virtual CreaturePtr asCreature() { return nullptr; }
    virtual EffectPtr asEffect() { return nullptr; }
    virtual PlayerPtr asPlayer() { return nullptr; }
    virtual LocalPlayerPtr asLocalPlayer() { return nullptr; }

protected:
    void internalDraw(int x, int y, int blendframes, int xdiv, int ydiv, int zdiv, int anim);

    uint32 m_id;
    ThingType m_type;
    Position m_position;
};

#endif
