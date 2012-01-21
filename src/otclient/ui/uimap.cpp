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

#include "uimap.h"
#include <otclient/core/game.h>
#include <otclient/core/map.h>
#include <framework/otml/otml.h>
#include <framework/graphics/graphics.h>
#include <otclient/core/localplayer.h>

UIMap::UIMap()
{
    m_dragable = true;
}

void UIMap::draw()
{
    drawSelf();

    g_painter.setColor(Fw::black);
    g_painter.drawBoundingRect(m_mapRect.expanded(1));
    g_map.draw(m_mapRect);

    drawChildren();
}

TilePtr UIMap::getTile(const Point& mousePos)
{
    if(!m_mapRect.contains(mousePos))
        return nullptr;

    // Get tile position
    Point relativeStretchMousePos = mousePos - m_mapRect.topLeft();

    LocalPlayerPtr localPlayer = g_game.getLocalPlayer();
    if(localPlayer)
        relativeStretchMousePos += localPlayer->getWalkOffset();

    Size mapSize(g_map.getVibibleSize().width() * Map::NUM_TILE_PIXELS, g_map.getVibibleSize().height() * Map::NUM_TILE_PIXELS);

    PointF stretchFactor(m_mapRect.width() / (float)mapSize.width(), m_mapRect.height() / (float)mapSize.height());
    PointF relativeMousePos = PointF(relativeStretchMousePos.x, relativeStretchMousePos.y) / stretchFactor;

    PointF tilePosF = relativeMousePos / Map::NUM_TILE_PIXELS;
    Position tilePos = Position(1 + (int)tilePosF.x - g_map.getCentralOffset().x, 1 + (int)tilePosF.y - g_map.getCentralOffset().y, 0) + g_map.getCentralPosition();
    if(!tilePos.isValid())
        return nullptr;

    // Get tile
    TilePtr tile = nullptr;

    // We must check every floor, from top to bottom to check for a clickable tile
    int firstFloor = g_map.getFirstVisibleFloor();
    tilePos.perspectiveUp(tilePos.z - firstFloor);
    for(int i = firstFloor; i <= Map::MAX_Z; i++) {
        tile = g_map.getTile(tilePos);
        if(tile && tile->isClickable())
            break;
        tilePos.coveredDown();
    }

    // todo: get creature, using walkOffset etc.

    if(!tile || !tile->isClickable())
        return nullptr;

    return tile;
}

void UIMap::onGeometryChange(const Rect& oldRect, const Rect& newRect)
{
    Rect mapRect = getChildrenRect().expanded(-1);
    Size mapSize(g_map.getVibibleSize().width() * Map::NUM_TILE_PIXELS, g_map.getVibibleSize().height() * Map::NUM_TILE_PIXELS);
    mapSize.scale(mapRect.size(), Fw::KeepAspectRatio);

    m_mapRect.resize(mapSize);
    m_mapRect.moveCenter(newRect.center());
}
