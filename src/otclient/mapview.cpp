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

#include "mapview.h"

#include "creature.h"
#include "map.h"
#include "tile.h"
#include "statictext.h"
#include "animatedtext.h"
#include "missile.h"
#include "shadermanager.h"

#include <framework/graphics/graphics.h>
#include <framework/graphics/framebuffermanager.h>
#include <framework/core/eventdispatcher.h>
#include <framework/core/application.h>


enum {
    // 3840x2160 => 1080p optimized
    // 2560x1440 => 720p optimized
    // 1728x972 => 480p optimized

    NEAR_VIEW_AREA = 32*32,
    MID_VIEW_AREA = 64*64,
    FAR_VIEW_AREA = 128*128,
    MAX_TILE_DRAWS = NEAR_VIEW_AREA*7
};

MapView::MapView()
{
    m_viewMode = NEAR_VIEW;
    m_lockedFirstVisibleFloor = -1;
    m_cachedFirstVisibleFloor = 7;
    m_cachedLastVisibleFloor = 7;
    m_updateTilesPos = 0;
    m_optimizedSize = Size(Otc::AWARE_X_TILES, Otc::AWARE_Y_TILES) * Otc::TILE_PIXELS;

    m_framebuffer = g_framebuffers.createFrameBuffer();
    setVisibleDimension(Size(15, 11));

    m_shader = g_shaders.getDefaultMapShader();
}

MapView::~MapView()
{
    assert(!g_app.isTerminated());
}

