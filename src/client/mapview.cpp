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

#include "mapview.h"

#include "animatedtext.h"
#include "creature.h"
#include "game.h"
#include "lightview.h"
#include "map.h"
#include "missile.h"
#include "shadermanager.h"
#include "statictext.h"
#include "tile.h"

#include <framework/core/application.h>
#include <framework/core/eventdispatcher.h>
#include <framework/core/resourcemanager.h>
#include <framework/graphics/framebuffermanager.h>
#include <framework/graphics/graphics.h>
#include <framework/graphics/image.h>


enum {
    // 3840x2160 => 1080p optimized
    // 2560x1440 => 720p optimized
    // 1728x972 => 480p optimized

    NEAR_VIEW_AREA = 32 * 32,
    MID_VIEW_AREA = 64 * 64,
    FAR_VIEW_AREA = 128 * 128,
    MAX_TILE_DRAWS = NEAR_VIEW_AREA * 7
};

MapView::MapView()
{
    m_viewMode = NEAR_VIEW;
    m_lockedFirstVisibleFloor = -1;
    m_cachedFirstVisibleFloor = 7;
    m_cachedLastVisibleFloor = 7;
    m_updateTilesPos = 0;
    m_fadeOutTime = 0;
    m_fadeInTime = 0;
    m_redrawFlag = Otc::RedrawAll;
    m_minimumAmbientLight = 0;
    m_optimizedSize = Size(g_map.getAwareRange().horizontal(), g_map.getAwareRange().vertical()) * Otc::TILE_PIXELS;

    m_framebuffer = g_framebuffers.createFrameBuffer();
    m_nameFramebuffer = g_framebuffers.createFrameBuffer();
    setVisibleDimension(Size(15, 11));

    m_shader = g_shaders.getDefaultMapShader();

    m_floorMin = m_floorMax = 0;

    for(int dir = Otc::North; dir < Otc::InvalidDirection; ++dir) {
        const ViewportControl viewport(static_cast<Otc::Direction>(dir));
        m_viewportControl[dir] = viewport;
    }
}

MapView::~MapView()
{
#ifndef NDEBUG
    assert(!g_app.isTerminated());
#endif
}

