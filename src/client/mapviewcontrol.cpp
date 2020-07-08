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

#include "mapviewcontrol.h"

#include "map.h"

MapViewControl::MapViewControl(const Otc::Direction directionWalking)
{
    m_top = maxViewportY;
    m_right = maxViewportX;
    m_bottom = m_top;
    m_left = m_right;

    switch(directionWalking) {
    case Otc::North:
    case Otc::South:
        m_top += 1;
        m_bottom += 1;
        break;

    case Otc::West:
    case Otc::East:
        m_right += 1;
        m_left += 1;
        break;

    case Otc::NorthEast:
    case Otc::SouthEast:
    case Otc::NorthWest:
    case Otc::SouthWest:
        m_left += 1;
        m_bottom += 1;
        m_top += 1;
        m_right += 1;
        break;

    default:
        break;
    }
}

bool MapViewControl::isValid(const TilePtr& tile, const Position cameraPosition, LightView* lightView) const
{
    const Position tilePos = tile->getPosition();

    const int dz = tilePos.z - cameraPosition.z;
    const Position checkPos = tilePos.translated(dz, dz);

    // Check for non-visible tiles on the screen and ignore them
    {
        if(lightView && lightView->isDark() && tile->hasLight()) return true;

        if((cameraPosition.x - checkPos.x >= m_left) || (checkPos.x - cameraPosition.x == m_right && tile->isSingleDimension()))
            return false;

        if((cameraPosition.y - checkPos.y >= m_top) || (checkPos.y - cameraPosition.y == m_bottom && tile->isSingleDimension()))
            return false;

        if((checkPos.x - cameraPosition.x > m_right) || (checkPos.y - cameraPosition.y > m_bottom))
            return false;
    }

    return true;
}