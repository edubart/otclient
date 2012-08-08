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

#include "creature.h"
#include "thingtypemanager.h"
#include "localplayer.h"
#include "map.h"
#include "tile.h"
#include "item.h"
#include "game.h"
#include "effect.h"
#include "luavaluecasts.h"

#include <framework/graphics/graphics.h>
#include <framework/core/eventdispatcher.h>
#include <framework/core/clock.h>

#include <framework/graphics/paintershaderprogram.h>
#include <framework/graphics/painterogl2_shadersources.h>
#include <framework/graphics/texturemanager.h>
#include <framework/graphics/framebuffermanager.h>
#include "spritemanager.h"

Creature::Creature() : Thing()
{
    m_id = 0;
    m_healthPercent = 100;
    m_speed = 200;
    m_direction = Otc::South;
    m_walkAnimationPhase = 0;
    m_walkedPixels = 0;
    m_walkTurnDirection = Otc::InvalidDirection;
    m_skull = Otc::SkullNone;
    m_shield = Otc::ShieldNone;
    m_emblem = Otc::EmblemNone;
    m_lastStepDirection = Otc::InvalidDirection;
    m_nameCache.setFont(g_fonts.getFont("verdana-11px-rounded"));
    m_nameCache.setAlign(Fw::AlignTopCenter);
    m_footStep = 0;
}

void Creature::draw(const Point& dest, float scaleFactor, bool animate)
{
    Point animationOffset = animate ? m_walkOffset : Point(0,0);

    if(m_showTimedSquare && animate) {
        g_painter->setColor(m_timedSquareColor);
        g_painter->drawBoundingRect(Rect(dest + (animationOffset - getDisplacement() + 2)*scaleFactor, Size(28, 28)*scaleFactor), std::max((int)(2*scaleFactor), 1));
        g_painter->setColor(Color::white);
    }

    if(m_showStaticSquare && animate) {
        g_painter->setColor(m_staticSquareColor);
        g_painter->drawBoundingRect(Rect(dest + (animationOffset - getDisplacement())*scaleFactor, Size(Otc::TILE_PIXELS, Otc::TILE_PIXELS)*scaleFactor), std::max((int)(2*scaleFactor), 1));
        g_painter->setColor(Color::white);
    }

    internalDrawOutfit(dest + animationOffset * scaleFactor, scaleFactor, animate, animate, m_direction);
    m_footStepDrawn = true;
}

void Creature::internalDrawOutfit(Point dest, float scaleFactor, bool animateWalk, bool animateIdle, Otc::Direction direction)
{
    // outfit is a real creature
    if(m_outfit.getCategory() == ThingCategoryCreature) {
        int animationPhase = animateWalk ? m_walkAnimationPhase : 0;

        if(isAnimateAlways() && animateIdle) {
            int ticksPerFrame = 1000 / getAnimationPhases();
            animationPhase = (g_clock.millis() % (ticksPerFrame * getAnimationPhases())) / ticksPerFrame;
        }

        // xPattern => creature direction
        int xPattern;
        if(direction == Otc::NorthEast || direction == Otc::SouthEast)
            xPattern = Otc::East;
        else if(direction == Otc::NorthWest || direction == Otc::SouthWest)
            xPattern = Otc::West;
        else
            xPattern = direction;

        int zPattern = 0;
        if(m_outfit.getMount() != 0) {
            auto datType = g_things.rawGetThingType(m_outfit.getMount(), ThingCategoryCreature);
            dest -= datType->getDisplacement() * scaleFactor;
            datType->draw(dest, scaleFactor, 0, xPattern, 0, 0, animationPhase);
            dest += getDisplacement() * scaleFactor;
            zPattern = 1;
        }

        // yPattern => creature addon
        for(int yPattern = 0; yPattern < getNumPatternY(); yPattern++) {

            // continue if we dont have this addon
            if(yPattern > 0 && !(m_outfit.getAddons() & (1 << (yPattern-1))))
                continue;

            auto datType = rawGetThingType();
            datType->draw(dest, scaleFactor, 0, xPattern, yPattern, zPattern, animationPhase);

            if(getLayers() > 1) {
                Color oldColor = g_painter->getColor();
                Painter::CompositionMode oldComposition = g_painter->getCompositionMode();
                g_painter->setCompositionMode(Painter::CompositionMode_Multiply);
                g_painter->setColor(m_outfit.getHeadColor());
                datType->draw(dest, scaleFactor, SpriteMaskYellow, xPattern, yPattern, zPattern, animationPhase);
                g_painter->setColor(m_outfit.getBodyColor());
                datType->draw(dest, scaleFactor, SpriteMaskRed, xPattern, yPattern, zPattern, animationPhase);
                g_painter->setColor(m_outfit.getLegsColor());
                datType->draw(dest, scaleFactor, SpriteMaskGreen, xPattern, yPattern, zPattern, animationPhase);
                g_painter->setColor(m_outfit.getFeetColor());
                datType->draw(dest, scaleFactor, SpriteMaskBlue, xPattern, yPattern, zPattern, animationPhase);
                g_painter->setColor(oldColor);
                g_painter->setCompositionMode(oldComposition);
            }
        }
    // outfit is a creature imitating an item or the invisible effect
    } else  {
        int animationPhase = 0;
        int animationPhases = getAnimationPhases();
        int animateTicks = Otc::ITEM_TICKS_PER_FRAME;

        // when creature is an effect we cant render the first and last animation phase,
        // instead we should loop in the phases between
        if(m_outfit.getCategory() == ThingCategoryEffect) {
            animationPhases = std::max(1, animationPhases-2);
            animateTicks = Otc::INVISIBLE_TICKS_PER_FRAME;
        }

        if(animationPhases > 1) {
            if(animateIdle)
                animationPhase = (g_clock.millis() % (animateTicks * animationPhases)) / animateTicks;
            else
                animationPhase = animationPhases-1;
        }

        if(m_outfit.getCategory() == ThingCategoryEffect)
            animationPhase = std::min(animationPhase+1, getAnimationPhases());

        rawGetThingType()->draw(dest, scaleFactor, 0, 0, 0, 0, animationPhase);
    }
}

