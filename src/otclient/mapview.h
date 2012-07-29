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

#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "declarations.h"
#include <framework/graphics/paintershaderprogram.h>
#include <framework/graphics/declarations.h>
#include <framework/luaengine/luaobject.h>
#include <framework/core/declarations.h>

// @bindclass
class MapView : public LuaObject
{
    enum {
        // 3840x2160 => 1080p optimized
        // 2560x1440 => 720p optimized
        // 1728x972 => 480p optimized

        NEAR_VIEW_AREA = 32*32,
        MID_VIEW_AREA = 64*64,
        FAR_VIEW_AREA = 128*128,
        MAX_TILE_DRAWS = NEAR_VIEW_AREA*7
    };

public:
    enum ViewMode {
        NEAR_VIEW,
        MID_VIEW,
        FAR_VIEW,
        HUGE_VIEW
    };

    MapView();
    ~MapView();
    void draw(const Rect& rect);

private:
    void updateGeometry(const Size& visibleDimension, const Size& optimizedSize);
    void updateVisibleTilesCache(int start = 0);
    void requestVisibleTilesCacheUpdate() { m_mustUpdateVisibleTilesCache = true; }

protected:
    void onTileUpdate(const Position& pos);
    void onMapCenterChange(const Position& pos);

    friend class Map;

public:
    // floor visibility related
    void lockFirstVisibleFloor(int firstVisibleFloor);
    void unlockFirstVisibleFloor();
    int getLockedFirstVisibleFloor() { return m_lockedFirstVisibleFloor; }

    void setMultifloor(bool enable) { m_multifloor = enable; requestVisibleTilesCacheUpdate(); }
    bool isMultifloor() { return m_multifloor; }

    // map dimension related
    void setVisibleDimension(const Size& visibleDimension);
    Size getVisibleDimension() { return m_visibleDimension; }

    // view mode related
    void setViewMode(ViewMode viewMode);
    ViewMode getViewMode() { return m_viewMode; }
    void optimizeForSize(const Size& visibleSize);

    void setAutoViewMode(bool enable);
    bool isAutoViewModeEnabled() { return m_autoViewMode; }

    // camera related
    void followCreature(const CreaturePtr& creature);
    CreaturePtr getFollowingCreature() { return m_followingCreature; }
    bool isFollowingCreature() { return m_followingCreature && m_follow; }

    void setCameraPosition(const Position& pos);
    Position getCameraPosition();

    // drawing related
    void setDrawFlags(Otc::DrawFlags drawFlags) { m_drawFlags = drawFlags; requestVisibleTilesCacheUpdate(); }
    Otc::DrawFlags getDrawFlags() { return m_drawFlags; }

    void setDrawTexts(bool enable) { m_drawTexts = enable; }
    bool isDrawingTexts() { return m_drawTexts; }

    void setDrawMinimapColors(bool enable);
    bool isDrawingMinimapColors() { return m_drawMinimapColors; }

    void setAnimated(bool animated) { m_animated = animated; requestVisibleTilesCacheUpdate(); }
    bool isAnimating() { return m_animated; }

    void setShader(const PainterShaderProgramPtr& shader) { m_shader = shader; }
    PainterShaderProgramPtr getShader() { return m_shader; }

    // get tile
    TilePtr getTile(const Point& mousePos, const Rect& mapRect);

    MapViewPtr asMapView() { return self_cast<MapView>(); }

private:
    int calcFirstVisibleFloor();
    int calcLastVisibleFloor();
    Point transformPositionTo2D(const Position& position, const Position& relativePosition) {
        return Point((m_virtualCenterOffset.x + (position.x - relativePosition.x) - (relativePosition.z - position.z)) * m_tileSize,
                     (m_virtualCenterOffset.y + (position.y - relativePosition.y) - (relativePosition.z - position.z)) * m_tileSize);
    }

    int m_lockedFirstVisibleFloor;
    int m_cachedFirstVisibleFloor;
    int m_cachedLastVisibleFloor;
    int m_tileSize;
    int m_updateTilesPos;
    Size m_drawDimension;
    Size m_visibleDimension;
    Size m_optimizedSize;
    Point m_virtualCenterOffset;
    Point m_visibleCenterOffset;
    Position m_customCameraPosition;
    Boolean<true> m_mustUpdateVisibleTilesCache;
    Boolean<true> m_mustDrawVisibleTilesCache;
    Boolean<true> m_mustCleanFramebuffer;
    Boolean<true> m_multifloor;
    Boolean<true> m_animated;
    Boolean<true> m_autoViewMode;
    Boolean<true> m_drawTexts;
    Boolean<true> m_smooth;
    Boolean<false> m_drawMinimapColors;
    Boolean<true> m_follow;
    std::vector<TilePtr> m_cachedVisibleTiles;
    std::vector<CreaturePtr> m_cachedFloorVisibleCreatures;
    CreaturePtr m_followingCreature;
    FrameBufferPtr m_framebuffer;
    PainterShaderProgramPtr m_shader;
    ViewMode m_viewMode;
    Otc::DrawFlags m_drawFlags;
    std::vector<Point> m_spiral;
};

#endif
