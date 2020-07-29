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

#ifndef MAPVIEW_H
#define MAPVIEW_H

 // Define 1, to draw separately. (Ground first => remainder after)
#define DRAW_SEPARATELY 0

 // Define 1, to draw creature information after light.
#define DRAW_CREATURE_INFORMATION_AFTER_LIGHT 0

#include <framework/core/declarations.h>
#include <framework/graphics/declarations.h>
#include <framework/graphics/paintershaderprogram.h>
#include <framework/luaengine/luaobject.h>
#include "lightview.h"

 // @bindclass
class MapView : public LuaObject
{
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
    void updateVisibleTilesCache();
    void requestVisibleTilesCacheUpdate() { m_mustUpdateVisibleTilesCache = true; }

protected:
    void onTileUpdate(const Position& pos, const ThingPtr& thing, const Otc::Operation operation);
    void onMapCenterChange(const Position& pos);

    friend class Map;

public:
    // floor visibility related
    int getLockedFirstVisibleFloor() { return m_lockedFirstVisibleFloor; }
    void lockFirstVisibleFloor(int firstVisibleFloor);
    void unlockFirstVisibleFloor();

    bool isMultifloor() { return m_multifloor; }
    void setMultifloor(bool enable) { m_multifloor = enable; requestVisibleTilesCacheUpdate(); }

    // map dimension related
    Point getVisibleCenterOffset() { return m_visibleCenterOffset; }
    Size getVisibleDimension() { return m_visibleDimension; }
    void setVisibleDimension(const Size& visibleDimension);
    int getTileSize() { return m_tileSize; }
    int getCachedFirstVisibleFloor() { return m_cachedFirstVisibleFloor; }
    int getCachedLastVisibleFloor() { return m_cachedLastVisibleFloor; }

    // view mode related
    ViewMode getViewMode() { return m_viewMode; }
    void setViewMode(ViewMode viewMode);
    void optimizeForSize(const Size& visibleSize);

    void setAutoViewMode(bool enable);
    bool isAutoViewModeEnabled() { return m_autoViewMode; }

    // camera related
    CreaturePtr getFollowingCreature() { return m_followingCreature; }
    void followCreature(const CreaturePtr& creature);
    bool isFollowingCreature() { return m_followingCreature && m_follow; }

    Position getCameraPosition();
    void setCameraPosition(const Position& pos);

    void setMinimumAmbientLight(float intensity) { m_minimumAmbientLight = intensity; requestDrawing(Position(), Otc::ReDrawLight, true); }
    float getMinimumAmbientLight() { return m_minimumAmbientLight; }

    // drawing related
    void setDrawTexts(bool enable) { m_drawTexts = enable; }
    bool isDrawingTexts() { return m_drawTexts; }

    void setDrawNames(bool enable) { m_drawNames = enable; requestDrawing(Position(), Otc::ReDrawAllInformation, true); }
    bool isDrawingNames() { return m_drawNames; }

    void setDrawHealthBars(bool enable) { m_drawHealthBars = enable; requestDrawing(Position(), Otc::ReDrawAllInformation, true); }
    bool isDrawingHealthBars() { return m_drawHealthBars; }

    void setDrawLights(bool enable);
    bool isDrawingLights() { return m_drawLights; }

    void setDrawManaBar(bool enable) { m_drawManaBar = enable; requestDrawing(Position(), Otc::ReDrawAllInformation, true); }
    bool isDrawingManaBar() { return m_drawManaBar; }

    void move(int x, int y);

    void setAddLightMethod(bool add) { m_lightView->setBlendEquation(add ? Painter::BlendEquation_Add : Painter::BlendEquation_Max); }

    void setShader(const PainterShaderProgramPtr& shader, float fadein, float fadeout);
    PainterShaderProgramPtr getShader() { return m_shader; }

    Position getPosition(const Point& point, const Size& mapSize);

    MapViewPtr asMapView() { return static_self_cast<MapView>(); }

    void requestDrawing(const Position& pos, const Otc::RequestDrawFlags reDrawFlags, const bool force = false, const bool isLocalPlayer = false);
    void resetLastCamera() { m_lastCameraPosition = Position(); }

private:
    struct ViewPort {
        int top, right, bottom, left;
    };

    struct FrameCache {
        FrameBufferPtr tile, staticText,
            creatureInformation;
    };

    int calcFirstVisibleFloor();
    int calcLastVisibleFloor();

    void initViewPortDirection();

#if DRAW_SEPARATELY == 1
    void drawSeparately(const int floor, const ViewPort& viewPort, LightView* lightView);
#endif

    void drawCreatureInformation(const Rect& rect, Point drawOffset, const float horizontalStretchFactor, const float verticalStretchFactor);
    void drawText(const Rect& rect, Point drawOffset, const float horizontalStretchFactor, const float verticalStretchFactor);

    Rect calcFramebufferSource(const Size& destSize);

    Point transformPositionTo2D(const Position& position, const Position& relativePosition)
    {
        return Point((m_virtualCenterOffset.x + (position.x - relativePosition.x) - (relativePosition.z - position.z)) * m_tileSize,
                     (m_virtualCenterOffset.y + (position.y - relativePosition.y) - (relativePosition.z - position.z)) * m_tileSize);
    }

    bool isInRange(const Position& pos);
    bool canRenderTile(const TilePtr& tile, const ViewPort& viewPort, LightView* lightView);

    int m_lockedFirstVisibleFloor;
    int m_cachedFirstVisibleFloor;
    int m_cachedLastVisibleFloor;
    int m_tileSize;

    Rect m_rectDimension;

    Size m_drawDimension;
    Size m_visibleDimension;
    Size m_optimizedSize;

    Point m_virtualCenterOffset;
    Point m_visibleCenterOffset;
    Point m_moveOffset;

    Position m_customCameraPosition;

    std::array<ViewPort, Otc::InvalidDirection + 1> m_viewPortDirection;

    stdext::boolean<true> m_mustUpdateVisibleTilesCache;
    stdext::boolean<true> m_mustCleanFramebuffer;
    stdext::boolean<true> m_shaderSwitchDone;
    stdext::boolean<true> m_drawHealthBars;
    stdext::boolean<true> m_autoViewMode;
    stdext::boolean<true> m_drawManaBar;
    stdext::boolean<true> m_multifloor;
    stdext::boolean<true> m_drawTexts;
    stdext::boolean<true> m_drawNames;
    stdext::boolean<true> m_smooth;
    stdext::boolean<true> m_follow;

    stdext::boolean<false> m_drawLights;

    std::vector<CreaturePtr> m_visibleCreatures;

    std::array<std::vector<TilePtr>, Otc::MAX_Z + 1> m_cachedVisibleTiles;

    PainterShaderProgramPtr m_shader;
    PainterShaderProgramPtr m_nextShader;

    CreaturePtr m_followingCreature;

    FrameCache m_frameCache;

    ViewMode m_viewMode;
    LightViewPtr m_lightView;

    float m_minimumAmbientLight;
    float m_fadeInTime;
    float m_fadeOutTime;
    float m_scaleFactor;

    uint32 m_redrawFlag;

    Timer m_minTimeRender;
    Timer m_fadeTimer;

    uint_fast8_t m_floorMin, m_floorMax;

    Position m_lastCameraPosition;
};

#endif