void Creature::drawOutfit(const Rect& destRect, bool resize)
{
    if(g_graphics.canUseFBO()) {
        const FrameBufferPtr& outfitBuffer = g_framebuffers.getTemporaryFrameBuffer();
        outfitBuffer->resize(Size(2*Otc::TILE_PIXELS, 2*Otc::TILE_PIXELS));
        outfitBuffer->bind();
        g_painter->setAlphaWriting(true);
        g_painter->clear(Color::alpha);
        internalDrawOutfit(Point(Otc::TILE_PIXELS,Otc::TILE_PIXELS) + getDisplacement(), 1, false, true, Otc::South);
        outfitBuffer->release();

        Rect srcRect;
        if(resize)
            srcRect.resize(getExactSize(), getExactSize());
        else
            srcRect.resize(2*Otc::TILE_PIXELS*0.75f, 2*Otc::TILE_PIXELS*0.75f);
        srcRect.moveBottomRight(Point(2*Otc::TILE_PIXELS - 1, 2*Otc::TILE_PIXELS - 1));
        outfitBuffer->draw(destRect, srcRect);
    } else {
        float scaleFactor;
        if(resize)
            scaleFactor = destRect.width() / (float)getExactSize();
        else
            scaleFactor = destRect.width() / (float)(2*Otc::TILE_PIXELS*0.75f);
        Point dest = destRect.bottomRight() - (Point(Otc::TILE_PIXELS,Otc::TILE_PIXELS) - getDisplacement())*scaleFactor;
        internalDrawOutfit(dest, scaleFactor, false, true, Otc::South);
    }
}

void Creature::drawInformation(const Point& point, bool useGray, const Rect& parentRect)
{
    if(m_healthPercent < 1) // creature is dead
        return;

    Color fillColor = Color(96, 96, 96);

    if(!useGray)
        fillColor = m_informationColor;

    // calculate main rects
    Rect backgroundRect = Rect(point.x-(13.5), point.y, 27, 4);
    backgroundRect.bind(parentRect);

    Size nameSize = m_nameCache.getTextSize();
    Rect textRect = Rect(point.x - nameSize.width() / 2.0, point.y-12, nameSize);
    textRect.bind(parentRect);

    // distance them
    if(textRect.top() == parentRect.top())
        backgroundRect.moveTop(textRect.top() + 12);
    if(backgroundRect.bottom() == parentRect.bottom())
        textRect.moveTop(backgroundRect.top() - 12);

    // health rect is based on background rect, so no worries
    Rect healthRect = backgroundRect.expanded(-1);
    healthRect.setWidth((m_healthPercent / 100.0) * 25);

    // draw
    g_painter->setColor(Color::black);
    g_painter->drawFilledRect(backgroundRect);

    g_painter->setColor(fillColor);
    g_painter->drawFilledRect(healthRect);

    m_nameCache.draw(textRect);

    if(m_skull != Otc::SkullNone && m_skullTexture) {
        g_painter->setColor(Color::white);
        g_painter->drawTexturedRect(Rect(point.x + 12, point.y + 5, m_skullTexture->getSize()), m_skullTexture);
    }
    if(m_shield != Otc::ShieldNone && m_shieldTexture && m_showShieldTexture) {
        g_painter->setColor(Color::white);
        g_painter->drawTexturedRect(Rect(point.x, point.y + 5, m_shieldTexture->getSize()), m_shieldTexture);
    }
    if(m_emblem != Otc::EmblemNone && m_emblemTexture) {
        g_painter->setColor(Color::white);
        g_painter->drawTexturedRect(Rect(point.x + 12, point.y + 16, m_emblemTexture->getSize()), m_emblemTexture);
    }
}

