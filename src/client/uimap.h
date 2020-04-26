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

#ifndef UIMAP_H
#define UIMAP_H

#include "declarations.h"
#include <framework/ui/uiwidget.h>
#include "tile.h"

#include "mapview.h"

class UIMap : public UIWidget
{
public:
    UIMap();
    ~UIMap();

    void drawSelf(Fw::DrawPane drawPane);

    void movePixels(int x, int y);
    bool setZoom(int zoom);
    bool zoomIn();
    bool zoomOut();
    void followCreature(const CreaturePtr& creature) { m_mapView->followCreature(creature); }

    void setCameraPosition(const Position& pos) { m_mapView->setCameraPosition(pos); }
    void setMaxZoomIn(int maxZoomIn) { m_maxZoomIn = maxZoomIn; }
    void setMaxZoomOut(int maxZoomOut) { m_maxZoomOut = maxZoomOut; }
    void setMultifloor(bool enable) { m_mapView->setMultifloor(enable); }
    void lockVisibleFloor(int floor) { m_mapView->lockFirstVisibleFloor(floor); }
    void unlockVisibleFloor() { m_mapView->unlockFirstVisibleFloor(); }
    void setVisibleDimension(const Size& visibleDimension);
    void setViewMode(MapView::ViewMode viewMode)  { m_mapView->setViewMode(viewMode); }
    void setAutoViewMode(bool enable) { m_mapView->setAutoViewMode(enable); }
    void setDrawFlags(Otc::DrawFlags drawFlags) { m_mapView->setDrawFlags(drawFlags); }
    void setDrawTexts(bool enable) { m_mapView->setDrawTexts(enable); }
    void setDrawNames(bool enable) { m_mapView->setDrawNames(enable); }
    void setDrawHealthBars(bool enable) { m_mapView->setDrawHealthBars(enable); }
    void setDrawLights(bool enable) { m_mapView->setDrawLights(enable); }
    void setDrawManaBar(bool enable) { m_mapView->setDrawManaBar(enable); }
    void setAnimated(bool enable) { m_mapView->setAnimated(enable); }
    void setKeepAspectRatio(bool enable);
    void setMapShader(const PainterShaderProgramPtr& shader, float fadeout, float fadein) { m_mapView->setShader(shader, fadein, fadeout); }
    void setMinimumAmbientLight(float intensity) { m_mapView->setMinimumAmbientLight(intensity); }
    void setLimitVisibleRange(bool limitVisibleRange) { m_limitVisibleRange = limitVisibleRange; updateVisibleDimension(); }
    void setAddLightMethod(bool add) { m_mapView->setAddLightMethod(add); }

    bool isMultifloor() { return m_mapView->isMultifloor(); }
    bool isAutoViewModeEnabled() { return m_mapView->isAutoViewModeEnabled(); }
    bool isDrawingTexts() { return m_mapView->isDrawingTexts(); }
    bool isDrawingNames() { return m_mapView->isDrawingNames(); }
    bool isDrawingHealthBars() { return m_mapView->isDrawingHealthBars(); }
    bool isDrawingLights() { return m_mapView->isDrawingLights(); }
    bool isDrawingManaBar() { return m_mapView->isDrawingManaBar(); }
    bool isAnimating() { return m_mapView->isAnimating(); }
    bool isKeepAspectRatioEnabled() { return m_keepAspectRatio; }
    bool isLimitVisibleRangeEnabled() { return m_limitVisibleRange; }

    Size getVisibleDimension() { return m_mapView->getVisibleDimension(); }
    MapView::ViewMode getViewMode() { return m_mapView->getViewMode(); }
    CreaturePtr getFollowingCreature() { return m_mapView->getFollowingCreature(); }
    Otc::DrawFlags getDrawFlags() { return m_mapView->getDrawFlags(); }
    Position getCameraPosition() { return m_mapView->getCameraPosition(); }
    Position getPosition(const Point& mousePos);
    TilePtr getTile(const Point& mousePos);
    int getMaxZoomIn() { return m_maxZoomIn; }
    int getMaxZoomOut() { return m_maxZoomOut; }
    int getZoom() { return m_zoom; }
    PainterShaderProgramPtr getMapShader() { return m_mapView->getShader(); }
    float getMinimumAmbientLight() { return m_mapView->getMinimumAmbientLight(); }

protected:
    virtual void onStyleApply(const std::string& styleName, const OTMLNodePtr& styleNode);
    virtual void onGeometryChange(const Rect& oldRect, const Rect& newRect);

private:
    void updateVisibleDimension();
    void updateMapSize();

    int m_zoom;
    MapViewPtr m_mapView;
    Rect m_mapRect;
    float m_aspectRatio;
    bool m_keepAspectRatio;
    bool m_limitVisibleRange;
    int m_maxZoomIn;
    int m_maxZoomOut;
};

#endif