void MapView::draw(const Rect& rect)
{
    // update visible tiles cache when needed
    if(m_mustUpdateVisibleTilesCache || m_updateTilesPos > 0)
        updateVisibleTilesCache(m_mustUpdateVisibleTilesCache ? 0 : m_updateTilesPos);

    float scaleFactor = m_tileSize/(float)Otc::TILE_PIXELS;
    Position cameraPosition = getCameraPosition();

    int drawFlags = 0;
    if(m_viewMode == NEAR_VIEW)
        drawFlags = Otc::DrawGround | Otc::DrawGroundBorders | Otc::DrawWalls |
                    Otc::DrawItems | Otc::DrawCreatures | Otc::DrawEffects | Otc::DrawMissiles | Otc::DrawAnimations;
    else
        drawFlags = Otc::DrawGround | Otc::DrawGroundBorders | Otc::DrawWalls | Otc::DrawItems;

    Size tileSize = Size(1,1) * m_tileSize;
    if(m_mustDrawVisibleTilesCache || (drawFlags & Otc::DrawAnimations)) {
        m_framebuffer->bind();

        if(m_mustCleanFramebuffer) {
            Rect clearRect = Rect(0, 0, m_drawDimension * m_tileSize);
            g_painter->setColor(Color::black);
            g_painter->drawFilledRect(clearRect);
        }
        g_painter->setColor(Color::white);

        auto it = m_cachedVisibleTiles.begin();
        auto end = m_cachedVisibleTiles.end();
        for(int z=m_cachedLastVisibleFloor;z>=m_cachedFirstVisibleFloor;--z) {
            while(it != end) {
                const TilePtr& tile = *it;
                Position tilePos = tile->getPosition();
                if(tilePos.z != z)
                    break;
                else
                    ++it;

                if(!m_drawMinimapColors)
                    tile->draw(transformPositionTo2D(tilePos, cameraPosition), scaleFactor, drawFlags);
                else {
                    uint8 c = tile->getMinimapColorByte();
                    if(c == 0)
                        continue;

                    g_painter->setColor(Color::from8bit(c));
                    g_painter->drawFilledRect(Rect(transformPositionTo2D(tilePos, cameraPosition), tileSize));
                }
            }

            if(drawFlags & Otc::DrawMissiles && !m_drawMinimapColors) {
                for(const MissilePtr& missile : g_map.getFloorMissiles(z)) {
                    missile->draw(transformPositionTo2D(missile->getPosition(), cameraPosition), scaleFactor, drawFlags & Otc::DrawAnimations);
                }
            }
        }

        m_framebuffer->release();

        // generating mipmaps each frame can be slow in older cards
        //m_framebuffer->getTexture()->buildHardwareMipmaps();

        m_mustDrawVisibleTilesCache = false;
    }


    Point drawOffset = ((m_drawDimension - m_visibleDimension - Size(1,1)).toPoint()/2) * m_tileSize;
    if(isFollowingCreature())
        drawOffset += m_followingCreature->getWalkOffset() * scaleFactor;

    Size srcSize = rect.size();
    Size srcVisible = m_visibleDimension * m_tileSize;
    srcSize.scale(srcVisible, Fw::KeepAspectRatio);
    drawOffset.x += (srcVisible.width() - srcSize.width()) / 2;
    drawOffset.y += (srcVisible.height() - srcSize.height()) / 2;
    Rect srcRect = Rect(drawOffset, srcSize);

    g_painter->setColor(Color::white);
    glDisable(GL_BLEND);
    g_painter->setShaderProgram(m_shader);
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
    glEnable(GL_BLEND);


    // this could happen if the player position is not known yet
    if(!cameraPosition.isValid())
        return;

    float horizontalStretchFactor = rect.width() / (float)srcRect.width();
    float verticalStretchFactor = rect.height() / (float)srcRect.height();

    // avoid drawing texts on map in far zoom outs
    if(m_viewMode == NEAR_VIEW && m_drawTexts) {
        for(const CreaturePtr& creature : m_cachedFloorVisibleCreatures) {
            Point creatureOffset = Point(16 - creature->getDisplacementX(), -3 - creature->getDisplacementY());
            Position pos = creature->getPosition();
            Point p = transformPositionTo2D(pos, cameraPosition) - drawOffset;
            p += (creature->getDrawOffset() + creatureOffset) * scaleFactor;
            p.x = p.x * horizontalStretchFactor;
            p.y = p.y * verticalStretchFactor;
            p += rect.topLeft();

            creature->drawInformation(p, g_map.isCovered(pos, m_cachedFirstVisibleFloor), rect);
        }

        for(const StaticTextPtr& staticText : g_map.getStaticTexts()) {
            Position pos = staticText->getPosition();

            // ony draw static texts from current camera floor, unless yells
            //if(pos.z != cameraPosition.z && !staticText->isYell())
            //    continue;

            Point p = transformPositionTo2D(pos, cameraPosition) - drawOffset;
            p.x = p.x * horizontalStretchFactor;
            p.y = p.y * verticalStretchFactor;
            p += rect.topLeft();
            staticText->drawText(p, rect);
        }

        for(const AnimatedTextPtr& animatedText : g_map.getAnimatedTexts()) {
            Position pos = animatedText->getPosition();

            // only draw animated texts from visible floors
            if(pos.z < m_cachedFirstVisibleFloor || pos.z > m_cachedLastVisibleFloor)
                continue;

            // dont draw animated texts from covered tiles
            if(pos.z != cameraPosition.z && g_map.isCovered(pos, m_cachedFirstVisibleFloor))
                continue;

            Point p = transformPositionTo2D(pos, cameraPosition) - drawOffset;
            p.x = p.x * horizontalStretchFactor;
            p.y = p.y * verticalStretchFactor;
            p += rect.topLeft();
            animatedText->drawText(p, rect);
        }
    } else if(m_viewMode > NEAR_VIEW) {
        // draw a cross in the center instead of our creature
        /*
            Known Issue: Changing Z axis causes the cross to go off a little bit.
        */
        Rect vRect(0, 0, 2, 10);
        Rect hRect(0, 0, 10, 2);
        g_painter->setColor(Color::white);

        if(!m_follow && m_followingCreature)
        {
            Position pos = m_followingCreature->getPosition();
            Point p = transformPositionTo2D(pos, cameraPosition) - drawOffset;
            p.x = p.x * horizontalStretchFactor;
            p.y = p.y * verticalStretchFactor;
            p += rect.topLeft();

            vRect.setX(p.x); vRect.setY(p.y - 4);
            hRect.setX(p.x - 4); hRect.setY(p.y);

            hRect.setWidth(10); hRect.setHeight(2);
            vRect.setWidth(2); vRect.setHeight(10);
        }
        else {
            vRect.moveCenter(rect.center());
            hRect.moveCenter(rect.center());
        }

        g_painter->drawFilledRect(vRect);
        g_painter->drawFilledRect(hRect);
    }
}