void Creature::turn(Otc::Direction direction)
{
    // if is not walking change the direction right away
    if(!m_walking)
        setDirection(direction);
    // schedules to set the new direction when walk ends
    else
        m_walkTurnDirection = direction;
}

void Creature::walk(const Position& oldPos, const Position& newPos)
{
    if(oldPos == newPos)
        return;

    // get walk direction
    m_lastStepDirection = oldPos.getDirectionFromPosition(newPos);

    // set current walking direction
    setDirection(m_lastStepDirection);

    // starts counting walk
    m_walking = true;
    m_walkTimer.restart();
    m_walkedPixels = 0;

    // no direction need to be changed when the walk ends
    m_walkTurnDirection = Otc::InvalidDirection;

    // starts updating walk
    nextWalkUpdate();
}

void Creature::stopWalk()
{
    if(!m_walking)
        return;

    // reset walk animation states
    m_walkOffset = Point(0,0);
    m_walkAnimationPhase = 0;

    // stops the walk right away
    terminateWalk();
}

void Creature::onPositionChange(const Position& newPos, const Position& oldPos)
{
    callLuaField("onPositionChange", newPos, oldPos);
}

void Creature::onAppear()
{
    // cancel any disappear event
    if(m_disappearEvent) {
        m_disappearEvent->cancel();
        m_disappearEvent = nullptr;
    }

    // creature appeared the first time or wasn't seen for a long time
    if(m_removed) {
        stopWalk();
        m_removed = false;
        callLuaField("onAppear");
    // walk
    } else if(m_oldPosition != m_position && m_oldPosition.isInRange(m_position,1,1)) {
        walk(m_oldPosition, m_position);
        callLuaField("onWalk", m_oldPosition, m_position);
    // teleport
    } else if(m_oldPosition != m_position) {
        stopWalk();
        callLuaField("onDisappear");
        callLuaField("onAppear");
    } // else turn
}

void Creature::onDisappear()
{
    if(m_disappearEvent)
        m_disappearEvent->cancel();

    m_oldPosition = m_position;

    // a pair onDisappear and onAppear events are fired even when creatures walks or turns,
    // so we must filter
    auto self = static_self_cast<Creature>();
    m_disappearEvent = g_dispatcher.addEvent([self] {
        self->m_removed = true;
        self->stopWalk();

        self->callLuaField("onDisappear");

        // invalidate this creature position
        if(!self->isLocalPlayer())
            self->setPosition(Position());
        self->m_oldPosition = Position();
        self->m_disappearEvent = nullptr;
    });
}

void Creature::updateWalkAnimation(int totalPixelsWalked)
{
    // update outfit animation
    if(m_outfit.getCategory() != ThingCategoryCreature)
        return;

    int footAnimPhases = getAnimationPhases() - 1;
    if(totalPixelsWalked == 32 || footAnimPhases == 0)
        m_walkAnimationPhase = 0;
    else if(m_footStepDrawn && m_footTimer.ticksElapsed() >= getStepDuration() / 4 ) {
        m_footStep++;
        m_walkAnimationPhase = 1 + (m_footStep % footAnimPhases);
        m_footStepDrawn = false;
        m_footTimer.restart();
    }
}

