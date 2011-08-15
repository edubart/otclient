#ifndef CREATURE_H
#define CREATURE_H

#include "thing.h"

struct Outfit {
    uint16 type;
    uint8 head;
    uint8 body;
    uint8 legs;
    uint8 feet;
    uint8 addons;
};

class Creature : public Thing
{
public:
    Creature();
    virtual ~Creature() { }

    virtual void draw(int x, int y);

    void setName(const std::string& name) { m_name = name; }
    void setHealthPercent(uint8 healthPercent) { m_healthPercent = healthPercent; }
    void setDirection(Direction direction) { m_direction = direction; }
    void setOutfit(const Outfit& outfit) { m_outfit = outfit; }
    Direction getDirection() { return m_direction; }

    Outfit getOutfit() { return m_outfit; }
    std::string getName() { return m_name; }
    uint8 getHealthPercent() { return m_healthPercent; }
    const ThingAttributes& getAttributes();

    CreaturePtr asCreature() { return std::static_pointer_cast<Creature>(shared_from_this()); }

private:
    std::string m_name;
    uint8 m_healthPercent;
    Direction m_direction;
    Outfit m_outfit;
};

#endif
