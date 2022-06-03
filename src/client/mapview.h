/*
 * Copyright (c) 2010-2022 OTClient <https://github.com/edubart/otclient>
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

#pragma once

#include "lightview.h"
#include <framework/core/inputevent.h>
#include <framework/graphics/declarations.h>
#include <framework/graphics/paintershaderprogram.h>
#include <framework/luaengine/luaobject.h>

struct AwareRange
{
    uint8 left, top, right, bottom;
    uint8 horizontal() { return left + right + 1; }
    uint8 vertical() { return top + bottom + 1; }
};

// @bindclass
class MapView : public LuaObject
{
public:
    enum FloorViewMode
    {
        NORMAL,
        FADE,
        LOCKED,
        ALWAYS,
        ALWAYS_WITH_TRANSPARENCY
    };

    enum AntialiasingMode :uint8
    {
        ANTIALIASING_DISABLED,
        ANTIALIASING_ENABLED,
        ANTIALIASING_SMOOTH_RETRO
    };

    MapView();
    ~MapView() override;
    void draw(const Rect& rect);

    // floor visibility related
    uint8 getLockedFirstVisibleFloor() { return m_lockedFirstVisibleFloor; }
    uint8 getCachedFirstVisibleFloor() { return m_cachedFirstVisibleFloor; }
    uint8 getCachedLastVisibleFloor() { return m_cachedLastVisibleFloor; }
    uint8 getTileSize() { return m_tileSize; }

    void lockFirstVisibleFloor(uint8 firstVisibleFloor);
    void unlockFirstVisibleFloor();

    // map dimension related
    Size getVisibleDimension() { return m_visibleDimension; }
    void setVisibleDimension(const Size& visibleDimension);

    // view mode related
    FloorViewMode getFloorViewMode() { return m_floorViewMode; }
    void setFloorViewMode(FloorViewMode viewMode);

    // camera related
    CreaturePtr getFollowingCreature() { return m_followingCreature; }
    void followCreature(const CreaturePtr& creature);
    bool isFollowingCreature() { return m_followingCreature && m_follow; }

    Position getCameraPosition();
    void setCameraPosition(const Position& pos);

    void setMinimumAmbientLight(float intensity) { m_minimumAmbientLight = intensity; updateLight(); }
    float getMinimumAmbientLight() { return m_minimumAmbientLight; }

    void setShadowFloorIntensity(float intensity) { m_shadowFloorIntensity = intensity; updateLight(); }
    float getShadowFloorIntensity() { return m_shadowFloorIntensity; }

    // drawing related
    void setDrawTexts(bool enable) { m_drawTexts = enable; }
    bool isDrawingTexts() { return m_drawTexts; }

    void setDrawNames(bool enable) { m_drawNames = enable; }
    bool isDrawingNames() { return m_drawNames; }

    void setDrawHealthBars(bool enable) { m_drawHealthBars = enable; }
    bool isDrawingHealthBars() { return m_drawHealthBars; }

    void setDrawLights(bool enable);
    bool isDrawingLights() { return m_drawLights && m_lightView->isDark(); }

    void setDrawViewportEdge(bool enable) { m_drawViewportEdge = enable; }
    bool isDrawingViewportEdge() { return m_drawViewportEdge; }

    void setDrawManaBar(bool enable) { m_drawManaBar = enable; }
    bool isDrawingManaBar() { return m_drawManaBar; }

    void move(int32 x, int32 y);

    void setShader(const PainterShaderProgramPtr& shader, float fadein, float fadeout);
    PainterShaderProgramPtr getShader() { return m_shader; }

    Position getPosition(const Point& point, const Size& mapSize);

    MapViewPtr asMapView() { return static_self_cast<MapView>(); }

    void resetLastCamera() { m_lastCameraPosition = {}; }

    std::vector<CreaturePtr>& getVisibleCreatures() { return m_visibleCreatures; }
    std::vector<CreaturePtr> getSpectators(const Position& centerPos, bool multiFloor);
    std::vector<CreaturePtr> getSightSpectators(const Position& centerPos, bool multiFloor);

    bool isInRange(const Position& pos, bool ignoreZ = false);
    bool isInRangeEx(const Position& pos, bool ignoreZ = false);

    TilePtr getTopTile(Position tilePos);

    void setCrosshairTexture(const std::string_view texturePath);
    void setAntiAliasingMode(AntialiasingMode mode);

    void onMouseMove(const Position& mousePos, bool isVirtualMove = false);
    void onKeyRelease(const InputEvent& inputEvent);

    void setLastMousePosition(const Position& mousePos) { m_mousePosition = mousePos; }
    const Position& getLastMousePosition() { return m_mousePosition; }

    void setDrawHighlightTarget(const bool enable) { m_drawHighlightTarget = enable; }

    void setFloorFading(uint16 value) { m_floorFading = value; }

protected:
    void onGlobalLightChange(const Light& light);
    void onFloorChange(uint8 floor, uint8 previousFloor);
    void onTileUpdate(const Position& pos, const ThingPtr& thing, Otc::Operation operation);
    void onMapCenterChange(const Position& newPos, const Position& oldPos);
    void onCameraMove(const Point& offset);

    friend class Map;
    friend class Tile;
    friend class LightView;

private:
    struct MapList
    {
        std::vector<TilePtr> shades, grounds, surfaces, effects;
        void clear() { shades.clear(); grounds.clear(); surfaces.clear(); effects.clear(); }
    };

    struct Crosshair
    {
        bool positionChanged = false;
        Position position;
        TexturePtr texture;
    };

    struct RectCache
    {
        Rect rect, srcRect;
        Point drawOffset;
        float horizontalStretchFactor, verticalStretchFactor;
    };

    void updateGeometry(const Size& visibleDimension);
    void updateVisibleTilesCache();
    void requestVisibleTilesCacheUpdate() { m_mustUpdateVisibleTilesCache = true; }

    uint8 calcFirstVisibleFloor(bool checkLimitsFloorsView);
    uint8 calcLastVisibleFloor();

    void updateLight();
    void updateViewportDirectionCache();
    void drawFloor();
    void drawCreatureInformation();
    void drawText();

    void updateViewport(const Otc::Direction dir = Otc::InvalidDirection) { m_viewport = m_viewPortDirection[dir]; }

    bool canFloorFade() { return m_floorViewMode == FADE && m_floorFading; }

    float getFadeLevel(uint8 z)
    {
        float fading = std::clamp<float>(static_cast<float>(m_fadingFloorTimers[z].elapsed_millis()) / static_cast<float>(m_floorFading), 0.f, 1.f);
        if (z < m_cachedFirstVisibleFloor)
            fading = 1.0 - fading;
        return fading;
    }

    Rect calcFramebufferSource(const Size& destSize);

    Point transformPositionTo2D(const Position& position, const Position& relativePosition)
    {
        return {
            (m_virtualCenterOffset.x + (position.x - relativePosition.x) - (relativePosition.z - position.z)) * m_tileSize,
                     (m_virtualCenterOffset.y + (position.y - relativePosition.y) - (relativePosition.z - position.z)) * m_tileSize
        };
    }

    int8 m_lockedFirstVisibleFloor{ -1 };

    uint8 m_cachedFirstVisibleFloor{ SEA_FLOOR },
        m_cachedLastVisibleFloor{ SEA_FLOOR },
        m_tileSize{ SPRITE_SIZE },
        m_floorMin{ 0 },
        m_floorMax{ 0 },
        m_antiAliasingMode;

    uint16 m_floorFading = 500;

    float m_minimumAmbientLight{ 0 },
        m_fadeInTime{ 0 },
        m_fadeOutTime{ 0 },
        m_shadowFloorIntensity{ 0 },
        m_scaleFactor{ 1.f };

    Rect m_rectDimension;

    Size m_drawDimension, m_visibleDimension;

    Point m_virtualCenterOffset, m_moveOffset;

    Position m_customCameraPosition,
        m_lastCameraPosition,
        m_mousePosition;

    std::array<AwareRange, Otc::InvalidDirection + 1> m_viewPortDirection;
    AwareRange m_viewport;

    bool
        m_mustUpdateVisibleTilesCache{ true },
        m_mustUpdateVisibleCreaturesCache{ true },
        m_shaderSwitchDone{ true },
        m_drawHealthBars{ true },
        m_drawManaBar{ true },
        m_drawTexts{ true },
        m_drawNames{ true },
        m_smooth{ true },
        m_follow{ true };

    bool m_drawLights{ false },
        m_autoViewMode{ false },
        m_drawViewportEdge{ false },
        m_drawHighlightTarget{ false },
        m_shiftPressed{ false };

    std::vector<CreaturePtr> m_visibleCreatures;

    std::array<MapList, MAX_Z + 1> m_cachedVisibleTiles;

    stdext::timer m_fadingFloorTimers[MAX_Z + 1];

    PainterShaderProgramPtr m_shader, m_nextShader;
    LightViewPtr m_lightView;
    CreaturePtr m_followingCreature;

    RectCache m_rectCache;
    FloorViewMode m_floorViewMode{ NORMAL };

    Timer m_fadeTimer;

    AwareRange m_awareRange;

    TilePtr m_lastHighlightTile;
    TexturePtr m_crosshairTexture;
    EffectPtr m_crosshairEffect;
};
