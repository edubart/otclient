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

void LocalPlayer::preWalk(Otc::Direction direction)
{
    // we're not walking, so start a client walk.
    Position newPos = m_pos + Position::getPosFromDirection(direction);
    walk(m_pos, newPos, true);
}

bool LocalPlayer::canWalk(Otc::Direction direction)
{
    if(m_walking || (m_preWalking && g_clock.ticksElapsed(m_walkEnd) < 1000))
        return false;

    // check for blockable tiles in the walk direction
    TilePtr tile = g_map.getTile(m_pos + Position::getPosFromDirection(direction));
    if(!tile)
        return false;

    if(!tile->isWalkable()) {
        g_game.processTextMessage("statusSmall", "Sorry, not possible.");
        return false;
    }

    return true;
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
