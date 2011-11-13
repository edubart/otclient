/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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
#include <framework/graphics/fontmanager.h>

class Creature : public Thing
{
public:
    Creature();
    virtual ~Creature() { }

    virtual void draw(const Point& p);
    void drawInformation(int x, int y, bool useGray, const Rect& rect);

    void setName(const std::string& name);
    void setHealthPercent(uint8 healthPercent);
    void setDirection(Otc::Direction direction);
    void setOutfit(const Outfit& outfit) { m_outfit = outfit; }
    void setLight(const Light& light) { m_light = light; }
    void setSpeed(uint16 speed) { m_speed = speed; }
    void setSkull(uint8 skull) { m_skull = skull; }
    void setShield(uint8 shield) { m_shield = shield; }
    void setEmblem(uint8 emblem) { m_emblem = emblem; }
    void setImpassable(bool impassable) { m_impassable = impassable; }
    void setSquareColor(uint8 squareColor) { m_squareColor = squareColor; }

    std::string getName() { return m_name; }
    uint8 getHealthPercent() { return m_healthPercent; }
    Otc::Direction getDirection() { return m_direction; }
    Outfit getOutfit() { return m_outfit; }
    Light getLight() { return m_light; }
    uint16 getSpeed() { return m_speed; }
    uint8 getSkull() { return m_skull; }
    uint8 getShield() { return m_shield; }
    uint8 getEmblem() { return m_emblem; }
    bool getImpassable() { return m_impassable; }
    const ThingType& getType();

    virtual void walk(const Position& position, bool inverse = true);
    virtual void cancelWalk(Otc::Direction direction);
    Point getWalkOffset() { return m_walkOffset; }

    bool isWalking() { return m_walking; }

    CreaturePtr asCreature() { return std::static_pointer_cast<Creature>(shared_from_this()); }

protected:
    void updateWalk();

    std::string m_name;
    Size m_nameSize;
    uint8 m_healthPercent;
    Otc::Direction m_direction;
    Outfit m_outfit;
    Light m_light;
    uint16 m_speed;
    uint8 m_skull;
    uint8 m_shield;
    uint8 m_emblem;
    bool m_impassable;
    uint8 m_squareColor;
    bool m_showSquareColor;

    FontPtr m_informationFont;
    Color m_informationColor;

    int m_walkStartTicks;
    bool m_walking, m_inverseWalking;
    float m_walkTimePerPixel;
    Position m_walkingFromPosition;
    Point m_walkOffset;
};

#endif