void MapView::draw(const Rect& rect)
{
    // update visible tiles cache when needed
    if(m_mustUpdateVisibleTilesCache || m_updateTilesPos > 0)
        updateVisibleTilesCache(m_mustUpdateVisibleTilesCache ? 0 : m_updateTilesPos);

    const float scaleFactor = m_tileSize / static_cast<float>(Otc::TILE_PIXELS);
    const Position cameraPosition = getCameraPosition();

    const auto redrawTile = m_redrawFlag & Otc::ReDrawTile;
    if(redrawTile) {
        m_framebuffer->bind();

        if(m_mustCleanFramebuffer) {
            const Rect clearRect = Rect(0, 0, m_drawDimension * m_tileSize);
            g_painter->setColor(Color::black);
            g_painter->drawFilledRect(clearRect);

            if(m_drawLights) {
                m_lightView->reset();
                m_lightView->resize(m_framebuffer->getSize());

                Light ambientLight;
                if(cameraPosition.z <= Otc::SEA_FLOOR) {
                    ambientLight = g_map.getLight();
                } else {
                    ambientLight.color = 215;
                    ambientLight.intensity = 0;
                }
                ambientLight.intensity = std::max<int>(m_minimumAmbientLight * 255, ambientLight.intensity);
                m_lightView->setGlobalLight(ambientLight);
            }
        }
        g_painter->setColor(Color::white);

        const LocalPlayerPtr player = g_game.getLocalPlayer();
        const bool isWalking = player->isWalking() || player->isPreWalking() || player->isServerWalking();

        const auto& viewport = isWalking ? m_viewportControl[player->getDirection()] : m_viewportControl[Otc::InvalidDirection];

        for(int_fast8_t z = m_floorMax; z >= m_floorMin; --z) {
            for(const auto& tile : m_cachedVisibleTiles[z]) {
                if(!viewport.isValid(tile, cameraPosition)) continue;

                const Position tilePos = tile->getPosition();

                tile->draw(transformPositionTo2D(tilePos, cameraPosition), scaleFactor, g_map.isCovered(tilePos, m_floorMin) ? nullptr : m_lightView.get());
            }

            for(const MissilePtr& missile : g_map.getFloorMissiles(z)) {
                missile->draw(transformPositionTo2D(missile->getPosition(), cameraPosition), scaleFactor, m_lightView.get());
            }
        }

        m_framebuffer->release();

        m_minTimeRender.restart();
        m_redrawFlag &= ~Otc::ReDrawTile;
    }


    // generating mipmaps each frame can be slow in older cards
    //m_framebuffer->getTexture()->buildHardwareMipmaps();

    float fadeOpacity = 1.0f;
    if(!m_shaderSwitchDone && m_fadeOutTime > 0) {
        fadeOpacity = 1.0f - (m_fadeTimer.timeElapsed() / m_fadeOutTime);
        if(fadeOpacity < 0.0f) {
            m_shader = m_nextShader;
            m_nextShader = nullptr;
            m_shaderSwitchDone = true;
            m_fadeTimer.restart();
        }
    }

    if(m_shaderSwitchDone && m_shader && m_fadeInTime > 0)
        fadeOpacity = std::min<float>(m_fadeTimer.timeElapsed() / m_fadeInTime, 1.0f);

    const Rect srcRect = calcFramebufferSource(rect.size());
    const Point drawOffset = srcRect.topLeft();

    if(m_shader && g_painter->hasShaders() && g_graphics.shouldUseShaders() && m_viewMode == NEAR_VIEW) {
        const Rect framebufferRect = Rect(0, 0, m_drawDimension * m_tileSize);
        const Point center = srcRect.center();
        const Point globalCoord = Point(cameraPosition.x - m_drawDimension.width() / 2, -(cameraPosition.y - m_drawDimension.height() / 2)) * m_tileSize;
        m_shader->bind();
        m_shader->setUniformValue(ShaderManager::MAP_CENTER_COORD, center.x / static_cast<float>(framebufferRect.width()), 1.0f - center.y / static_cast<float>(framebufferRect.height()));
        m_shader->setUniformValue(ShaderManager::MAP_GLOBAL_COORD, globalCoord.x / static_cast<float>(framebufferRect.height()), globalCoord.y / static_cast<float>(framebufferRect.height()));
        m_shader->setUniformValue(ShaderManager::MAP_ZOOM, scaleFactor);
        g_painter->setShaderProgram(m_shader);
    }

    g_painter->setColor(Color::white);
    g_painter->setOpacity(fadeOpacity);
    glDisable(GL_BLEND);
#if 0
    // debug source area
    g_painter->saveAndResetState();
    m_framebuffer->bind();
    g_painter->setColor(Color::green);
    g_painter->drawBoundingRect(srcRect, 2);
    m_framebuffer->release();
    g_painter->restoreSavedState();
    m_framebuffer->draw(rect);
#else
    m_framebuffer->draw(rect, srcRect);
#endif
    g_painter->resetShaderProgram();
    g_painter->resetOpacity();
    glEnable(GL_BLEND);


    // this could happen if the player position is not known yet
    if(!cameraPosition.isValid())
        return;

    const float horizontalStretchFactor = rect.width() / static_cast<float>(srcRect.width());
    const float verticalStretchFactor = rect.height() / static_cast<float>(srcRect.height());

    // avoid drawing texts on map in far zoom outs
    if(m_viewMode == NEAR_VIEW) {
        if(redrawTile || m_redrawFlag & Otc::ReDrawInformation) {
            m_nameFramebuffer->bind();
            g_painter->setAlphaWriting(true);
            g_painter->clear(Color::alpha);
            for(const CreaturePtr& creature : m_cachedFloorVisibleCreatures) {
                if(!creature->canBeSeen())
                    continue;

                const PointF jumpOffset = creature->getJumpOffset() * scaleFactor;
                Point creatureOffset = Point(16 - creature->getDisplacementX(), -creature->getDisplacementY() - 2);
                Position pos = creature->getPosition();
                Point p = transformPositionTo2D(pos, cameraPosition) - drawOffset;
                p += (creature->getDrawOffset() + creatureOffset) * scaleFactor - Point(stdext::round(jumpOffset.x), stdext::round(jumpOffset.y));
                p.x = p.x * horizontalStretchFactor;
                p.y = p.y * verticalStretchFactor;
                p += rect.topLeft();

                int flags = 0;
                if(m_drawNames) { flags = Otc::DrawNames; }
                if(m_drawHealthBars) { flags |= Otc::DrawBars; }
                if(m_drawManaBar) { flags |= Otc::DrawManaBar; }
                creature->drawInformation(p, g_map.isCovered(pos, m_floorMin), rect, flags);
            }
            m_nameFramebuffer->release();

            m_redrawFlag &= ~Otc::ReDrawInformation;
        }
        m_nameFramebuffer->draw();
    }

    // lights are drawn after names and before texts
    if(m_drawLights)
        m_lightView->draw(rect, srcRect);

    if(m_viewMode == NEAR_VIEW && m_drawTexts) {
        for(const StaticTextPtr& staticText : g_map.getStaticTexts()) {
            Position pos = staticText->getPosition();

            // ony draw static texts from current camera floor, unless yells
            //if(pos.z != cameraPosition.z && !staticText->isYell())
            //    continue;

            if(pos.z != cameraPosition.z && staticText->getMessageMode() == Otc::MessageNone)
                continue;

            Point p = transformPositionTo2D(pos, cameraPosition) - drawOffset;
            p.x = p.x * horizontalStretchFactor;
            p.y = p.y * verticalStretchFactor;
            p += rect.topLeft();
            staticText->drawText(p, rect);
        }

        for(const AnimatedTextPtr& animatedText : g_map.getAnimatedTexts()) {
            Position pos = animatedText->getPosition();

            /*
            // only draw animated texts from visible floors
            if(pos.z < m_cachedFirstVisibleFloor || pos.z > m_cachedLastVisibleFloor)
                continue;

            // dont draw animated texts from covered tiles
            if(pos.z != cameraPosition.z && g_map.isCovered(pos, m_cachedFirstVisibleFloor))
                continue;
            */
            if(pos.z != cameraPosition.z)
                continue;

            Point p = transformPositionTo2D(pos, cameraPosition) - drawOffset;
            p.x *= horizontalStretchFactor;
            p.y *= verticalStretchFactor;
            p += rect.topLeft();
            animatedText->drawText(p, rect);
        }
    }
}

