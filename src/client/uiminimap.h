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

#ifndef UIMINIMAP_H
#define UIMINIMAP_H

#include "declarations.h"
#include <framework/ui/uiwidget.h>

class UIMinimap : public UIWidget
{
public:
    UIMinimap();

    void drawSelf(Fw::DrawPane drawPane);

    bool zoomIn() { return setZoom(m_zoom+1); }
    bool zoomOut() { return setZoom(m_zoom-1); }

    bool setZoom(int zoom);
    void setMinZoom(int minZoom) { m_minZoom = minZoom; }
    void setMaxZoom(int maxZoom) { m_maxZoom = maxZoom; }
    void setCameraPosition(const Position& pos);
    bool floorUp();
    bool floorDown();

    Point getTilePoint(const Position& pos);
    Rect getTileRect(const Position& pos);
    Position getTilePosition(const Point& mousePos);

    Position getCameraPosition() { return m_cameraPosition; }
    int getMinZoom() { return m_minZoom; }
    int getMaxZoom() { return m_maxZoom; }
    int getZoom() { return m_zoom; }
    float getScale() { return m_scale; }

    void anchorPosition(const UIWidgetPtr& anchoredWidget, Fw::AnchorEdge anchoredEdge, const Position& hookedPosition, Fw::AnchorEdge hookedEdge);
    void fillPosition(const UIWidgetPtr& anchoredWidget, const Position& hookedPosition);
    void centerInPosition(const UIWidgetPtr& anchoredWidget, const Position& hookedPosition);

protected:
    virtual void onZoomChange(int zoom, int oldZoom);
    virtual void onCameraPositionChange(const Position& position, const Position& oldPosition);
    virtual void onStyleApply(const std::string& styleName, const OTMLNodePtr& styleNode);

private:
    void update();

    Rect m_mapArea;
    Position m_cameraPosition;
    float m_scale;
    int m_zoom;
    int m_minZoom;
    int m_maxZoom;
};

#endif
