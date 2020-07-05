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

#include "creature.h"
#include "effect.h"
#include "game.h"
#include "item.h"
#include "lightview.h"
#include "localplayer.h"
#include "luavaluecasts.h"
#include "map.h"
#include "thingtypemanager.h"
#include "tile.h"

#include <framework/core/clock.h>
#include <framework/core/eventdispatcher.h>
#include <framework/graphics/graphics.h>

#include <framework/graphics/framebuffermanager.h>
#include <framework/graphics/paintershaderprogram.h>
#include <framework/graphics/texturemanager.h>
#include <framework/graphics/ogl/painterogl2_shadersources.h>
#include "spritemanager.h"

double Creature::speedA = 0;
double Creature::speedB = 0;
double Creature::speedC = 0;

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
    m_type = Proto::CreatureTypeUnknown;
    m_icon = Otc::NpcIconNone;
    m_lastStepDirection = Otc::InvalidDirection;
    m_nameCache.setFont(g_fonts.getFont("verdana-11px-rounded"));
    m_nameCache.setAlign(Fw::AlignTopCenter);
    m_footStep = 0;
    m_speedFormula.fill(-1);
    m_outfitColor = Color::white;
}

void Creature::draw(const Point& dest, float scaleFactor, LightView* lightView)
{
    if(!canBeSeen())
        return;

    if(m_showTimedSquare) {
        g_painter->setColor(m_timedSquareColor);
        g_painter->drawBoundingRect(Rect(dest + (m_walkOffset - getDisplacement() + 2) * scaleFactor, Size(28, 28) * scaleFactor), std::max<int>(static_cast<int>(2 * scaleFactor), 1));
        g_painter->setColor(Color::white);
    }

    if(m_showStaticSquare) {
        g_painter->setColor(m_staticSquareColor);
        g_painter->drawBoundingRect(Rect(dest + (m_walkOffset - getDisplacement()) * scaleFactor, Size(Otc::TILE_PIXELS, Otc::TILE_PIXELS) * scaleFactor), std::max<int>(static_cast<int>(2 * scaleFactor), 1));
        g_painter->setColor(Color::white);
    }

    internalDrawOutfit(dest + m_walkOffset * scaleFactor, scaleFactor, true, m_direction);

    if(lightView) {
        auto light = getLight();

        if(isLocalPlayer() && (g_map.getLight().intensity < 64 || m_position.z > Otc::SEA_FLOOR)) {
            light.intensity = std::max<uint8>(light.intensity, 1);
            if(light.color == 0 || light.color > 215) {
                light.color = 215;
            }
        }

        if(light.intensity > 0) {
            lightView->addLightSource(dest + (m_walkOffset + Point(16, 16)) * scaleFactor, scaleFactor, light);
        }
    }
}