void MapView::updateVisibleTilesCache(int start)
{
    if(start == 0) {
        m_cachedFirstVisibleFloor = calcFirstVisibleFloor();
        m_cachedLastVisibleFloor = calcLastVisibleFloor();
        assert(m_cachedFirstVisibleFloor >= 0 && m_cachedLastVisibleFloor >= 0 &&
               m_cachedFirstVisibleFloor <= Otc::MAX_Z && m_cachedLastVisibleFloor <= Otc::MAX_Z);

        if(m_cachedLastVisibleFloor < m_cachedFirstVisibleFloor)
            m_cachedLastVisibleFloor = m_cachedFirstVisibleFloor;

        m_cachedFloorVisibleCreatures.clear();
        m_cachedVisibleTiles.clear();

        m_mustCleanFramebuffer = true;
        m_mustDrawVisibleTilesCache = true;
        m_mustUpdateVisibleTilesCache = false;
        m_updateTilesPos = 0;
    } else
        m_mustCleanFramebuffer = false;

    // there is no tile to render on invalid positions
    Position cameraPosition = getCameraPosition();
    if(!cameraPosition.isValid())
        return;

    bool stop = false;

    // clear current visible tiles cache
    m_cachedVisibleTiles.clear();
    m_mustDrawVisibleTilesCache = true;
    m_updateTilesPos = 0;

    // cache visible tiles in draw order
    // draw from last floor (the lower) to first floor (the higher)
    for(int iz = m_cachedLastVisibleFloor; iz >= m_cachedFirstVisibleFloor && !stop; --iz) {
        if(m_viewMode <= FAR_VIEW) {
            const int numDiagonals = m_drawDimension.width() + m_drawDimension.height() - 1;
            // loop through / diagonals beginning at top left and going to top right
            for(int diagonal = 0; diagonal < numDiagonals && !stop; ++diagonal) {
                // loop current diagonal tiles
                int advance = std::max(diagonal - m_drawDimension.height(), 0);
                for(int iy = diagonal - advance, ix = advance; iy >= 0 && ix < m_drawDimension.width() && !stop; --iy, ++ix) {
                    // only start really looking tiles in the desired start
                    if(m_updateTilesPos < start) {
                        m_updateTilesPos++;
                        continue;
                    }

                    // avoid rendering too much tiles at once
                    if((int)m_cachedVisibleTiles.size() > MAX_TILE_DRAWS && m_viewMode >= HUGE_VIEW) {
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
                        if(tile->isEmpty())
                            continue;
                        // skip tiles that are completely behind another tile
                        if(g_map.isCompletelyCovered(tilePos, m_cachedFirstVisibleFloor))
                            continue;
                        m_cachedVisibleTiles.push_back(tile);
                    }
                    m_updateTilesPos++;
                }
            }
        } else {
            // cache tiles in spiral mode
            static std::vector<Point> m_spiral;
            if(start == 0) {
                m_spiral.resize(m_drawDimension.area());
                int width = m_drawDimension.width();
                int height = m_drawDimension.height();
                int tpx = width/2 - 2;
                int tpy = height/2 - 2;
                int count = 0;
                Rect area(0, 0, m_drawDimension);
                m_spiral[count++] = Point(tpx+1,tpy+1);
                for(int step = 1; tpx >= 0 || tpy >= 0; ++step, --tpx, --tpy) {
                    int qs = 2*step;
                    Rect lines[4] = {
                        Rect(tpx,       tpy,       qs,  1),
                        Rect(tpx + qs,  tpy,       1,   qs),
                        Rect(tpx +  1,  tpy + qs,  qs,  1),
                        Rect(tpx,       tpy + 1,   1,   qs),
                    };

                    for(int i=0;i<4;++i) {
                        int sx = std::max(lines[i].left(), area.left());
                        int ex = std::min(lines[i].right(), area.right());
                        int sy = std::max(lines[i].top(), area.top());
                        int ey = std::min(lines[i].bottom(), area.bottom());
                        for(int qx=sx;qx<=ex;++qx)
                            for(int qy=sy;qy<=ey;++qy)
                                m_spiral[count++] = Point(qx, qy);
                    }
                }
            }

            for(m_updateTilesPos = start; m_updateTilesPos < (int)m_spiral.size(); ++m_updateTilesPos) {
                // avoid rendering too much tiles at once
                if((int)m_cachedVisibleTiles.size() > MAX_TILE_DRAWS) {
                    stop = true;
                    break;
                }

                const Point& p = m_spiral[m_updateTilesPos];
                Position tilePos = cameraPosition.translated(p.x - m_virtualCenterOffset.x, p.y - m_virtualCenterOffset.y);
                tilePos.coveredUp(cameraPosition.z - iz);
                if(const TilePtr& tile = g_map.getTile(tilePos)) {
                    if(!tile->isEmpty())
                        m_cachedVisibleTiles.push_back(tile);
                }
            }
        }
    }

    if(!stop) {
        m_updateTilesPos = 0;
        m_spiral.clear();
    }

    if(start == 0 && m_drawTexts && m_viewMode <= NEAR_VIEW)
        m_cachedFloorVisibleCreatures = g_map.getSpectators(cameraPosition, false);
}