void MapView::updateVisibleTilesCache(int start)
{
    if(start != 0) {
        m_mustCleanFramebuffer = false;
    } else {
        m_cachedFirstVisibleFloor = calcFirstVisibleFloor();
        m_cachedLastVisibleFloor = calcLastVisibleFloor();

        assert(m_cachedFirstVisibleFloor >= 0 && m_cachedLastVisibleFloor >= 0 &&
               m_cachedFirstVisibleFloor <= Otc::MAX_Z && m_cachedLastVisibleFloor <= Otc::MAX_Z);

        if(m_cachedLastVisibleFloor < m_cachedFirstVisibleFloor)
            m_cachedLastVisibleFloor = m_cachedFirstVisibleFloor;

        m_cachedFloorVisibleCreatures.clear();

        m_mustCleanFramebuffer = true;
    }

    // clear current visible tiles cache
    do {
        m_cachedVisibleTiles[m_floorMin].clear();
    } while(++m_floorMin <= m_floorMax);
    m_mustUpdateVisibleTilesCache = false;
    m_updateTilesPos = 0;

    // there is no tile to render on invalid positions
    const Position cameraPosition = getCameraPosition();
    if(!cameraPosition.isValid())
        return;

    const uint_fast8_t cameraZ = cameraPosition.z;

    uint_fast16_t processedTiles = 0;

    m_floorMin = cameraZ;
    m_floorMax = cameraZ;

    bool stop = false;

    // cache visible tiles in draw order
    // draw from last floor (the lower) to first floor (the higher)
    const int numDiagonals = m_drawDimension.width() + m_drawDimension.height() - 1;
    for(int iz = m_cachedLastVisibleFloor; iz >= m_cachedFirstVisibleFloor && !stop; --iz) {
        // loop through / diagonals beginning at top left and going to top right
        for(int diagonal = 0; diagonal < numDiagonals && !stop; ++diagonal) {
            // loop current diagonal tiles
            const int advance = std::max<int>(diagonal - m_drawDimension.height(), 0);
            for(int iy = diagonal - advance, ix = advance; iy >= 0 && ix < m_drawDimension.width() && !stop; --iy, ++ix) {
                // only start really looking tiles in the desired start
                if(m_updateTilesPos < start) {
                    ++m_updateTilesPos;
                    continue;
                }

                // avoid rendering too much tiles at once
                if(processedTiles > MAX_TILE_DRAWS && m_viewMode >= HUGE_VIEW) {
                    stop = true;
                    break;
                }

                // position on current floor
                //TODO: check position limits
                Position tilePos = cameraPosition.translated(ix - m_virtualCenterOffset.x, iy - m_virtualCenterOffset.y);
                // adjust tilePos to the wanted floor
                tilePos.coveredUp(cameraPosition.z - iz);
                if(const TilePtr& tile = g_map.getTile(tilePos)) {
                    // skip tiles that have nothing
                    if(!tile->isDrawable())
                        continue;

                    // skip tiles that are completely behind another tile
                    if(g_map.isCompletelyCovered(tilePos, m_cachedFirstVisibleFloor))
                        continue;

                    m_cachedVisibleTiles[iz].push_back(tile);

                    if(iz < m_floorMin)
                        m_floorMin = iz;
                    else if(iz > m_floorMax)
                        m_floorMax = iz;

                    ++processedTiles;
                }

                ++m_updateTilesPos;
            }
        }
    }

    if(!stop) m_updateTilesPos = 0;

    if(start == 0 && m_viewMode <= NEAR_VIEW)
        m_cachedFloorVisibleCreatures = g_map.getSightSpectators(cameraPosition, false);
}

