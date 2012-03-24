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
#include <otclient/core/mapview.h>
#include <framework/otml/otml.h>
#include <framework/graphics/graphics.h>
#include <otclient/core/localplayer.h>

UIMap::UIMap()
{
    m_dragable = true;
    m_mapView = MapViewPtr(new MapView);
    g_map.addMapView(m_mapView);
}

UIMap::~UIMap()
{
    g_map.removeMapView(m_mapView);
}

void UIMap::drawSelf()
{
    UIWidget::drawSelf();

    // draw map border
    g_painter.setColor(Color::black);
    g_painter.drawBoundingRect(m_mapRect.expanded(1));

    m_mapView->draw(m_mapRect);
}

void UIMap::zoomIn()
{
    int dimensionHeight = m_mapView->getVisibleDimension().height() * 0.99;
    if(dimensionHeight == m_mapView->getVisibleDimension().height())
        dimensionHeight -= 1;
    if(dimensionHeight % 2 == 0)
        dimensionHeight -= 1;
    int dimensionWidth = dimensionHeight * getSize().ratio();
    if(dimensionWidth % 2 == 0)
        dimensionWidth -= 1;

    m_mapView->setVisibleDimension(Size(dimensionWidth, dimensionHeight));

    Rect mapRect = getChildrenRect().expanded(-1);
    Size mapSize = m_mapView->getVisibleSize();
    mapSize.scale(mapRect.size(), Fw::KeepAspectRatio);

    m_mapRect.resize(mapSize);
    m_mapRect.moveCenter(m_rect.center());
}

void UIMap::zoomOut()
{
    int dimensionHeight = m_mapView->getVisibleDimension().height() * 1.01;
    if(dimensionHeight == m_mapView->getVisibleDimension().height())
        dimensionHeight += 1;
    if(dimensionHeight % 2 == 0)
        dimensionHeight += 1;
    int dimensionWidth = dimensionHeight * getSize().ratio();
    if(dimensionWidth % 2 == 0)
        dimensionWidth += 1;

    m_mapView->setVisibleDimension(Size(dimensionWidth, dimensionHeight));

    Rect mapRect = getChildrenRect().expanded(-1);
    Size mapSize = m_mapView->getVisibleSize();
    mapSize.scale(mapRect.size(), Fw::KeepAspectRatio);

    m_mapRect.resize(mapSize);
    m_mapRect.moveCenter(m_rect.center());
}

void UIMap::followCreature(const CreaturePtr& creature)
{
    m_mapView->followCreature(creature);
}

void UIMap::setCameraPosition(const Position& pos)
{
    m_mapView->setCameraPosition(pos);
}

TilePtr UIMap::getTile(const Point& mousePos)
{
    if(!m_mapRect.contains(mousePos))
        return nullptr;

    //TODO: move MapView code to UIMap and rework this shit
    return m_mapView->getTile(mousePos, m_mapRect);
}

void UIMap::onGeometryChange(const Rect& oldRect, const Rect& newRect)
{
    UIWidget::onGeometryChange(oldRect, newRect);

    Rect mapRect = getChildrenRect().expanded(-1);
    Size mapSize = m_mapView->getVisibleSize();
    mapSize.scale(mapRect.size(), Fw::KeepAspectRatio);

    m_mapRect.resize(mapSize);
    m_mapRect.moveCenter(newRect.center());
}