void MapView::updateGeometry(const Size& visibleDimension, const Size& optimizedSize)
{
    int tileSize = 0;
    Size bufferSize;

    if(!m_drawMinimapColors) {
        int possiblesTileSizes[] = {1,2,4,8,16,32};
        for(int candidateTileSize : possiblesTileSizes) {
            bufferSize = (visibleDimension + Size(3,3)) * candidateTileSize;
            if(bufferSize.width() > g_graphics.getMaxTextureSize() || bufferSize.height() > g_graphics.getMaxTextureSize())
                break;

            tileSize = candidateTileSize;
            if(optimizedSize.width() < bufferSize.width() - 3*candidateTileSize && optimizedSize.height() < bufferSize.height() - 3*candidateTileSize)
                break;
        }

        if(tileSize == 0) {
            g_logger.traceError("reached max zoom out");
            return;
        }
    } else {
        tileSize = 1;
        bufferSize = visibleDimension + Size(3,3);
    }

    Size drawDimension = visibleDimension + Size(3,3);
    Point virtualCenterOffset = (drawDimension/2 - Size(1,1)).toPoint();
    Point visibleCenterOffset = virtualCenterOffset;

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

    requestVisibleTilesCacheUpdate();
}

void MapView::onTileUpdate(const Position& pos)
{
    if(!m_drawMinimapColors)
        requestVisibleTilesCacheUpdate();
}

void MapView::onMapCenterChange(const Position& pos)
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

    if(visibleDimension < Size(3,3)) {
        g_logger.traceError("reach max zoom in");
        return;
    }

    updateGeometry(visibleDimension, m_optimizedSize);
}

void MapView::setViewMode(MapView::ViewMode viewMode)
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