void Creature::internalDrawOutfit(Point dest, float scaleFactor, bool animateWalk, Otc::Direction direction, LightView* lightView)
{
    g_painter->setColor(m_outfitColor);

    // outfit is a real creature
    if(m_outfit.getCategory() == ThingCategoryCreature) {
        int animationPhase = 0;

        // xPattern => creature direction
        int xPattern;
        if(direction == Otc::NorthEast || direction == Otc::SouthEast)
            xPattern = Otc::East;
        else if(direction == Otc::NorthWest || direction == Otc::SouthWest)
            xPattern = Otc::West;
        else
            xPattern = direction;

        int zPattern = 0;
        if(m_outfit.hasMount()) {
            if(animateWalk) animationPhase = getCurrentAnimationPhase(true);

            const auto& datType = rawGetMountThingType();

            dest -= datType->getDisplacement() * scaleFactor;
            datType->draw(dest, scaleFactor, 0, xPattern, 0, 0, animationPhase, lightView);
            dest += getDisplacement() * scaleFactor;

            zPattern = std::min<int>(1, getNumPatternZ() - 1);
        }

        if(animateWalk) animationPhase = getCurrentAnimationPhase();

        const PointF jumpOffset = m_jumpOffset * scaleFactor;
        dest -= Point(stdext::round(jumpOffset.x), stdext::round(jumpOffset.y));

        // yPattern => creature addon
        for(int yPattern = 0; yPattern < getNumPatternY(); ++yPattern) {

            // continue if we dont have this addon
            if(yPattern > 0 && !(m_outfit.getAddons() & (1 << (yPattern - 1))))
                continue;

            auto* datType = rawGetThingType();
            datType->draw(dest, scaleFactor, 0, xPattern, yPattern, zPattern, animationPhase, yPattern == 0 ? lightView : nullptr);

            if(getLayers() > 1) {
                Color oldColor = g_painter->getColor();
                const Painter::CompositionMode oldComposition = g_painter->getCompositionMode();
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
    } else {
        ThingType* type = g_things.rawGetThingType(m_outfit.getAuxId(), m_outfit.getCategory());

        int animationPhase = 0;
        int animationPhases = type->getAnimationPhases();
        int animateTicks = Otc::ITEM_TICKS_PER_FRAME;

        // when creature is an effect we cant render the first and last animation phase,
        // instead we should loop in the phases between
        if(m_outfit.getCategory() == ThingCategoryEffect) {
            animationPhases = std::max<int>(1, animationPhases - 2);
            animateTicks = Otc::INVISIBLE_TICKS_PER_FRAME;
        }

        if(animationPhases > 1) {
            animationPhase = (g_clock.millis() % (animateTicks * animationPhases)) / animateTicks;
        }

        if(m_outfit.getCategory() == ThingCategoryEffect)
            animationPhase = std::min<int>(animationPhase + 1, animationPhases);

        type->draw(dest - (getDisplacement() * scaleFactor), scaleFactor, 0, 0, 0, 0, animationPhase, lightView);
    }

    g_painter->resetColor();
}

void Creature::drawOutfit(const Rect& destRect, bool resize)
{
    int exactSize;
    if(m_outfit.getCategory() == ThingCategoryCreature)
        exactSize = getExactSize();
    else
        exactSize = g_things.rawGetThingType(m_outfit.getAuxId(), m_outfit.getCategory())->getExactSize();

    int frameSize;
    if(!resize)
        frameSize = std::max<int>(exactSize * 0.75f, 2 * Otc::TILE_PIXELS * 0.75f);
    else if((frameSize = exactSize) == 0)
        return;

    if(g_graphics.canUseFBO()) {
        const FrameBufferPtr& outfitBuffer = g_framebuffers.getTemporaryFrameBuffer();
        outfitBuffer->resize(Size(frameSize, frameSize));
        outfitBuffer->bind();
        g_painter->setAlphaWriting(true);
        g_painter->clear(Color::alpha);
        internalDrawOutfit(Point(frameSize - Otc::TILE_PIXELS, frameSize - Otc::TILE_PIXELS) + getDisplacement(), 1, false, Otc::South);
        outfitBuffer->release();
        outfitBuffer->draw(destRect, Rect(0, 0, frameSize, frameSize));
    } else {
        const float scaleFactor = destRect.width() / static_cast<float>(frameSize);
        const Point dest = destRect.bottomRight() - (Point(Otc::TILE_PIXELS, Otc::TILE_PIXELS) - getDisplacement()) * scaleFactor;
        internalDrawOutfit(dest, scaleFactor, false, Otc::South);
    }
}

void Creature::drawInformation(const Point& point, bool useGray, const Rect& parentRect, int drawFlags)
{
    if(m_healthPercent < 1) // creature is dead
        return;

    Color fillColor = Color(96, 96, 96);

    if(!useGray)
        fillColor = m_informationColor;

    // calculate main rects
    Rect backgroundRect = Rect(point.x - (13.5), point.y, 27, 4);
    // backgroundRect.bind(parentRect);

    const Size nameSize = m_nameCache.getTextSize();
    Rect textRect = Rect(point.x - nameSize.width() / 2.0, point.y - 12, nameSize);
    // textRect.bind(parentRect);

    // distance them
    uint32 offset = 12;
    if(isLocalPlayer()) {
        offset *= 2;
    }

    if(textRect.top() == parentRect.top())
        backgroundRect.moveTop(textRect.top() + offset);
    if(backgroundRect.bottom() == parentRect.bottom())
        textRect.moveTop(backgroundRect.top() - offset);

    // health rect is based on background rect, so no worries
    Rect healthRect = backgroundRect.expanded(-1);
    healthRect.setWidth((m_healthPercent / 100.0) * 25);

    // draw
    if(g_game.getFeature(Otc::GameBlueNpcNameColor) && isNpc() && m_healthPercent == 100 && !useGray)
        fillColor = Color(0x66, 0xcc, 0xff);

    if(drawFlags & Otc::DrawBars && (!isNpc() || !g_game.getFeature(Otc::GameHideNpcNames))) {
        g_painter->setColor(Color::black);
        g_painter->drawFilledRect(backgroundRect);

        g_painter->setColor(fillColor);
        g_painter->drawFilledRect(healthRect);

        if(drawFlags & Otc::DrawManaBar && isLocalPlayer()) {
            LocalPlayerPtr player = g_game.getLocalPlayer();
            if(player) {
                backgroundRect.moveTop(backgroundRect.bottom());

                g_painter->setColor(Color::black);
                g_painter->drawFilledRect(backgroundRect);

                Rect manaRect = backgroundRect.expanded(-1);
                const double maxMana = player->getMaxMana();
                if(maxMana == 0) {
                    manaRect.setWidth(25);
                } else {
                    manaRect.setWidth(player->getMana() / (maxMana * 1.0) * 25);
                }

                g_painter->setColor(Color::blue);
                g_painter->drawFilledRect(manaRect);
            }
        }
    }

    if(drawFlags & Otc::DrawNames) {
        if(g_painter->getColor() != fillColor)
            g_painter->setColor(fillColor);
        m_nameCache.draw(textRect);
    }

    if(m_skull != Otc::SkullNone && m_skullTexture) {
        g_painter->setColor(Color::white);
        const Rect skullRect = Rect(backgroundRect.x() + 13.5 + 12, backgroundRect.y() + 5, m_skullTexture->getSize());
        g_painter->drawTexturedRect(skullRect, m_skullTexture);
    }
    if(m_shield != Otc::ShieldNone && m_shieldTexture && m_showShieldTexture) {
        g_painter->setColor(Color::white);
        const Rect shieldRect = Rect(backgroundRect.x() + 13.5, backgroundRect.y() + 5, m_shieldTexture->getSize());
        g_painter->drawTexturedRect(shieldRect, m_shieldTexture);
    }
    if(m_emblem != Otc::EmblemNone && m_emblemTexture) {
        g_painter->setColor(Color::white);
        const Rect emblemRect = Rect(backgroundRect.x() + 13.5 + 12, backgroundRect.y() + 16, m_emblemTexture->getSize());
        g_painter->drawTexturedRect(emblemRect, m_emblemTexture);
    }
    if(m_type != Proto::CreatureTypeUnknown && m_typeTexture) {
        g_painter->setColor(Color::white);
        const Rect typeRect = Rect(backgroundRect.x() + 13.5 + 12 + 12, backgroundRect.y() + 16, m_typeTexture->getSize());
        g_painter->drawTexturedRect(typeRect, m_typeTexture);
    }
    if(m_icon != Otc::NpcIconNone && m_iconTexture) {
        g_painter->setColor(Color::white);
        const Rect iconRect = Rect(backgroundRect.x() + 13.5 + 12, backgroundRect.y() + 5, m_iconTexture->getSize());
        g_painter->drawTexturedRect(iconRect, m_iconTexture);
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
    m_lastStepFromPosition = oldPos;
    m_lastStepToPosition = newPos;

    // set current walking direction
    setDirection(m_lastStepDirection);

    // starts counting walk
    m_walking = true;
    m_walkTimer.restart();
    m_walkedPixels = 0;

    // no direction need to be changed when the walk ends
    m_walkTurnDirection = Otc::InvalidDirection;

    if(m_walkFinishAnimEvent) {
        m_walkFinishAnimEvent->cancel();
        m_walkFinishAnimEvent = nullptr;
    }

    // starts updating walk
    nextWalkUpdate();
}

void Creature::stopWalk()
{
    if(!m_walking)
        return;

    // stops the walk right away
    terminateWalk();
}

void Creature::jump(int height, int duration)
{
    if(!m_jumpOffset.isNull())
        return;

    m_jumpTimer.restart();
    m_jumpHeight = height;
    m_jumpDuration = duration;

    updateJump();
}

void Creature::updateJump()
{
    const int t = m_jumpTimer.ticksElapsed();
    const double a = -4 * m_jumpHeight / (m_jumpDuration * m_jumpDuration);
    const double b = +4 * m_jumpHeight / m_jumpDuration;

    const double height = a * t * t + b * t;
    const int roundHeight = stdext::round(height);
    const int halfJumpDuration = m_jumpDuration / 2;

    // schedules next update
    if(m_jumpTimer.ticksElapsed() < m_jumpDuration) {
        m_jumpOffset = PointF(height, height);

        int diff = 0;
        if(m_jumpTimer.ticksElapsed() < halfJumpDuration)
            diff = 1;
        else if(m_jumpTimer.ticksElapsed() > halfJumpDuration)
            diff = -1;

        int nextT, i = 1;
        do {
            nextT = stdext::round((-b + std::sqrt(std::max<double>(b * b + 4 * a * (roundHeight + diff * i), 0.0)) * diff) / (2 * a));
            ++i;

            if(nextT < halfJumpDuration)
                diff = 1;
            else if(nextT > halfJumpDuration)
                diff = -1;
        } while(nextT - m_jumpTimer.ticksElapsed() == 0 && i < 3);

        auto self = static_self_cast<Creature>();
        g_dispatcher.scheduleEvent([self] {
            self->updateJump();
            self->requestDrawing();
        }, nextT - m_jumpTimer.ticksElapsed());
    } else
        m_jumpOffset = PointF(0, 0);
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

    const auto idleAnimator = getIdleAnimator();
    if(idleAnimator) startListenerPainter(idleAnimator->getAverageDuration());
    else if(isAnimateAlways()) startListenerPainter(1000 / getAnimationPhases());

    // creature appeared the first time or wasn't seen for a long time
    if(m_removed) {
        stopWalk();
        m_removed = false;
        callLuaField("onAppear");

    } // walk
    else if(m_oldPosition != m_position && m_oldPosition.isInRange(m_position, 1, 1) && m_allowAppearWalk) {
        m_allowAppearWalk = false;
        walk(m_oldPosition, m_position);
        callLuaField("onWalk", m_oldPosition, m_position);

    } // teleport
    else if(m_oldPosition != m_position) {
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

void Creature::onDeath()
{
    callLuaField("onDeath");
}

void Creature::updateWalkAnimation()
{
    if(m_outfit.getCategory() != ThingCategoryCreature)
        return;

    const int footAnimPhases = getTotalAnimationPhase();

    const int footDelay = m_stepCache.getDuration(m_lastStepDirection) / footAnimPhases;

    if(m_footTimer.ticksElapsed() >= footDelay) {
        if(m_walkAnimationPhase == footAnimPhases) m_walkAnimationPhase = 1;
        else ++m_walkAnimationPhase;

        m_footTimer.restart();
    }
}

void Creature::updateWalkOffset(int totalPixelsWalked)
{
    m_walkOffset = Point(0, 0);
    if(m_direction == Otc::North || m_direction == Otc::NorthEast || m_direction == Otc::NorthWest)
        m_walkOffset.y = Otc::TILE_PIXELS - totalPixelsWalked;
    else if(m_direction == Otc::South || m_direction == Otc::SouthEast || m_direction == Otc::SouthWest)
        m_walkOffset.y = totalPixelsWalked - Otc::TILE_PIXELS;

    if(m_direction == Otc::East || m_direction == Otc::NorthEast || m_direction == Otc::SouthEast)
        m_walkOffset.x = totalPixelsWalked - Otc::TILE_PIXELS;
    else if(m_direction == Otc::West || m_direction == Otc::NorthWest || m_direction == Otc::SouthWest)
        m_walkOffset.x = Otc::TILE_PIXELS - totalPixelsWalked;
}

void Creature::updateWalkingTile()
{
    // determine new walking tile
    TilePtr newWalkingTile;

    const Rect virtualCreatureRect(Otc::TILE_PIXELS + (m_walkOffset.x - getDisplacementX()),
                                   Otc::TILE_PIXELS + (m_walkOffset.y - getDisplacementY()),
                                   Otc::TILE_PIXELS, Otc::TILE_PIXELS);

    for(int xi = -1; xi <= 1 && !newWalkingTile; ++xi) {
        for(int yi = -1; yi <= 1 && !newWalkingTile; ++yi) {
            Rect virtualTileRect((xi + 1) * Otc::TILE_PIXELS, (yi + 1) * Otc::TILE_PIXELS, Otc::TILE_PIXELS, Otc::TILE_PIXELS);

            // only render creatures where bottom right is inside tile rect
            if(virtualTileRect.contains(virtualCreatureRect.bottomRight())) {
                newWalkingTile = g_map.getOrCreateTile(m_position.translated(xi, yi, 0));
            }
        }
    }

    if(newWalkingTile == m_walkingTile) return;

    if(m_walkingTile)
        m_walkingTile->removeWalkingCreature(static_self_cast<Creature>());

    if(newWalkingTile) {
        newWalkingTile->addWalkingCreature(static_self_cast<Creature>());

        // recache visible tiles in map views
        if(newWalkingTile->isEmpty()) {
            g_map.notificateTileUpdate(newWalkingTile->getPosition());
        }
    }

    m_walkingTile = newWalkingTile;
}

void Creature::nextWalkUpdate()
{
    // remove any previous scheduled walk updates
    if(m_walkUpdateEvent)
        m_walkUpdateEvent->cancel();

    // do the update
    updateWalk();

    if(!m_walking) return;

    // schedules next update
    auto self = static_self_cast<Creature>();
    m_walkUpdateEvent = g_dispatcher.scheduleEvent([self] {
        self->m_walkUpdateEvent = nullptr;
        self->nextWalkUpdate();

        self->requestDrawing();
    }, std::max<int>(m_stepCache.getDuration(m_lastStepDirection) / Otc::TILE_PIXELS, 15));
}

void Creature::updateWalk(const bool isPreWalking)
{
    // Generate step cache for no-localPlayer
    if(!isLocalPlayer()) getStepDuration(true);

    // update walk animation
    updateWalkAnimation();

    int stepDuration = m_stepCache.duration + (12 - m_stepCache.duration * .01);
    const float walkTicksPerPixel = stepDuration / Otc::TILE_PIXELS;
    const int totalPixelsWalked = std::min<int>(m_walkTimer.ticksElapsed() / walkTicksPerPixel, Otc::TILE_PIXELS);

    // needed for paralyze effect
    m_walkedPixels = std::max<int>(m_walkedPixels, totalPixelsWalked);

    // update offsets
    updateWalkOffset(m_walkedPixels);

    updateWalkingTile();

    // terminate walk only when client and server side walk are completed
    if(m_walking && !isPreWalking && m_walkTimer.ticksElapsed() >= stepDuration) {
        terminateWalk();
    }
}

void Creature::terminateWalk()
{
    // remove any scheduled walk update
    if(m_walkUpdateEvent) {
        m_walkUpdateEvent->cancel();
        m_walkUpdateEvent = nullptr;
    }

    // now the walk has ended, do any scheduled turn
    if(m_walkTurnDirection != Otc::InvalidDirection) {
        setDirection(m_walkTurnDirection);
        m_walkTurnDirection = Otc::InvalidDirection;
    }

    if(m_walkingTile) {
        m_walkingTile->removeWalkingCreature(static_self_cast<Creature>());
        m_walkingTile = nullptr;
    }

    m_walkedPixels = 0;
    m_walkOffset = Point(0, 0);
    m_walking = false;

    const auto self = static_self_cast<Creature>();
    m_walkFinishAnimEvent = g_dispatcher.scheduleEvent([self] {
        self->m_walkAnimationPhase = 0;
        self->m_walkFinishAnimEvent = nullptr;

        self->requestDrawing();
    }, g_game.getServerBeat());

}

void Creature::setName(const std::string& name)
{
    m_nameCache.setText(name);
    m_name = name;
}

void Creature::setHealthPercent(uint8 healthPercent)
{
    if(healthPercent > 92)
        m_informationColor = Color(0x00, 0xBC, 0x00);
    else if(healthPercent > 60)
        m_informationColor = Color(0x50, 0xA1, 0x50);
    else if(healthPercent > 30)
        m_informationColor = Color(0xA1, 0xA1, 0x00);
    else if(healthPercent > 8)
        m_informationColor = Color(0xBF, 0x0A, 0x0A);
    else if(healthPercent > 3)
        m_informationColor = Color(0x91, 0x0F, 0x0F);
    else
        m_informationColor = Color(0x85, 0x0C, 0x0C);

    m_healthPercent = healthPercent;
    callLuaField("onHealthPercentChange", healthPercent);

    if(healthPercent <= 0)
        onDeath();
}

void Creature::setDirection(Otc::Direction direction)
{
    assert(direction != Otc::InvalidDirection);
    m_direction = direction;
}

void Creature::setOutfit(const Outfit& outfit)
{
    const Outfit oldOutfit = m_outfit;
    if(outfit.getCategory() != ThingCategoryCreature) {
        if(!g_things.isValidDatId(outfit.getAuxId(), outfit.getCategory()))
            return;

        m_outfit.setAuxId(outfit.getAuxId());
        m_outfit.setCategory(outfit.getCategory());
    } else {
        if(outfit.getId() > 0 && !g_things.isValidDatId(outfit.getId(), ThingCategoryCreature))
            return;

        m_outfit = outfit;
    }

    m_walkAnimationPhase = 0; // might happen when player is walking and outfit is changed.

    callLuaField("onOutfitChange", m_outfit, oldOutfit);
}

void Creature::setOutfitColor(const Color& color, int duration)
{
    if(m_outfitColorUpdateEvent) {
        m_outfitColorUpdateEvent->cancel();
        m_outfitColorUpdateEvent = nullptr;
    }

    if(duration <= 0) {
        m_outfitColor = color;
        return;
    }

    const Color delta = (color - m_outfitColor) / static_cast<float>(duration);
    m_outfitColorTimer.restart();
    updateOutfitColor(m_outfitColor, color, delta, duration);
}

void Creature::updateOutfitColor(Color color, Color finalColor, Color delta, int duration)
{
    if(m_outfitColorTimer.ticksElapsed() >= duration) {
        m_outfitColor = finalColor;
        return;
    }

    m_outfitColor = color + delta * m_outfitColorTimer.ticksElapsed();

    const auto self = static_self_cast<Creature>();
    m_outfitColorUpdateEvent = g_dispatcher.scheduleEvent([=] {
        self->updateOutfitColor(color, finalColor, delta, duration);
    }, 100);
}

void Creature::setSpeed(uint16 speed)
{
    const uint16 oldSpeed = m_speed;
    m_speed = speed;

    // Cache for stepSpeed Law
    if(g_game.getFeature(Otc::GameNewSpeedLaw)) {
        speed *= 2;

        if(speed > -speedB) {
            m_calculatedStepSpeed = floor((Creature::speedA * log((speed / 2) + Creature::speedB) + Creature::speedC) + 0.5);
            if(m_calculatedStepSpeed == 0) m_calculatedStepSpeed = 1;
        } else m_calculatedStepSpeed = 1;
    }

    // speed can change while walking (utani hur, paralyze, etc..)
    if(m_walking)
        nextWalkUpdate();

    callLuaField("onSpeedChange", m_speed, oldSpeed);
}

void Creature::setBaseSpeed(double baseSpeed)
{
    if(m_baseSpeed != baseSpeed) {
        const double oldBaseSpeed = m_baseSpeed;
        m_baseSpeed = baseSpeed;

        callLuaField("onBaseSpeedChange", baseSpeed, oldBaseSpeed);
    }
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

void Creature::setType(uint8 type)
{
    m_type = type;
    callLuaField("onTypeChange", m_type);
}

void Creature::setIcon(uint8 icon)
{
    m_icon = icon;
    callLuaField("onIconChange", m_icon);
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

void Creature::setTypeTexture(const std::string& filename)
{
    m_typeTexture = g_textures.getTexture(filename);
}

void Creature::setIconTexture(const std::string& filename)
{
    m_iconTexture = g_textures.getTexture(filename);
}

void Creature::addTimedSquare(uint8 color)
{
    m_showTimedSquare = true;
    m_timedSquareColor = Color::from8bit(color);

    // schedule removal
    const auto self = static_self_cast<Creature>();
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
    } else if(!m_shieldBlink)
        m_showShieldTexture = true;
}

Point Creature::getDrawOffset()
{
    Point drawOffset;
    if(m_walking) {
        if(m_walkingTile)
            drawOffset -= Point(1, 1) * m_walkingTile->getDrawElevation();
        drawOffset += m_walkOffset;
    } else {
        const TilePtr& tile = getTile();
        if(tile)
            drawOffset -= Point(1, 1) * tile->getDrawElevation();
    }
    return drawOffset;
}

int Creature::getStepDuration(bool ignoreDiagonal, Otc::Direction dir)
{
    int stepSpeed = m_speed;
    if(stepSpeed < 1)
        return 0;

    Position tilePos;
    int groundSpeed;

    if(dir == Otc::InvalidDirection)
        tilePos = m_lastStepToPosition;
    else
        tilePos = m_position.translatedToDirection(dir);

    if(!tilePos.isValid())
        tilePos = m_position;

    const TilePtr& tile = g_map.getTile(tilePos);
    if(tile) {
        groundSpeed = tile->getGroundSpeed();
        if(groundSpeed == 0)
            groundSpeed = 150;
    } else groundSpeed = 150;

    if(stepSpeed != m_stepCache.speed || groundSpeed != m_stepCache.groundSpeed) {
        m_stepCache.speed = m_speed;
        m_stepCache.groundSpeed = groundSpeed;

        double stepDuration = 1000. * groundSpeed;
        if(g_game.getFeature(Otc::GameNewSpeedLaw)) {
            stepDuration = std::floor(stepDuration / m_calculatedStepSpeed);
        } else stepDuration /= stepSpeed;

        if(g_game.getClientVersion() >= 900) {
            const auto serverBeat = g_game.getServerBeat();
            stepDuration = std::ceil(stepDuration / serverBeat) * serverBeat;
        }

        m_stepCache.duration = stepDuration;
        m_stepCache.diagonalDuration = stepDuration * (g_game.getClientVersion() <= 810 ? 2 : 3);
    }

    return ignoreDiagonal ? m_stepCache.duration : m_stepCache.getDuration(m_lastStepDirection);
}

Point Creature::getDisplacement()
{
    if(m_outfit.getCategory() == ThingCategoryEffect)
        return Point(8, 8);

    if(m_outfit.getCategory() == ThingCategoryItem)
        return Point(0, 0);

    return Thing::getDisplacement();
}

int Creature::getDisplacementX()
{
    if(m_outfit.getCategory() == ThingCategoryEffect)
        return 8;

    if(m_outfit.getCategory() == ThingCategoryItem)
        return 0;

    if(m_outfit.hasMount())
        return rawGetMountThingType()->getDisplacementX();

    return Thing::getDisplacementX();
}

int Creature::getDisplacementY()
{
    if(m_outfit.getCategory() == ThingCategoryEffect)
        return 8;

    if(m_outfit.getCategory() == ThingCategoryItem)
        return 0;

    if(m_outfit.hasMount()) {
        return rawGetMountThingType()->getDisplacementY();
    }

    return Thing::getDisplacementY();
}

Light Creature::getLight()
{
    Light light = Thing::getLight();

    if(m_light.color > 0 && m_light.intensity >= light.intensity)
        light = m_light;

    return light;
}

int Creature::getTotalAnimationPhase()
{
    if(!m_outfit.hasMount()) return getAnimationPhases();

    return rawGetMountThingType()->getAnimationPhases();
}

int Creature::getCurrentAnimationPhase(const bool mount)
{
    const auto& thingType = mount ? rawGetMountThingType() : rawGetThingType();

    const auto idleAnimator = thingType->getIdleAnimator();
    if(idleAnimator) {
        if(m_walkAnimationPhase == 0) return idleAnimator->getPhase();
        return m_walkAnimationPhase + idleAnimator->getAnimationPhases() - 1;
    }

    if(thingType->isAnimateAlways()) {
        const int ticksPerFrame = std::round(1000 / thingType->getAnimationPhases());
        return (g_clock.millis() % (ticksPerFrame * thingType->getAnimationPhases())) / ticksPerFrame;
    }

    return m_walkAnimationPhase;
}

int Creature::getExactSize(int layer, int xPattern, int yPattern, int zPattern, int animationPhase)
{
    const int numPatternY = getNumPatternY(),
        layers = getLayers();

    animationPhase = 0;

    if(m_outfit.hasMount())
        zPattern = m_outfit.hasMount() ? 1 : 0;

    int exactSize = 0;
    for(yPattern = 0; yPattern < numPatternY; ++yPattern) {
        if(yPattern > 0 && !(m_outfit.getAddons() & (1 << (yPattern - 1))))
            continue;

        for(layer = 0; layer < layers; ++layer)
            exactSize = std::max<int>(exactSize, Thing::getExactSize(layer, xPattern, yPattern, zPattern, animationPhase));
    }

    return exactSize;
}

const ThingTypePtr& Creature::getThingType()
{
    return g_things.getThingType(m_outfit.getId(), m_outfit.getCategory());
}

ThingType* Creature::rawGetThingType()
{
    return g_things.rawGetThingType(m_outfit.getId(), m_outfit.getCategory());
}

ThingType* Creature::rawGetMountThingType()
{
    return g_things.rawGetThingType(m_outfit.getMount(), m_outfit.getCategory());
}