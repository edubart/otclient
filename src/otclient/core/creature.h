/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef CREATURE_H
#define CREATURE_H

#include "thing.h"
#include "outfit.h"
#include <framework/core/declarations.h>
#include <framework/core/timer.h>
#include <framework/graphics/fontmanager.h>

class Creature : public Thing
{
public:
    enum {
        SHIELD_BLINK_TICKS = 500,
        INVISIBLE_TICKS = 500,
        VOLATILE_SQUARE_DURATION = 1000
    };

    Creature();
    virtual ~Creature() { }

    virtual void draw(const Point& p, const Rect&);
    void drawInformation(int x, int y, bool useGray, const Rect& visibleRect);

    void setName(const std::string& name);
    void setHealthPercent(uint8 healthPercent);
    void setDirection(Otc::Direction direction);
    void setOutfit(const Outfit& outfit);
    void setLight(const Light& light) { m_light = light; }
    void setSpeed(uint16 speed) { m_speed = speed; }
    void setSkull(uint8 skull);
    void setShield(uint8 shield);
    void setEmblem(uint8 emblem);
    void setSkullTexture(const std::string& filename);
    void setShieldTexture(const std::string& filename, bool blink);
    void setEmblemTexture(const std::string& filename);
    void setPassable(bool passable) { m_passable = passable; }

    void addVolatileSquare(uint8 color);
    void removeVolatileSquare() { m_showVolatileSquare = false; }

    void showStaticSquare(const Color& color) { m_showStaticSquare = true; m_staticSquareColor = color; }
    void hideStaticSquare() { m_showStaticSquare = false; }

    std::string getName() { return m_name; }
    uint8 getHealthPercent() { return m_healthPercent; }
    Otc::Direction getDirection() { return m_direction; }
    Outfit getOutfit() { return m_outfit; }
    Light getLight() { return m_light; }
    uint16 getSpeed() { return m_speed; }
    uint8 getSkull() { return m_skull; }
    uint8 getShield() { return m_shield; }
    uint8 getEmblem() { return m_emblem; }
    bool getPassable() { return m_passable; }

    void updateInvisibleAnimation();
    void updateShield();

    ThingType *getType();

    // walk related
    void turn(Otc::Direction direction);
    virtual void walk(const Position& oldPos, const Position& newPos);
    virtual void stopWalk();
    Point getWalkOffset() { return m_walkOffset; }

    bool isWalking() { return m_walking; }

    CreaturePtr asCreature() { return std::static_pointer_cast<Creature>(shared_from_this()); }

protected:
    virtual void updateWalkAnimation(int totalPixelsWalked);
    virtual void updateWalkOffset(int totalPixelsWalked);
    virtual void nextWalkUpdate();
    virtual void updateWalk();
    virtual void terminateWalk();

    std::string m_name;
    Size m_nameSize;
    uint8 m_healthPercent;
    Otc::Direction m_direction;
    Outfit m_outfit;
    Light m_light;
    int m_speed;
    uint8 m_skull, m_shield, m_emblem;
    TexturePtr m_skullTexture, m_shieldTexture, m_emblemTexture;
    bool m_showShieldTexture, m_shieldBlink;
    bool m_passable;
    Color m_volatileSquareColor, m_staticSquareColor;
    bool m_showVolatileSquare, m_showStaticSquare;

    FontPtr m_informationFont;
    Color m_informationColor;

    // walk related
    Timer m_walkTimer;
    int m_walkInterval;
    int m_walkAnimationInterval;
    bool m_walking;
    ScheduledEventPtr m_walkUpdateEvent;
    Point m_walkOffset;
    Otc::Direction m_walkTurnDirection;
};

class Npc : public Creature {
public:
    NpcPtr asNpc() { return std::static_pointer_cast<Npc>(shared_from_this()); }
};

class Monster : public Creature {
public:
    MonsterPtr asMonster() { return std::static_pointer_cast<Monster>(shared_from_this()); }
};

#endif
