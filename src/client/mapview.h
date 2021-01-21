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
    int top;
    int right;
    int bottom;
    int left;

    int horizontal() { return left + right + 1; }
    int vertical() { return top + bottom + 1; }
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

private:
    void updateGeometry(const Size& visibleDimension, const Size& optimizedSize);
    void updateVisibleTilesCache();
    void requestVisibleTilesCacheUpdate() { m_timeUpdateVisibleTilesCache.restart();  m_mustUpdateVisibleTilesCache = true; }

protected:
    void onGlobalLightChange(const Light& light);
    void onFloorDrawingStart(const short floor);
    void onFloorDrawingEnd(const short floor);
    void onFloorChange(const short floor, const short previousFloor);
    void onTileUpdate(const Position& pos, const ThingPtr& thing, const Otc::Operation operation);
    void onMapCenterChange(const Position& pos);

    std::vector<CreaturePtr> getSightSpectators(const Position& centerPos, bool multiFloor);

    friend class Map;
    friend class Tile;
    friend class LightView;

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

    void setDrawFloorShadowing(bool enable) { m_lastFloorShadowingColor = Color::white; m_drawFloorShadowing = enable; schedulePainting(Otc::FUpdateThing); }
    bool isDrawingFloorShadowing() { return m_drawFloorShadowing; }
    const Color getLastFloorShadowingColor() { return m_lastFloorShadowingColor; }

    void setDrawViewportEdge(bool enable) { m_drawViewportEdge = enable; schedulePainting(Otc::FUpdateThing); }
    bool isDrawingViewportEdge() { return m_drawViewportEdge; }

    void setDrawManaBar(bool enable) { m_drawManaBar = enable; schedulePainting(Otc::FUpdateCreatureInformation); }
    bool isDrawingManaBar() { return m_drawManaBar; }

    void setLightVersion(int version) { m_lightVersion = version; setDrawLights(false); setDrawLights(true); }
    bool getLightVersion() { return m_lightVersion; }

    void move(int x, int y);

    void setAddLightMethod(bool add) { m_lightView->setBlendEquation(add ? Painter::BlendEquation_Add : Painter::BlendEquation_Max); }

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

    void setCrosshairPosition(const Position& pos);
    void setCrosshairTexture(const std::string& texturePath);

private:
    struct ViewPort {
        int top, right, bottom, left;
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

    int calcFirstVisibleFloor();
    int calcLastVisibleFloor();

    void updateLight();
    void updateViewportDirectionCache();

#if DRAW_ALL_GROUND_FIRST == 1
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
    stdext::boolean<true> m_shaderSwitchDone;
    stdext::boolean<true> m_drawFloorShadowing;
    stdext::boolean<true> m_drawHealthBars;
    stdext::boolean<true> m_drawManaBar;
    stdext::boolean<true> m_multifloor;
    stdext::boolean<true> m_drawTexts;
    stdext::boolean<true> m_drawNames;
    stdext::boolean<true> m_smooth;
    stdext::boolean<true> m_follow;

    stdext::boolean<false> m_drawLights;
    stdext::boolean<false> m_autoViewMode;
    stdext::boolean<false> m_drawViewportEdge;

    std::vector<CreaturePtr> m_visibleCreatures;

    std::array<std::vector<TilePtr>, Otc::MAX_Z + 1> m_cachedVisibleTiles;

    PainterShaderProgramPtr m_shader;
    PainterShaderProgramPtr m_nextShader;

    CreaturePtr m_followingCreature;

    FrameCache m_frameCache;
    Crosshair m_crosshair;

    ViewMode m_viewMode;
    LightViewPtr m_lightView;

    float m_minimumAmbientLight;
    float m_fadeInTime;
    float m_fadeOutTime;
    float m_scaleFactor;

    Color m_lastFloorShadowingColor;

    Timer m_fadeTimer, m_timeUpdateVisibleTilesCache;

    uint_fast8_t m_floorMin, m_floorMax;

    Position m_lastCameraPosition;

    AwareRange m_awareRange;

    Rect m_srcRect;

    int m_lightVersion;
};

#endif
