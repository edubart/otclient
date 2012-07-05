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
#include "mapview.h"
#include <framework/core/declarations.h>
#include <framework/core/timer.h>
#include <framework/graphics/fontmanager.h>
#include <framework/graphics/cachedtext.h>

class Creature : public Thing
{
public:
    enum {
        SHIELD_BLINK_TICKS = 500,
        VOLATILE_SQUARE_DURATION = 1000
    };

    Creature();

    virtual void draw(const Point& dest, float scaleFactor, bool animate);
    virtual void drawLight(const Point& dest, float scaleFactor, bool animate, MapView* mapview);

    void internalDrawOutfit(const Point& dest, float scaleFactor, bool animateWalk, bool animateIdle, Otc::Direction direction);
    void drawOutfit(const Rect& destRect, bool resize);
    void drawInformation(const Point& point, bool useGray, const Rect& parentRect);


    void setId(uint32 id) { m_id = id; }
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
    void setRemoved(bool removed) { m_removed = removed; }

    void addTimedSquare(uint8 color);
    void removeTimedSquare() { m_showTimedSquare = false; }

    void showStaticSquare(const Color& color) { m_showStaticSquare = true; m_staticSquareColor = color; }
    void hideStaticSquare() { m_showStaticSquare = false; }

    uint32 getId() { return m_id; }
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
    Point getDrawOffset();
    Point getWalkOffset() { return m_walkOffset; }

    void updateShield();

    // walk related
    void turn(Otc::Direction direction);
    virtual void walk(const Position& oldPos, const Position& newPos);
    virtual void stopWalk();

    bool isWalking() { return m_walking; }
    bool isRemoved() { return m_removed; }

    CreaturePtr asCreature() { return std::static_pointer_cast<Creature>(shared_from_this()); }
    bool isCreature() { return true; }

protected:
    virtual void updateWalkAnimation(int totalPixelsWalked);
    virtual void updateWalkOffset(int totalPixelsWalked);
    void updateWalkingTile();
    virtual void nextWalkUpdate();
    virtual void updateWalk();
    virtual void terminateWalk();

    uint32 m_id;
    std::string m_name;
    uint8 m_healthPercent;
    Otc::Direction m_direction;
    Outfit m_outfit;
    Light m_light;
    int m_speed;
    uint8 m_skull;
    uint8 m_shield;
    uint8 m_emblem;
    TexturePtr m_skullTexture;
    TexturePtr m_shieldTexture;
    TexturePtr m_emblemTexture;
    Boolean<true> m_showShieldTexture;
    Boolean<false> m_shieldBlink;
    Boolean<false> m_passable;
    Color m_timedSquareColor;
    Color m_staticSquareColor;
    Boolean<false> m_showTimedSquare;
    Boolean<false> m_showStaticSquare;
    Boolean<false> m_removed;
    CachedText m_nameCache;
    Color m_informationColor;

    // walk related
    int m_walkAnimationPhase;
    uint m_footStep;
    Timer m_walkTimer;
    Timer m_footTimer;
    TilePtr m_walkingTile;
    int m_walkInterval;
    int m_walkAnimationInterval;
    Boolean<false> m_walking;
    Boolean<false> m_footStepDrawn;
    ScheduledEventPtr m_walkUpdateEvent;
    Point m_walkOffset;
    Otc::Direction m_walkTurnDirection;
};

class Npc : public Creature {
public:
    NpcPtr asNpc() { return std::static_pointer_cast<Npc>(shared_from_this()); }
    bool isNpc() { return true; }
};

class Monster : public Creature {
public:
    MonsterPtr asMonster() { return std::static_pointer_cast<Monster>(shared_from_this()); }
    bool isMonster() { return true; }
};

#endif
