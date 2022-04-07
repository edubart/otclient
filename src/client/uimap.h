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
#include "tile.h"
#include <framework/ui/uiwidget.h>

#include "mapview.h"

class UIMap : public UIWidget
{
public:
    UIMap();
    ~UIMap() override;

    void drawSelf(Fw::DrawPane drawPane) override;

    void movePixels(int x, int y);
    void followCreature(const CreaturePtr& creature) { m_mapView->followCreature(creature); }
    void setCameraPosition(const Position& pos) { m_mapView->setCameraPosition(pos); }
    void setMaxZoomIn(int maxZoomIn) { m_maxZoomIn = maxZoomIn; }
    void setMaxZoomOut(int maxZoomOut) { m_maxZoomOut = maxZoomOut; }
    void lockVisibleFloor(int floor) { m_mapView->lockFirstVisibleFloor(floor); }
    void unlockVisibleFloor() { m_mapView->unlockFirstVisibleFloor(); }
    void setVisibleDimension(const Size& visibleDimension);
    void setFloorViewMode(MapView::FloorViewMode viewMode) { m_mapView->setFloorViewMode(viewMode); }
    void setDrawTexts(bool enable) { m_mapView->setDrawTexts(enable); }
    void setDrawNames(bool enable) { m_mapView->setDrawNames(enable); }
    void setDrawHealthBars(bool enable) { m_mapView->setDrawHealthBars(enable); }
    void setDrawLights(bool enable) { m_mapView->setDrawLights(enable); }
    void setDrawViewportEdge(bool enable) { m_mapView->setDrawViewportEdge(enable); }
    void setDrawManaBar(bool enable) { m_mapView->setDrawManaBar(enable); }
    void setKeepAspectRatio(bool enable);
    void setMapShader(const PainterShaderProgramPtr& shader, float fadein, float fadeout) { m_mapView->setShader(shader, fadein, fadeout); }
    void setMinimumAmbientLight(float intensity) { m_mapView->setMinimumAmbientLight(intensity); }
    void setLimitVisibleRange(bool limitVisibleRange) { m_limitVisibleRange = limitVisibleRange; updateVisibleDimension(); }

    bool zoomIn();
    bool zoomOut();
    bool setZoom(int zoom);
    bool isDrawingTexts() { return m_mapView->isDrawingTexts(); }
    bool isDrawingNames() { return m_mapView->isDrawingNames(); }
    bool isDrawingHealthBars() { return m_mapView->isDrawingHealthBars(); }
    bool isDrawingLights() { return m_mapView->isDrawingLights(); }
    bool isDrawingViewportEdge() { return m_mapView->isDrawingViewportEdge(); }
    bool isDrawingManaBar() { return m_mapView->isDrawingManaBar(); }
    bool isKeepAspectRatioEnabled() { return m_keepAspectRatio; }
    bool isLimitVisibleRangeEnabled() { return m_limitVisibleRange; }

    void setShadowFloorIntensity(float intensity) { m_mapView->setShadowFloorIntensity(intensity); }

    std::vector<CreaturePtr> getVisibleCreatures() { return m_mapView->getVisibleCreatures(); }
    std::vector<CreaturePtr> getSpectators(const Position& centerPos, bool multiFloor) { return m_mapView->getSpectators(centerPos, multiFloor); }
    std::vector<CreaturePtr> getSightSpectators(const Position& centerPos, bool multiFloor) { return m_mapView->getSightSpectators(centerPos, multiFloor); }
    bool isInRange(const Position& pos) { return m_mapView->isInRange(pos); }

    PainterShaderProgramPtr getMapShader() { return m_mapView->getShader(); }
    MapView::FloorViewMode getFloorViewMode() { return m_mapView->getFloorViewMode(); }
    CreaturePtr getFollowingCreature() { return m_mapView->getFollowingCreature(); }
    Position getCameraPosition() { return m_mapView->getCameraPosition(); }
    Position getPosition(const Point& mousePos);
    TilePtr getTile(const Point& mousePos);
    Size getVisibleDimension() { return m_mapView->getVisibleDimension(); }

    int getMaxZoomIn() { return m_maxZoomIn; }
    int getMaxZoomOut() { return m_maxZoomOut; }
    int getZoom() { return m_zoom; }

    float getMinimumAmbientLight() { return m_mapView->getMinimumAmbientLight(); }

    void setCrosshairTexture(const std::string& texturePath) { m_mapView->setCrosshairTexture(texturePath); }
    void setDrawHighlightTarget(const bool enable) { m_mapView->setDrawHighlightTarget(enable); }
    void setAntiAliasingMode(const MapView::AntialiasingMode mode) { m_mapView->setAntiAliasingMode(mode); }
    void setFloorFading(const uint16 v) { m_mapView->setFloorFading(v); }

protected:
    void onStyleApply(const std::string& styleName, const OTMLNodePtr& styleNode) override;
    void onGeometryChange(const Rect& oldRect, const Rect& newRect) override;
    bool onMouseMove(const Point& mousePos, const Point& mouseMoved) override;

private:
    void updateVisibleDimension();
    void updateMapSize();

    MapViewPtr m_mapView;
    Rect m_mapRect;
    float m_aspectRatio;

    bool m_keepAspectRatio,
        m_limitVisibleRange;

    int m_maxZoomIn,
        m_maxZoomOut,
        m_zoom;
};

#endif