void Creature::updateWalkOffset(int totalPixelsWalked)
{
    m_walkOffset = Point(0,0);
    if(m_direction == Otc::North || m_direction == Otc::NorthEast || m_direction == Otc::NorthWest)
        m_walkOffset.y = 32 - totalPixelsWalked;
    else if(m_direction == Otc::South || m_direction == Otc::SouthEast || m_direction == Otc::SouthWest)
        m_walkOffset.y = totalPixelsWalked - 32;

    if(m_direction == Otc::East || m_direction == Otc::NorthEast || m_direction == Otc::SouthEast)
        m_walkOffset.x = totalPixelsWalked - 32;
    else if(m_direction == Otc::West || m_direction == Otc::NorthWest || m_direction == Otc::SouthWest)
        m_walkOffset.x = 32 - totalPixelsWalked;
}

void Creature::updateWalkingTile()
{
    // determine new walking tile
    TilePtr newWalkingTile;
    Rect virtualCreatureRect(Otc::TILE_PIXELS + (m_walkOffset.x - getDisplacementX()),
                             Otc::TILE_PIXELS + (m_walkOffset.y - getDisplacementY()),
                             Otc::TILE_PIXELS, Otc::TILE_PIXELS);
    for(int xi = -1; xi <= 1 && !newWalkingTile; ++xi) {
        for(int yi = -1; yi <= 1 && !newWalkingTile; ++yi) {
            Rect virtualTileRect((xi+1)*Otc::TILE_PIXELS, (yi+1)*Otc::TILE_PIXELS, Otc::TILE_PIXELS, Otc::TILE_PIXELS);

            // only render creatures where bottom right is inside tile rect
            if(virtualTileRect.contains(virtualCreatureRect.bottomRight())) {
                newWalkingTile = g_map.getOrCreateTile(m_position.translated(xi, yi, 0));
            }
        }
    }

    if(newWalkingTile != m_walkingTile) {
        if(m_walkingTile)
            m_walkingTile->removeWalkingCreature(static_self_cast<Creature>());
        if(newWalkingTile)
            newWalkingTile->addWalkingCreature(static_self_cast<Creature>());
        m_walkingTile = newWalkingTile;
    }
}

void Creature::nextWalkUpdate()
{
    // remove any previous scheduled walk updates
    if(m_walkUpdateEvent)
        m_walkUpdateEvent->cancel();

    // do the update
    updateWalk();

    // schedules next update
    if(m_walking) {
        auto self = static_self_cast<Creature>();
        m_walkUpdateEvent = g_dispatcher.scheduleEvent([self] {
            self->m_walkUpdateEvent = nullptr;
            self->nextWalkUpdate();
        }, getStepDuration() / 32);
    }
}

void Creature::updateWalk()
{
    int stepDuration = getStepDuration();
    float walkTicksPerPixel = stepDuration / 32;
    int totalPixelsWalked = std::min(m_walkTimer.ticksElapsed() / walkTicksPerPixel, 32.0f);

    // needed for paralyze effect
    m_walkedPixels = std::max(m_walkedPixels, totalPixelsWalked);

    // update walk animation and offsets
    updateWalkAnimation(totalPixelsWalked);
    updateWalkOffset(m_walkedPixels);
    updateWalkingTile();

    // terminate walk
    if(m_walking && m_walkTimer.ticksElapsed() >= stepDuration)
        terminateWalk();
}

void Creature::terminateWalk()
{
    // remove any scheduled walk update
    if(m_walkUpdateEvent) {
        m_walkUpdateEvent->cancel();
        m_walkUpdateEvent = nullptr;
    }

    // now the walk has ended, do any scheduled turn
    if(m_walkTurnDirection != Otc::InvalidDirection)  {
        setDirection(m_walkTurnDirection);
        m_walkTurnDirection = Otc::InvalidDirection;
    }

    if(m_walkingTile) {
        m_walkingTile->removeWalkingCreature(static_self_cast<Creature>());
        m_walkingTile = nullptr;
    }

    m_walking = false;
    m_walkedPixels = 0;
}

void Creature::setName(const std::string& name)
{
    m_nameCache.setText(name);
    m_name = name;
}

void Creature::setHealthPercent(uint8 healthPercent)
{
    m_informationColor = Color::black;

    if(healthPercent > 92) {
        m_informationColor.setGreen(188);
    }
    else if(healthPercent > 60) {
        m_informationColor.setRed(80);
        m_informationColor.setGreen(161);
        m_informationColor.setBlue(80);
    }
    else if(healthPercent > 30) {
        m_informationColor.setRed(161);
        m_informationColor.setGreen(161);
    }
    else if(healthPercent > 8) {
        m_informationColor.setRed(160);
        m_informationColor.setGreen(39);
        m_informationColor.setBlue(39);
    }
    else if(healthPercent > 3) {
        m_informationColor.setRed(160);
    }
    else {
        m_informationColor.setRed(79);
    }

    m_healthPercent = healthPercent;
    callLuaField("onHealthPercentChange", healthPercent);
}

