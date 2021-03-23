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

#include <framework/core/declarations.h>
#include <framework/graphics/declarations.h>
#include <framework/graphics/texturemanager.h>
#include <framework/graphics/paintershaderprogram.h>
#include <framework/luaengine/luaobject.h>
#include "lightview.h"

struct AwareRange
{
    uint8 top, right, bottom, left;
    uint8 horizontal() { return left + right + 1; }
    uint8 vertical() { return top + bottom + 1; }
};

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

protected:
    void onGlobalLightChange(const Light& light);
    void onFloorDrawingStart(const uint8 floor);
    void onFloorDrawingEnd(const uint8 floor);
    void onFloorChange(const uint8 floor, const uint8 previousFloor);
    void onTileUpdate(const Position& pos, const ThingPtr& thing, const Otc::Operation operation);
    void onMapCenterChange(const Position& pos);

    std::vector<CreaturePtr> getSightSpectators(const Position& centerPos, bool multiFloor);

    friend class Map;
    friend class Tile;
    friend class LightView;

public:
    // floor visibility related
    uint8 getLockedFirstVisibleFloor() { return m_lockedFirstVisibleFloor; }
    uint8 getCachedFirstVisibleFloor() { return m_cachedFirstVisibleFloor; }
    uint8 getCachedLastVisibleFloor() { return m_cachedLastVisibleFloor; }
    uint8 getTileSize() { return m_tileSize; }

    void lockFirstVisibleFloor(uint8 firstVisibleFloor);
    void unlockFirstVisibleFloor();

    bool isMultifloor() { return m_multifloor; }
    void setMultifloor(bool enable) { m_multifloor = enable; requestVisibleTilesCacheUpdate(); }

    // map dimension related
    Point getVisibleCenterOffset() { return m_visibleCenterOffset; }
    Size getVisibleDimension() { return m_visibleDimension; }
    void setVisibleDimension(const Size& visibleDimension);

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

    void setMinimumAmbientLight(float intensity) { m_minimumAmbientLight = intensity; updateLight(); }
    float getMinimumAmbientLight() { return m_minimumAmbientLight; }

    // drawing related
    void setDrawTexts(bool enable) { m_drawTexts = enable; }
    bool isDrawingTexts() { return m_drawTexts; }

    void setDrawNames(bool enable) { m_drawNames = enable; schedulePainting(Otc::FUpdateCreatureInformation); }
    bool isDrawingNames() { return m_drawNames; }

    void setDrawHealthBars(bool enable) { m_drawHealthBars = enable; schedulePainting(Otc::FUpdateCreatureInformation); }
    bool isDrawingHealthBars() { return m_drawHealthBars; }

    void setDrawLights(bool enable);
    bool isDrawingLights() { return m_drawLights && m_lightView->isDark(); }

    void setFloorShadowingFlag(const Otc::ShadowFloor flag) { m_lastFloorShadowingColor = Color::white; m_floorShadowingFlag = flag; requestVisibleTilesCacheUpdate(); schedulePainting(Otc::FUpdateThing); }
    bool hasFloorShadowingFlag(const Otc::ShadowFloor flag) { return m_floorShadowingFlag & flag; }
    bool hasFloorShadowingFlag() { return m_floorShadowingFlag > 0; }
    const Color getLastFloorShadowingColor() { return m_lastFloorShadowingColor; }

    void setDrawViewportEdge(bool enable) { m_drawViewportEdge = enable; schedulePainting(Otc::FUpdateThing); }
    bool isDrawingViewportEdge() { return m_drawViewportEdge; }

    void setDrawManaBar(bool enable) { m_drawManaBar = enable; schedulePainting(Otc::FUpdateCreatureInformation); }
    bool isDrawingManaBar() { return m_drawManaBar; }

    void move(int32 x, int32 y);

    void setShader(const PainterShaderProgramPtr& shader, float fadein, float fadeout);
    PainterShaderProgramPtr getShader() { return m_shader; }

    Position getPosition(const Point& point, const Size& mapSize);

    MapViewPtr asMapView() { return static_self_cast<MapView>(); }

    void schedulePainting(const Otc::FrameUpdate frameFlags, const uint16_t delay = FrameBuffer::MIN_TIME_UPDATE);
    void schedulePainting(const Position& pos, const Otc::FrameUpdate frameFlags, const uint16_t delay = FrameBuffer::MIN_TIME_UPDATE);
    void cancelScheduledPainting(const Otc::FrameUpdate frameFlags, uint16_t delay);

    void resetLastCamera() { m_lastCameraPosition = Position(); }

    std::vector<CreaturePtr>& getVisibleCreatures() { return m_visibleCreatures; }
    std::vector<CreaturePtr> getSpectators(const Position& centerPos, bool multiFloor);

    bool isInRange(const Position& pos, const bool ignoreZ = false);

    void setCrosshairTexture(const std::string& texturePath);

    void onPositionChange(const Position& newPos, const Position& oldPos);
    void onMouseMove(const Position& mousePos, const bool isVirtualMove = false);

    void setLastMousePosition(const Position& mousePos) { m_lastMousePosition = mousePos; }
    const Position& getLastMousePosition() { return m_lastMousePosition; }

    void setDrawHighlightTarget(const bool enable) { m_drawHighlightTarget = enable; }

    void setAntiAliasing(const bool enable);
    void setRenderScale(const uint8 scale);

