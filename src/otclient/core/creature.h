#ifndef CREATURE_H
#define CREATURE_H

#include "thing.h"

struct Outfit
{
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

    virtual ThingAttributes *getAttributes();
    void draw(int x, int y);

    void setName(const std::string& name) { m_name = name; }
    std::string getName() { return m_name; }

    void setHealthPercent(uint8 healthPercent) { m_healthPercent = healthPercent; }
    uint8 getHealthPercent() { return m_healthPercent; }

    void setDirection(Direction direction) { m_direction = direction; }
    Direction getDirection() { return m_direction; }

    void setOutfit(const Outfit& outfit) { m_outfit = outfit; }
    Outfit getOutfit() { return m_outfit; }

    virtual Creature *getCreature() { return this; }
    virtual const Creature *getCreature() const { return this; }

private:
    std::string m_name;
    uint8 m_healthPercent;
    Direction m_direction;
    Outfit m_outfit;

};

#endif // CREATURE_H
