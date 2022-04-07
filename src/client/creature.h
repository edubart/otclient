/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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

#include "mapview.h"
#include "outfit.h"
#include "thing.h"
#include <framework/core/declarations.h>
#include <framework/core/timer.h>
#include <framework/graphics/cachedtext.h>

// @bindclass
class Creature : public Thing
{
public:

    static double speedA, speedB, speedC;

    Creature();

    static bool hasSpeedFormula() { return speedA != 0 && speedB != 0 && speedC != 0; }

    void draw(const Point& dest, float scaleFactor, bool animate, const Highlight& highLight, TextureType textureType, Color color, LightView* lightView = nullptr) override;

    void internalDrawOutfit(Point dest, float scaleFactor, bool animateWalk, TextureType textureType, Otc::Direction direction, Color color);

    void drawOutfit(const Rect& destRect, bool resize, Color color = Color::white);
    void drawInformation(const Rect& parentRect, const Point& dest, float scaleFactor, const Point& drawOffset, bool useGray, float horizontalStretchFactor, float verticalStretchFactor, int drawFlags);

    void setId(uint32 id) override { m_id = id; }
    void setName(const std::string& name);
    void setHealthPercent(uint8 healthPercent);
    void setDirection(Otc::Direction direction);
    void setOutfit(const Outfit& outfit);
    void setOutfitColor(const Color& color, int duration);
    void setLight(const Light& light) { m_light = light; }
    void setSpeed(uint16 speed);
    void setBaseSpeed(double baseSpeed);
    void setSkull(uint8 skull);
    void setShield(uint8 shield);
    void setEmblem(uint8 emblem);
    void setType(uint8 type);
    void setIcon(uint8 icon);
    void setSkullTexture(const std::string& filename);
    void setShieldTexture(const std::string& filename, bool blink);
    void setEmblemTexture(const std::string& filename);
    void setTypeTexture(const std::string& filename);
    void setIconTexture(const std::string& filename);
    void setPassable(bool passable) { m_passable = passable; }
    void setOutfitShader(const PainterShaderProgramPtr& shader) { m_outfitShader = shader; }
    void setMountShader(const PainterShaderProgramPtr& shader) { m_mountShader = shader; }

    void addTimedSquare(uint8 color);
    void removeTimedSquare() { m_showTimedSquare = false; }
    void showStaticSquare(const Color& color) { m_showStaticSquare = true; m_staticSquareColor = color; }
    void hideStaticSquare() { m_showStaticSquare = false; }

    bool isDrawingOutfitColor() { return m_drawOutfitColor; }
    void setDrawOutfitColor(const bool draw) { m_drawOutfitColor = draw; }

    uint32 getId() override { return m_id; }
    std::string getName() { return m_name; }
    uint8 getHealthPercent() { return m_healthPercent; }
    Otc::Direction getDirection() { return m_direction; }
    Outfit getOutfit() { return m_outfit; }
    Light getLight() override;
    bool hasLight() override { return Thing::hasLight() || getLight().intensity > 0; }
    uint16 getSpeed() { return m_speed; }
    double getBaseSpeed() { return m_baseSpeed; }
    uint8 getSkull() { return m_skull; }
    uint8 getShield() { return m_shield; }
    uint8 getEmblem() { return m_emblem; }
    uint8 getType() { return m_type; }
    uint8 getIcon() { return m_icon; }
    bool isPassable() { return m_passable; }
    uint64 getStepDuration(bool ignoreDiagonal = false, Otc::Direction dir = Otc::InvalidDirection);
    Point getDrawOffset();
    Point getWalkOffset() { return m_walkOffset; }
    PointF getJumpOffset() { return m_jumpOffset; }
    Position getLastStepFromPosition() { return m_lastStepFromPosition; }
    Position getLastStepToPosition() { return m_lastStepToPosition; }
    float getStepProgress() { return m_walkTimer.ticksElapsed() / m_stepCache.duration; }
    float getStepTicksLeft() { return static_cast<float>(m_stepCache.getDuration(m_lastStepDirection)) - m_walkTimer.ticksElapsed(); }
    ticks_t getWalkTicksElapsed() { return m_walkTimer.ticksElapsed(); }
    std::array<double, Otc::LastSpeedFormula> getSpeedFormulaArray() { return m_speedFormula; }
    Point getDisplacement() override;
    int getDisplacementX() override;
    int getDisplacementY() override;
    int getExactSize() override;

    int getTotalAnimationPhase();
    int getCurrentAnimationPhase(bool mount = false);

    void updateShield();