int MapView::calcFirstVisibleFloor()
{
    int z = 7;
    // return forced first visible floor
    if(m_lockedFirstVisibleFloor != -1) {
        z = m_lockedFirstVisibleFloor;
    } else {
        Position cameraPosition = getCameraPosition();

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
                    firstFloor = std::max(cameraPosition.z - Otc::AWARE_UNDEGROUND_FLOOR_RANGE, (int)Otc::UNDERGROUND_FLOOR);

                // loop in 3x3 tiles around the camera
                for(int ix = -1; ix <= 1 && firstFloor < cameraPosition.z; ++ix) {
                    for(int iy = -1; iy <= 1 && firstFloor < cameraPosition.z; ++iy) {
                        Position pos = cameraPosition.translated(ix, iy);

                        // process tiles that we can look through, e.g. windows, doors
                        if((ix == 0 && iy == 0) || (/*(std::abs(ix) != std::abs(iy)) && */g_map.isLookPossible(pos))) {
                            Position upperPos = pos;
                            Position coveredPos = pos;

                            while(coveredPos.coveredUp() && upperPos.up() && upperPos.z >= firstFloor) {
                                // check tiles physically above
                                TilePtr tile = g_map.getTile(upperPos);
                                if(tile && tile->limitsFloorsView()) {
                                    firstFloor = upperPos.z + 1;
                                    break;
                                }

                                // check tiles geometrically above
                                tile = g_map.getTile(coveredPos);
                                if(tile && tile->limitsFloorsView()) {
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
    z = std::min(std::max(z, 0), (int)Otc::MAX_Z);
    return z;
}

int MapView::calcLastVisibleFloor()
{
    if(!m_multifloor)
        return calcFirstVisibleFloor();

    int z = 7;

    Position cameraPosition = getCameraPosition();
    // this could happens if the player is not known yet
    if(cameraPosition.isValid()) {
        // view only underground floors when below sea level
        if(cameraPosition.z > Otc::SEA_FLOOR)
            z = cameraPosition.z + Otc::AWARE_UNDEGROUND_FLOOR_RANGE;
        else
            z = Otc::SEA_FLOOR;
    }

    if(m_lockedFirstVisibleFloor != -1)
        z = std::max(m_lockedFirstVisibleFloor, z);

    // just ensure the that the floor is in the valid range
    z = std::min(std::max(z, 0), (int)Otc::MAX_Z);
    return z;
}

Position MapView::getCameraPosition()
{
    if(isFollowingCreature())
        return m_followingCreature->getPosition();

    return m_customCameraPosition;
}

TilePtr MapView::getTile(const Point& mousePos, const Rect& mapRect)
{
    Point relativeMousePos = mousePos - mapRect.topLeft();
    Size visibleSize = m_visibleDimension * m_tileSize;
    Position cameraPosition = getCameraPosition();

    // if we have no camera, its impossible to get the tile
    if(!cameraPosition.isValid())
        return nullptr;

    float scaleFactor = m_tileSize / (float)Otc::TILE_PIXELS;


    float horizontalStretchFactor = visibleSize.width() / (float)mapRect.width();
    float verticalStretchFactor = visibleSize.height() / (float)mapRect.height();

    Point tilePos2D = Point(relativeMousePos.x * horizontalStretchFactor, relativeMousePos.y * verticalStretchFactor);

    if(isFollowingCreature())
        tilePos2D += m_followingCreature->getWalkOffset() * scaleFactor;
    tilePos2D /= m_tileSize;

    Position tilePos = Position(1 + (int)tilePos2D.x - m_visibleCenterOffset.x, 1 + (int)tilePos2D.y - m_visibleCenterOffset.y, 0) + cameraPosition;
    if(!tilePos.isValid())
        return nullptr;

    // we must check every floor, from top to bottom to check for a clickable tile
    TilePtr tile;
    tilePos.coveredUp(tilePos.z - m_cachedFirstVisibleFloor);
    for(int i = m_cachedFirstVisibleFloor; i <= m_cachedLastVisibleFloor; i++) {
        tile = g_map.getTile(tilePos);
        if(tile && tile->isClickable())
            break;
        tilePos.coveredDown();
    }

    if(!tile || !tile->isClickable())
        return nullptr;

    return tile;
}

void MapView::setDrawMinimapColors(bool enable)
{
    if(m_drawMinimapColors == enable)
        return;
    m_drawMinimapColors = enable;
    updateGeometry(m_visibleDimension, m_optimizedSize);
    requestVisibleTilesCacheUpdate();
    m_smooth = !enable;
    m_framebuffer->setSmooth(m_smooth);
}