void MapView::updateGeometry(const Size& visibleDimension, const Size& optimizedSize)
{
    int tileSize = 0;
    Size bufferSize;

    int possiblesTileSizes[] = { 1,2,4,8,16,32 };
    for(int candidateTileSize : possiblesTileSizes) {
        bufferSize = (visibleDimension + Size(3, 3)) * candidateTileSize;
        if(bufferSize.width() > g_graphics.getMaxTextureSize() || bufferSize.height() > g_graphics.getMaxTextureSize())
            break;

        tileSize = candidateTileSize;
        if(optimizedSize.width() < bufferSize.width() - 3 * candidateTileSize && optimizedSize.height() < bufferSize.height() - 3 * candidateTileSize)
            break;
    }

    if(tileSize == 0) {
        g_logger.traceError("reached max zoom out");
        return;
    }

    const Size drawDimension = visibleDimension + Size(3, 3);
    const Point virtualCenterOffset = (drawDimension / 2 - Size(1, 1)).toPoint();
    const Point visibleCenterOffset = virtualCenterOffset;

    ViewMode viewMode = m_viewMode;
    if(m_autoViewMode) {
        if(tileSize >= 32 && visibleDimension.area() <= NEAR_VIEW_AREA)
            viewMode = NEAR_VIEW;
        else if(tileSize >= 16 && visibleDimension.area() <= MID_VIEW_AREA)
            viewMode = MID_VIEW;
        else if(tileSize >= 8 && visibleDimension.area() <= FAR_VIEW_AREA)
            viewMode = FAR_VIEW;
        else
            viewMode = HUGE_VIEW;

        if(viewMode >= FAR_VIEW)
            m_multifloor = false;
        else
            m_multifloor = true;
    }

    // draw actually more than what is needed to avoid massive recalculations on huge views
    /*
    if(viewMode >= HUGE_VIEW) {
        Size oldDimension = drawDimension;
        drawDimension = (m_framebuffer->getSize() / tileSize);
        virtualCenterOffset += (drawDimension - oldDimension).toPoint() / 2;
    }
    */

    m_viewMode = viewMode;
    m_visibleDimension = visibleDimension;
    m_drawDimension = drawDimension;
    m_tileSize = tileSize;
    m_virtualCenterOffset = virtualCenterOffset;
    m_visibleCenterOffset = visibleCenterOffset;
    m_optimizedSize = optimizedSize;

    m_framebuffer->resize(bufferSize);
    m_nameFramebuffer->resize(bufferSize * 4);

    requestVisibleTilesCacheUpdate();
}

void MapView::onTileUpdate(const Position&)
{
    requestVisibleTilesCacheUpdate();
}

void MapView::onMapCenterChange(const Position&)
{
    requestVisibleTilesCacheUpdate();
}

