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
    void drawInformation(int x, int y, bool useGray);

    void setName(const std::string& name) { m_name = name; }
    void setHealthPercent(uint8 healthPercent) { m_healthPercent = healthPercent; }
    void setDirection(Direction direction) { m_direction = direction; }
    void setOutfit(const Outfit& outfit) { m_outfit = outfit; }
    void setLight(const Light& light) { m_light = light; }
    void setSpeed(uint16 speed) { m_speed = speed; }
    void setSkull(uint8 skull) { m_skull = skull; }
    void setShield(uint8 shield) { m_shield = shield; }
    void setEmblem(uint8 emblem) { m_emblem = emblem; }
    void setImpassable(bool impassable) { m_impassable = impassable; }

    std::string getName() { return m_name; }
    uint8 getHealthPercent() { return m_healthPercent; }
    Direction getDirection() { return m_direction; }
    Outfit getOutfit() { return m_outfit; }
    Light getLight() { return m_light; }
    uint16 getSpeed() { return m_speed; }
    uint8 getSkull() { return m_skull; }
    uint8 getShield() { return m_shield; }
    uint8 getEmblem() { return m_emblem; }
    bool getImpassable() { return m_impassable; }

    void walk(const Position& position);

    const ThingAttributes& getAttributes();

    CreaturePtr asCreature() { return std::static_pointer_cast<Creature>(shared_from_this()); }

private:
    std::string m_name;
    uint8 m_healthPercent;
    Direction m_direction;
    Outfit m_outfit;
    Light m_light;
    uint16 m_speed;
    uint8 m_skull;
    uint8 m_shield;
    uint8 m_emblem;
    bool m_impassable;

    int m_lastTicks;
    bool m_walking;
    Position m_walkingPosition;
    float m_walkOffsetX, m_walkOffsetY;
    int m_animation;
};

#endif
