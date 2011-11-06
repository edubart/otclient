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

#include "localplayer.h"
#include "map.h"
#include "game.h"
#include "tile.h"

LocalPlayer::LocalPlayer()
{
    m_clientWalking = false;
}

void LocalPlayer::clientWalk(Otc::Direction direction)
{
    if(!m_walking) {
        Position newPos = m_position + Position::getPositionFromDirection(direction);
        Creature::walk(newPos, false);
        m_clientWalking = true;
    }
}

void LocalPlayer::walk(const Position& position, bool inverse)
{
    if(m_clientWalking) {
        Position pos = Position::getPositionFromDirection(m_direction);
        int walkOffsetX = m_walkOffsetX - pos.x * 32;
        int walkOffsetY = m_walkOffsetY - pos.y * 32;

        Creature::walk(position, inverse);

        m_walkOffsetX = walkOffsetX;
        m_walkOffsetY = walkOffsetY;
        m_clientWalking = false;
    }
    else {
        m_walkOffsetX = 0;
        m_walkOffsetY = 0;
        Creature::walk(position, inverse);
    }
}

void LocalPlayer::cancelWalk(Otc::Direction direction)
{
    m_clientWalking = false;
    Creature::cancelWalk(direction);
}

bool LocalPlayer::canWalk(Otc::Direction direction)
{
    if(m_walking)
        return false;

    Position newPos = m_position + Position::getPositionFromDirection(direction);
    TilePtr tile = g_map.getTile(newPos);
    if(!tile->isWalkable()) {
        // TODO: create enum for 17, white message on screen bottom and console.
        g_game.processTextMessage(17, "Sorry, not possible.");
        return false;
    }

    return true;
}