void MapView::lockFirstVisibleFloor(int firstVisibleFloor)
{
    m_lockedFirstVisibleFloor = firstVisibleFloor;
    requestVisibleTilesCacheUpdate();
}

void MapView::unlockFirstVisibleFloor()
{
    m_lockedFirstVisibleFloor = -1;
    requestVisibleTilesCacheUpdate();
}

void MapView::setVisibleDimension(const Size& visibleDimension)
{
    if(visibleDimension == m_visibleDimension)
        return;

    if(visibleDimension.width() % 2 != 1 || visibleDimension.height() % 2 != 1) {
        g_logger.traceError("visible dimension must be odd");
        return;
    }

    if(visibleDimension < Size(3, 3)) {
        g_logger.traceError("reach max zoom in");
        return;
    }

    updateGeometry(visibleDimension, m_optimizedSize);
}

void MapView::setViewMode(ViewMode viewMode)
{
    m_viewMode = viewMode;
    requestVisibleTilesCacheUpdate();
}

void MapView::setAutoViewMode(bool enable)
{
    m_autoViewMode = enable;
    if(enable)
        updateGeometry(m_visibleDimension, m_optimizedSize);
}

void MapView::optimizeForSize(const Size& visibleSize)
{
    updateGeometry(m_visibleDimension, visibleSize);
}

void MapView::followCreature(const CreaturePtr& creature)
{
    m_follow = true;
    m_followingCreature = creature;
    requestVisibleTilesCacheUpdate();
}

void MapView::setCameraPosition(const Position& pos)
{
    m_follow = false;
    m_customCameraPosition = pos;
    requestVisibleTilesCacheUpdate();
}

Position MapView::getPosition(const Point& point, const Size& mapSize)
{
    const Position cameraPosition = getCameraPosition();

    // if we have no camera, its impossible to get the tile
    if(!cameraPosition.isValid())
        return Position();

    const Rect srcRect = calcFramebufferSource(mapSize);
    const float sh = srcRect.width() / static_cast<float>(mapSize.width());
    const float sv = srcRect.height() / static_cast<float>(mapSize.height());

    const Point framebufferPos = Point(point.x * sh, point.y * sv);
    const Point centerOffset = (framebufferPos + srcRect.topLeft()) / m_tileSize;

    const Point tilePos2D = getVisibleCenterOffset() - m_drawDimension.toPoint() + centerOffset + Point(2, 2);
    if(tilePos2D.x + cameraPosition.x < 0 && tilePos2D.y + cameraPosition.y < 0)
        return Position();

    Position position = Position(tilePos2D.x, tilePos2D.y, 0) + cameraPosition;

    if(!position.isValid())
        return Position();

    return position;
}

void MapView::move(int x, int y)
{
    m_moveOffset.x += x;
    m_moveOffset.y += y;

    int32_t tmp = m_moveOffset.x / 32;
    bool requestTilesUpdate = false;
    if(tmp != 0) {
        m_customCameraPosition.x += tmp;
        m_moveOffset.x %= 32;
        requestTilesUpdate = true;
    }

    tmp = m_moveOffset.y / 32;
    if(tmp != 0) {
        m_customCameraPosition.y += tmp;
        m_moveOffset.y %= 32;
        requestTilesUpdate = true;
    }

    if(requestTilesUpdate)
        requestVisibleTilesCacheUpdate();
}

Rect MapView::calcFramebufferSource(const Size& destSize)
{
    const float scaleFactor = m_tileSize / static_cast<float>(Otc::TILE_PIXELS);
    Point drawOffset = ((m_drawDimension - m_visibleDimension - Size(1, 1)).toPoint() / 2) * m_tileSize;
    if(isFollowingCreature())
        drawOffset += m_followingCreature->getWalkOffset() * scaleFactor;
    else if(!m_moveOffset.isNull())
        drawOffset += m_moveOffset * scaleFactor;

    Size srcSize = destSize;
    const Size srcVisible = m_visibleDimension * m_tileSize;
    srcSize.scale(srcVisible, Fw::KeepAspectRatio);
    drawOffset.x += (srcVisible.width() - srcSize.width()) / 2;
    drawOffset.y += (srcVisible.height() - srcSize.height()) / 2;

    return Rect(drawOffset, srcSize);
}