    // walk related
    void turn(Otc::Direction direction);
    void jump(int height, int duration);
    void allowAppearWalk() { m_allowAppearWalk = true; }
    virtual void walk(const Position& oldPos, const Position& newPos);
    virtual void stopWalk();

    bool isWalking() { return m_walking; }
    bool isRemoved() { return m_removed; }
    bool isInvisible() { return m_outfit.getCategory() == ThingCategoryEffect && m_outfit.getAuxId() == 13; }
    bool isDead() { return m_healthPercent <= 0; }
    bool isFullHealth() { return m_healthPercent == 100; }
    bool canBeSeen() { return !isInvisible() || isPlayer(); }
    bool isCreature() override { return true; }
    bool isParalyzed() const { return m_speed < 10; }

    const ThingTypePtr& getThingType() override;
    ThingType* rawGetThingType() override;
    ThingType* rawGetMountThingType();

    void onPositionChange(const Position& newPos, const Position& oldPos) override;
    void onAppear() override;
    void onDisappear() override;
    virtual void onDeath();

    int getWalkedPixel() const { return m_walkedPixels; }

protected:
    void updateWalkingTile();
    virtual void updateWalkAnimation();
    virtual void updateWalkOffset(int totalPixelsWalked);
    virtual void updateWalk(bool isPreWalking = false);
    virtual void nextWalkUpdate();
    virtual void terminateWalk();

    void updateOutfitColor(Color color, Color finalColor, Color delta, int duration);
    void updateJump();

    uint32 m_id{ 0 };
    std::string m_name;
    Otc::Direction m_direction{ Otc::South };
    Outfit m_outfit;
    Light m_light;

    int m_speed{ 0 };
    int m_calculatedStepSpeed{ 0 };

    double m_baseSpeed;
    uint8 m_healthPercent{ 101 };
    uint8 m_skull{ Otc::SkullNone };
    uint8 m_shield{ Otc::ShieldNone };
    uint8 m_emblem{ Otc::EmblemNone };
    uint8 m_type;
    uint8 m_icon{ Otc::NpcIconNone };
    TexturePtr m_skullTexture;
    TexturePtr m_shieldTexture;
    TexturePtr m_emblemTexture;
    TexturePtr m_typeTexture;
    TexturePtr m_iconTexture;

    bool m_showShieldTexture{ true },
        m_shieldBlink{ false },
        m_passable{ false },
        m_showTimedSquare{ false },
        m_showStaticSquare{ false },
        m_forceWalk{ false },
        m_removed{ true };

    Color m_timedSquareColor;
    Color m_staticSquareColor;
    Color m_informationColor;
    Color m_outfitColor{ Color::white };
    CachedText m_nameCache;
    ScheduledEventPtr m_outfitColorUpdateEvent;
    Timer m_outfitColorTimer;

    std::array<double, Otc::LastSpeedFormula> m_speedFormula;

    // walk related
    int m_walkAnimationPhase{ 0 };
    int m_walkedPixels{ 0 };
    uint m_footStep{ 0 };
    Timer m_walkTimer;
    Timer m_footTimer;
    TilePtr m_walkingTile;
    bool m_walking{ false },
        m_allowAppearWalk{ false };
    ScheduledEventPtr m_walkUpdateEvent;
    ScheduledEventPtr m_walkFinishAnimEvent;
    EventPtr m_disappearEvent;
    Point m_walkOffset;
    Otc::Direction m_walkTurnDirection{ Otc::InvalidDirection };
    Otc::Direction m_lastStepDirection{ Otc::InvalidDirection };
    Position m_lastStepFromPosition;
    Position m_lastStepToPosition;
    Position m_oldPosition;

    // jump related
    float m_jumpHeight;
    float m_jumpDuration;
    PointF m_jumpOffset;
    Timer m_jumpTimer;

private:
    struct DrawCache
    {
        int exactSize, frameSizeNotResized;
    };

    struct StepCache
    {
        uint16 speed = 0;
        uint16 groundSpeed = 0;
        uint64 duration = 0;
        uint64 diagonalDuration = 0;

        uint64 getDuration(Otc::Direction dir) { return Position::isDiagonal(dir) ? diagonalDuration : duration; }
    };

    StepCache m_stepCache;
    DrawCache m_drawCache;

    bool m_drawOutfitColor{ true };
    PainterShaderProgramPtr m_outfitShader, m_mountShader;
};

// @bindclass
class Npc : public Creature
{
public:
    bool isNpc() override { return true; }
};

// @bindclass
class Monster : public Creature
{
public:
    bool isMonster() override { return true; }
};

#endif