private:
    struct ViewPort {
        uint8 top, right, bottom, left;
    };

    struct FrameCache {
        FrameBufferPtr tile, staticText,
            crosshair, creatureInformation, creatureDynamicInformation;

        uint32_t flags = 0;
    };

    struct Crosshair {
        bool positionChanged = false;
        Position position;
        TexturePtr texture;
    };

    void updateGeometry(const Size& visibleDimension, const Size& optimizedSize);
    void updateVisibleTilesCache();
    void requestVisibleTilesCacheUpdate() { m_timeUpdateVisibleTilesCache.restart();  m_mustUpdateVisibleTilesCache = true; }

    uint8 calcFirstVisibleFloor();
    uint8 calcLastVisibleFloor();

    void updateLight();
    void updateViewportDirectionCache();
    void drawCreatureInformation(const Rect& rect, Point drawOffset, const float horizontalStretchFactor, const float verticalStretchFactor);
    void drawText(const Rect& rect, Point drawOffset, const float horizontalStretchFactor, const float verticalStretchFactor);

#if DRAW_ALL_GROUND_FIRST == 1
    void drawSeparately(const uint8 floor, const ViewPort& viewPort, LightView* lightView);
#endif

    Rect calcFramebufferSource(const Size& destSize);

    Point transformPositionTo2D(const Position& position, const Position& relativePosition)
    {
        return Point((m_virtualCenterOffset.x + (position.x - relativePosition.x) - (relativePosition.z - position.z)) * m_tileSize,
                     (m_virtualCenterOffset.y + (position.y - relativePosition.y) - (relativePosition.z - position.z)) * m_tileSize);
    }

    bool canRenderTile(const TilePtr& tile, const ViewPort& viewPort, LightView* lightView);

    uint8 m_lockedFirstVisibleFloor,
        m_cachedFirstVisibleFloor,
        m_cachedLastVisibleFloor,
        m_lightVersion,
        m_renderScale,
        m_tileSize,
        m_floorMin,
        m_floorMax,
        m_floorShadowingFlag;

    float m_minimumAmbientLight,
        m_fadeInTime,
        m_fadeOutTime,
        m_scaleFactor;

    Rect m_rectDimension, m_srcRect;

    Size m_drawDimension,
        m_visibleDimension,
        m_optimizedSize;

    Point m_virtualCenterOffset,
        m_visibleCenterOffset,
        m_moveOffset;

    Position m_customCameraPosition,
        m_lastCameraPosition,
        m_lastMousePosition;

    std::array<ViewPort, Otc::InvalidDirection + 1> m_viewPortDirection;

    stdext::boolean<true>
        m_mustUpdateVisibleTilesCache,
        m_shaderSwitchDone,
        m_drawHealthBars,
        m_drawManaBar,
        m_multifloor,
        m_drawTexts,
        m_drawNames,
        m_smooth,
        m_follow,
        m_antiAliasing;

    stdext::boolean<false> m_drawLights,
        m_autoViewMode,
        m_drawViewportEdge,
        m_forceTileUpdateCache,
        m_drawHighlightTarget;

    std::vector<CreaturePtr> m_visibleCreatures;

    std::array<std::vector<TilePtr>, Otc::MAX_Z + 1> m_cachedVisibleTiles;

    PainterShaderProgramPtr m_shader, m_nextShader;
    LightViewPtr m_lightView;
    CreaturePtr m_followingCreature;

    FrameCache m_frameCache;
    Crosshair m_crosshair;
    ViewMode m_viewMode;

    Color m_lastFloorShadowingColor;

    Timer m_fadeTimer, m_timeUpdateVisibleTilesCache;

    AwareRange m_awareRange;

    TilePtr m_lastHighlightTile;
};

#endif