int MapView::calcFirstVisibleFloor()
{
    int z = 7;
    // return forced first visible floor
    if(m_lockedFirstVisibleFloor != -1) {
        z = m_lockedFirstVisibleFloor;
    } else {
        const Position cameraPosition = getCameraPosition();

        // this could happens if the player is not known yet
        if(cameraPosition.isValid()) {
            // avoid rendering multifloors in far views
            if(!m_multifloor) {
                z = cameraPosition.z;
            } else {
                // if nothing is limiting the view, the first visible floor is 0
                int firstFloor = 0;

                // limits to underground floors while under sea level
                if(cameraPosition.z > Otc::SEA_FLOOR)
                    firstFloor = std::max<int>(cameraPosition.z - Otc::AWARE_UNDEGROUND_FLOOR_RANGE, static_cast<int>(Otc::UNDERGROUND_FLOOR));

                // loop in 3x3 tiles around the camera
                for(int ix = -1; ix <= 1 && firstFloor < cameraPosition.z; ++ix) {
                    for(int iy = -1; iy <= 1 && firstFloor < cameraPosition.z; ++iy) {
                        Position pos = cameraPosition.translated(ix, iy);

                        // process tiles that we can look through, e.g. windows, doors
                        if((ix == 0 && iy == 0) || ((std::abs(ix) != std::abs(iy)) && g_map.isLookPossible(pos))) {
                            Position upperPos = pos;
                            Position coveredPos = pos;

                            while(coveredPos.coveredUp() && upperPos.up() && upperPos.z >= firstFloor) {
                                // check tiles physically above
                                TilePtr tile = g_map.getTile(upperPos);
                                if(tile && tile->limitsFloorsView(!g_map.isLookPossible(pos))) {
                                    firstFloor = upperPos.z + 1;
                                    break;
                                }

                                // check tiles geometrically above
                                tile = g_map.getTile(coveredPos);
                                if(tile && tile->limitsFloorsView(g_map.isLookPossible(pos))) {
                                    firstFloor = coveredPos.z + 1;
                                    break;
                                }
                            }
                        }
                    }
                }
                z = firstFloor;
            }
        }
    }

    // just ensure the that the floor is in the valid range
    z = stdext::clamp<int>(z, 0, static_cast<int>(Otc::MAX_Z));
    return z;
}

int MapView::calcLastVisibleFloor()
{
    if(!m_multifloor)
        return calcFirstVisibleFloor();

    int z = 7;

    const Position cameraPosition = getCameraPosition();
    // this could happens if the player is not known yet
    if(cameraPosition.isValid()) {
        // view only underground floors when below sea level
        if(cameraPosition.z > Otc::SEA_FLOOR)
            z = cameraPosition.z + Otc::AWARE_UNDEGROUND_FLOOR_RANGE;
        else
            z = Otc::SEA_FLOOR;
    }

    if(m_lockedFirstVisibleFloor != -1)
        z = std::max<int>(m_lockedFirstVisibleFloor, z);

    // just ensure the that the floor is in the valid range
    z = stdext::clamp<int>(z, 0, static_cast<int>(Otc::MAX_Z));
    return z;
}

Position MapView::getCameraPosition()
{
    if(isFollowingCreature())
        return m_followingCreature->getPosition();

    return m_customCameraPosition;
}

void MapView::setShader(const PainterShaderProgramPtr& shader, float fadein, float fadeout)
{
    if((m_shader == shader && m_shaderSwitchDone) || (m_nextShader == shader && !m_shaderSwitchDone))
        return;

    if(fadeout > 0.0f && m_shader) {
        m_nextShader = shader;
        m_shaderSwitchDone = false;
    } else {
        m_shader = shader;
        m_nextShader = nullptr;
        m_shaderSwitchDone = true;
    }
    m_fadeTimer.restart();
    m_fadeInTime = fadein;
    m_fadeOutTime = fadeout;
}

void MapView::setDrawLights(bool enable)
{
    if(enable == m_drawLights) return;

    m_lightView = enable ? LightViewPtr(new LightView) : nullptr;

    requestDrawing(Otc::ReDrawTile_Light);
    m_mustCleanFramebuffer = true;
    m_drawLights = enable;
}

/* vim: set ts=4 sw=4 et: */