void Creature::setDirection(Otc::Direction direction)
{
    assert(direction != Otc::InvalidDirection);
    m_direction = direction;
}

void Creature::setOutfit(const Outfit& outfit)
{
    if(!g_things.isValidDatId(outfit.getId(), outfit.getCategory()))
        return;
    m_walkAnimationPhase = 0; // might happen when player is walking and outfit is changed.
    m_outfit = outfit;
}

void Creature::setSpeed(uint16 speed)
{
    m_speed = speed;

    // speed can change while walking (utani hur, paralyze, etc..)
    if(m_walking)
        nextWalkUpdate();
}

void Creature::setSkull(uint8 skull)
{
    m_skull = skull;
    callLuaField("onSkullChange", m_skull);
}

void Creature::setShield(uint8 shield)
{
    m_shield = shield;
    callLuaField("onShieldChange", m_shield);
}

void Creature::setEmblem(uint8 emblem)
{
    m_emblem = emblem;
    callLuaField("onEmblemChange", m_emblem);
}

void Creature::setSkullTexture(const std::string& filename)
{
    m_skullTexture = g_textures.getTexture(filename);
}

void Creature::setShieldTexture(const std::string& filename, bool blink)
{
    m_shieldTexture = g_textures.getTexture(filename);
    m_showShieldTexture = true;

    if(blink && !m_shieldBlink) {
        auto self = static_self_cast<Creature>();
        g_dispatcher.scheduleEvent([self]() {
            self->updateShield();
        }, SHIELD_BLINK_TICKS);
    }

    m_shieldBlink = blink;
}

void Creature::setEmblemTexture(const std::string& filename)
{
    m_emblemTexture = g_textures.getTexture(filename);
}

void Creature::addTimedSquare(uint8 color)
{
    m_showTimedSquare = true;
    m_timedSquareColor = Color::from8bit(color);

    // schedule removal
    auto self = static_self_cast<Creature>();
    g_dispatcher.scheduleEvent([self]() {
        self->removeTimedSquare();
    }, VOLATILE_SQUARE_DURATION);
}

void Creature::updateShield()
{
    m_showShieldTexture = !m_showShieldTexture;

    if(m_shield != Otc::ShieldNone && m_shieldBlink) {
        auto self = static_self_cast<Creature>();
        g_dispatcher.scheduleEvent([self]() {
            self->updateShield();
        }, SHIELD_BLINK_TICKS);
    }
    else if(!m_shieldBlink)
        m_showShieldTexture = true;
}

Point Creature::getDrawOffset()
{
    Point drawOffset;
    if(m_walking) {
        if(m_walkingTile)
            drawOffset -= Point(1,1) * m_walkingTile->getDrawElevation();
        drawOffset += m_walkOffset;
    } else {
        const TilePtr& tile = getTile();
        if(tile)
            drawOffset -= Point(1,1) * tile->getDrawElevation();
    }
    return drawOffset;
}

int Creature::getStepDuration()
{
    int groundSpeed = 0;
    const TilePtr& tile = getTile();
    if(tile)
        groundSpeed = tile->getGroundSpeed();

    int interval = 1000;
    if(groundSpeed > 0 && m_speed > 0)
        interval = (1000 * groundSpeed) / m_speed;

    if(g_game.getClientVersion() >= 900)
        interval = (interval / g_game.getServerBeat()) * g_game.getServerBeat();

    if(m_lastStepDirection == Otc::NorthWest || m_lastStepDirection == Otc::NorthEast ||
       m_lastStepDirection == Otc::SouthWest || m_lastStepDirection == Otc::SouthEast)
        interval *= 3;

    interval = std::max(interval, g_game.getServerBeat());

    return interval;
}

const ThingTypePtr& Creature::getThingType()
{
    return g_things.getThingType(m_outfit.getId(), m_outfit.getCategory());
}

ThingType* Creature::rawGetThingType()
{
    return g_things.rawGetThingType(m_outfit.getId(), m_outfit.getCategory());
}
