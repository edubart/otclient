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
    m_zoom = m_mapView->getVisibleDimension().height();
    m_aspectRatio = 0.0f;
    m_maxZoomIn = 3;
    m_maxZoomOut = 512;
    m_mapRect.resize(1,1);
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
    g_painter->setColor(Color::black);
    g_painter->drawBoundingRect(m_mapRect.expanded(1));

    g_painter->setColor(Color::white);
    m_mapView->draw(m_mapRect);
}

bool UIMap::setZoom(int zoom)
{
    m_zoom = std::min(std::max(zoom, m_maxZoomIn), m_maxZoomOut);
    updateVisibleDimension();
    return false;
}

bool UIMap::zoomIn()
{
    int delta = 2;
    if(m_zoom - delta <= m_maxZoomIn)
        return false;

    m_zoom -= delta;
    updateVisibleDimension();
    return true;
}

bool UIMap::zoomOut()
{
    int delta = 2;
    if(m_zoom + delta >= m_maxZoomOut)
        return false;

    m_zoom += 2;
    updateVisibleDimension();
    return true;
}

void UIMap::setVisibleDimension(const Size& visibleDimension)
{
    m_mapView->setVisibleDimension(visibleDimension);

    if(m_aspectRatio != 0.0f)
        m_aspectRatio = visibleDimension.ratio();
}

void UIMap::setKeepAspectRatio(bool enable)
{
    if(enable)
        m_aspectRatio = getVisibleDimension().ratio();
    else
        m_aspectRatio = 0.0f;
}

TilePtr UIMap::getTile(const Point& mousePos)
{
    if(!m_mapRect.contains(mousePos))
        return nullptr;

    //TODO: move MapView code to UIMap and rework this shit
    return m_mapView->getTile(mousePos, m_mapRect);
}

void UIMap::onStyleApply(const std::string& styleName, const OTMLNodePtr& styleNode)
{
    UIWidget::onStyleApply(styleName, styleNode);
    for(const OTMLNodePtr& node : styleNode->children()) {
        if(node->tag() == "multifloor")
            setMultifloor(node->value<bool>());
        else if(node->tag() == "auto-view-mode")
            setAutoViewMode(node->value<bool>());
        else if(node->tag() == "draw-texts")
            setDrawTexts(node->value<bool>());
        else if(node->tag() == "draw-minimap-colors")
            setDrawMinimapColors(node->value<bool>());
        else if(node->tag() == "animated")
            setAnimated(node->value<bool>());
    }
}

void UIMap::onGeometryChange(const Rect& oldRect, const Rect& newRect)
{
    UIWidget::onGeometryChange(oldRect, newRect);
    updateMapSize();
}

void UIMap::updateVisibleDimension()
{
    int dimensionHeight = m_zoom;
    if(dimensionHeight % 2 == 0)
        dimensionHeight += 1;
    int dimensionWidth = m_zoom * m_mapRect.size().ratio();
    if(dimensionWidth % 2 == 0)
        dimensionWidth += 1;

    m_mapView->setVisibleDimension(Size(dimensionWidth, dimensionHeight));

    if(m_aspectRatio != 0.0f)
        updateMapSize();
}

void UIMap::updateMapSize()
{
    Rect clippingRect = getClippingRect();
    Size mapSize;
    if(m_aspectRatio != 0.0f) {
        Rect mapRect = clippingRect.expanded(-1);
        mapSize = Size(m_aspectRatio*m_zoom, m_zoom);
        mapSize.scale(mapRect.size(), Fw::KeepAspectRatio);
    } else {
        mapSize = clippingRect.expanded(-1).size();
    }

    m_mapRect.resize(mapSize);
    m_mapRect.moveCenter(clippingRect.center());
    m_mapView->optimizeForSize(mapSize);

    if(m_aspectRatio == 0.0f)
        updateVisibleDimension();
}

