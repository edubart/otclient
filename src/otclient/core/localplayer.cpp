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
    m_clientWalking = false;
    m_nextWalkDirection = Otc::InvalidDirection;
}

void LocalPlayer::clientWalk(Otc::Direction direction)
{
    // We're not walking, so start a client walk.
    if(!m_walking) {
        Position newPos = m_position + Position::getPosFromDirection(direction);
        Creature::walk(newPos, false);
        m_clientWalking = true;
    }
}

void LocalPlayer::walk(const Position& position, bool inverse)
{
    // This can only be received by protocol, so its always inverse.

    // If we're already walking, just finish it.
    if(m_clientWalking) {
        m_clientWalking = false;

        Position pos = Position::getPosFromDirection(m_direction);
        Point walkOffset = Point(m_walkOffset.x - pos.x * 32,
                                 m_walkOffset.y - pos.y * 32);

        Creature::walk(position, inverse);

        // Restore walk offset, because we were already walking.
        m_walkOffset = walkOffset;
    }
    // If we're not client walking, we'll just walk like every NPC. Ie: When player is pushed.
    else
        Creature::walk(position, inverse);
}

void LocalPlayer::cancelWalk(Otc::Direction direction, bool force)
{
    // Server said we cant walk. Ie: houses, vip areas.
    if(force) {
        m_clientWalking = false;
        Creature::cancelWalk(direction);
    }
    else {
        // Walk finished, and we already received the confirmation from server.
        if(m_walking && !m_clientWalking) {
            m_clientWalking = false;
            Creature::cancelWalk(direction);

            if(m_nextWalkDirection != Otc::InvalidDirection) {
                g_game.walk(m_nextWalkDirection);
                m_nextWalkDirection = Otc::InvalidDirection;
            }
        }
        //else..
        // Walk finished, however we havent received the confirmation from server. So wait for it.
    }
}

bool LocalPlayer::canWalk(Otc::Direction direction)
{
    if(m_walking) {
        if(direction != m_direction && m_nextWalkDirection != direction)
            m_nextWalkDirection = direction;
        else if(direction == m_direction && m_nextWalkDirection != Otc::InvalidDirection)
            m_nextWalkDirection = Otc::InvalidDirection;

        return false;
    }

    Position newPos = m_position + Position::getPosFromDirection(direction);
    TilePtr tile = g_map.getTile(newPos);
    if(!tile->isWalkable()) {
        g_game.processTextMessage("statusSmall", "Sorry, not possible.");
        return false;
    }

    return true;
}

void LocalPlayer::setAttackingCreature(const CreaturePtr& creature)
{
    if(m_attackingCreature) {
        m_attackingCreature->hideStaticSquare();
        m_attackingCreature = nullptr;
    }

    if(creature) {
        creature->showStaticSquare(Fw::red);
        m_attackingCreature = creature;
    }
}

void LocalPlayer::setFollowingCreature(const CreaturePtr& creature)
{
    if(m_followingCreature) {
        m_followingCreature->hideStaticSquare();
        m_followingCreature = nullptr;
    }

    if(creature) {
        creature->showStaticSquare(Fw::green);
        m_followingCreature = creature;
    }
}
