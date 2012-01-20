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

#include "localplayer.h"
#include "map.h"
#include "game.h"
#include "tile.h"

LocalPlayer::LocalPlayer()
{
    m_preWalking = false;
    m_canReportBugs = false;
    m_known = false;
    m_walkLocked = false;
    m_lastPrewalkDone = true;
    m_icons = 0;

    for(int i = 0; i < Otc::LastStatistic; ++i)
        m_statistics[i] = -1; // sets an invalid value to ensure lua event will be sent
}

void LocalPlayer::lockWalk()
{
    // prevents double locks
    if(m_walkLocked)
        return;

    m_walkLocked = true;
    m_walkLockTimer.restart();
}

void LocalPlayer::walk(const Position& oldPos, const Position& newPos)
{
    Otc::Direction direction = oldPos.getDirectionFromPosition(newPos);

    // a prewalk was going on
    if(m_preWalking) {
        // switch to normal walking
        m_preWalking = false;
        m_lastPrewalkDone = true;
        // if is to the last prewalk destination, updates the walk preserving the animation
        if(newPos == m_lastPrewalkDestionation) {
            updateWalk();
        // was to another direction, replace the walk
        } else
            Creature::walk(oldPos, newPos);
    } else
        Creature::walk(oldPos, newPos);
}

void LocalPlayer::preWalk(Otc::Direction direction)
{
    // start walking to direction
    Position newPos = m_pos + Position::getPosFromDirection(direction);
    m_preWalking = true;
    m_lastPrewalkDone = false;
    m_lastPrewalkDestionation = newPos;
    Creature::walk(m_pos, newPos);
}

bool LocalPlayer::canWalk(Otc::Direction direction)
{
    // prewalk has a timeout, because for some reason that I don't know yet the server sometimes doesn't answer the prewalk
    bool prewalkTimeouted = m_walking && m_preWalking && m_walkTimer.ticksElapsed() >= m_walkInterval + PREWALK_TIMEOUT;

    // cannot walk while already walking
    if(m_walking && !prewalkTimeouted)
        return false;

    // avoid doing more walks than wanted when receiving a lot of walks from server
    if(!m_lastPrewalkDone && m_preWalking && !prewalkTimeouted)
        return false;

    // cannot walk while locked
    if(m_walkLocked && m_walkLockTimer.ticksElapsed() <= WALK_LOCK_INTERVAL)
        return false;
    else
        m_walkLocked = false;

    return true;
}

void LocalPlayer::cancelWalk(Otc::Direction direction)
{
    // only cancel client side walks
    if(m_walking && m_preWalking)
        stopWalk();

    m_lastPrewalkDone = true;

    // turn to the cancel direction
    if(direction != Otc::InvalidDirection)
        setDirection(direction);
}

void LocalPlayer::stopWalk()
{
    Creature::stopWalk();
    m_lastPrewalkDone = true;
    m_lastPrewalkDestionation = Position();
}

void LocalPlayer::updateWalkOffset(int totalPixelsWalked)
{
    // pre walks offsets are calculated in the oposite direction
    if(m_preWalking) {
        m_walkOffset = Point(0,0);
        if(m_direction == Otc::North || m_direction == Otc::NorthEast || m_direction == Otc::NorthWest)
            m_walkOffset.y = -totalPixelsWalked;
        else if(m_direction == Otc::South || m_direction == Otc::SouthEast || m_direction == Otc::SouthWest)
            m_walkOffset.y = totalPixelsWalked;

        if(m_direction == Otc::East || m_direction == Otc::NorthEast || m_direction == Otc::SouthEast)
            m_walkOffset.x = totalPixelsWalked;
        else if(m_direction == Otc::West || m_direction == Otc::NorthWest || m_direction == Otc::SouthWest)
            m_walkOffset.x = -totalPixelsWalked;
    } else
        Creature::updateWalkOffset(totalPixelsWalked);
}

void LocalPlayer::updateWalk()
{
    float walkTicksPerPixel = m_walkAnimationInterval / 32.0f;
    int totalPixelsWalked = std::min(m_walkTimer.ticksElapsed() / walkTicksPerPixel, 32.0f);

    // update walk animation and offsets
    updateWalkAnimation(totalPixelsWalked);
    updateWalkOffset(totalPixelsWalked);

    // terminate walk only when client and server side walk are complated
    if(m_walking && !m_preWalking && m_walkTimer.ticksElapsed() >= m_walkInterval)
        terminateWalk();
}

void LocalPlayer::terminateWalk()
{
    Creature::terminateWalk();
    m_preWalking = false;
}

void LocalPlayer::setAttackingCreature(const CreaturePtr& creature)
{
    // clear current attacking creature
    if(m_attackingCreature) {
        m_attackingCreature->hideStaticSquare();
        m_attackingCreature = nullptr;
    }

    // set the new attacking creature
    if(creature) {
        creature->showStaticSquare(Fw::red);
        m_attackingCreature = creature;
    }
}

void LocalPlayer::setFollowingCreature(const CreaturePtr& creature)
{
    // clear current following creature
    if(m_followingCreature) {
        m_followingCreature->hideStaticSquare();
        m_followingCreature = nullptr;
    }

    // set the new attacking creature
    if(creature) {
        creature->showStaticSquare(Fw::green);
        m_followingCreature = creature;
    }
}
