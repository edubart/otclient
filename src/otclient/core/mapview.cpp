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

#include <framework/graphics/graphics.h>
#include <framework/graphics/framebuffer.h>
#include <framework/graphics/paintershaderprogram.h>
#include <framework/graphics/paintershadersources.h>
#include "creature.h"
#include "map.h"
#include "tile.h"
#include "statictext.h"
#include "animatedtext.h"
#include "missile.h"
#include <framework/core/eventdispatcher.h>

MapView::MapView()
{
    m_viewRange = NEAR_VIEW;
    m_lockedFirstVisibleFloor = -1;
    m_cachedFirstVisibleFloor = 0;
    m_cachedLastVisibleFloor = 7;
    m_customCameraPosition.z = 7;

    Size frameBufferSize(std::min(g_graphics.getMaxTextureSize(), (int)DEFAULT_FRAMBUFFER_WIDTH),
                         std::min(g_graphics.getMaxTextureSize(), (int)DEFAULT_FRAMBUFFER_HEIGHT));

    m_framebuffer = FrameBufferPtr(new FrameBuffer(frameBufferSize));
    m_framebuffer->setClearColor(Color::black);
    setVisibleDimension(Size(15, 11));

    m_shaderProgram = PainterShaderProgramPtr(new PainterShaderProgram);
    m_shaderProgram->addShaderFromSourceCode(Shader::Vertex, glslMainWithTexCoordsVertexShader + glslPositionOnlyVertexShader);
    m_shaderProgram->addShaderFromSourceFile(Shader::Fragment, "/game_shaders/map.frag");
    m_shaderProgram->link();
}

void MapView::draw(const Rect& rect)
{
    // update visible tiles cache when needed
    if(m_mustUpdateVisibleTilesCache)
        updateVisibleTilesCache();

    float scaleFactor = m_tileSize/(float)Otc::TILE_PIXELS;
    Position cameraPosition = getCameraPosition();

    int drawFlags = 0;
    if(m_viewRange == NEAR_VIEW)
        drawFlags = Otc::DrawGround | Otc::DrawGroundBorders | Otc::DrawWalls |
                    Otc::DrawItems | Otc::DrawCreatures | Otc::DrawEffects | Otc::DrawMissiles | Otc::DrawAnimations;
    else if(m_viewRange == MID_VIEW)
        drawFlags = Otc::DrawGround | Otc::DrawGroundBorders | Otc::DrawWalls | Otc::DrawItems;
    else if(m_viewRange == FAR_VIEW)
        drawFlags = Otc::DrawGround | Otc::DrawGroundBorders | Otc::DrawWalls;
    else if(m_tileSize >= 4) // HUGE_VIEW 1
        drawFlags = Otc::DrawGround | Otc::DrawGroundBorders;
    else // HUGE_VIEW 2
        drawFlags = Otc::DrawGround;

    if(m_mustDrawVisibleTilesCache || (drawFlags & Otc::DrawAnimations)) {
        m_framebuffer->bind(m_mustCleanFramebuffer);

        auto it = m_cachedVisibleTiles.begin();
        auto end = m_cachedVisibleTiles.end();
        for(int z=m_cachedLastVisibleFloor;z>=m_cachedFirstVisibleFloor;--z) {
            while(it != end) {
                const TilePtr& tile = *it;
                if(tile->getPosition().z != z)
                    break;
                else
                    ++it;

                tile->draw(transformPositionTo2D(tile->getPosition()), scaleFactor, drawFlags);
            }

            if(drawFlags & Otc::DrawMissiles) {
                for(const MissilePtr& missile : g_map.getFloorMissiles(z)) {
                    missile->draw(transformPositionTo2D(missile->getPosition()), scaleFactor, drawFlags & Otc::DrawAnimations);
                }
            }
        }

        m_framebuffer->release();

        // generating mipmaps each frame can be slow in older cards
        if(g_graphics.canGenerateRealtimeMipmaps())
            m_framebuffer->getTexture()->generateHardwareMipmaps();

        m_mustDrawVisibleTilesCache = false;
    }

    g_painter.setCustomProgram(m_shaderProgram);
    g_painter.setColor(Color::white);

    Point drawOffset = ((m_drawDimension - m_visibleDimension - Size(1,1)).toPoint()/2) * m_tileSize;
    if(m_followingCreature)
        drawOffset += m_followingCreature->getWalkOffset() * scaleFactor;
    Rect srcRect = Rect(drawOffset, m_visibleDimension * m_tileSize);
    m_framebuffer->draw(rect, srcRect);

    g_painter.releaseCustomProgram();

    // this could happen if the player position is not known yet
    if(!cameraPosition.isValid())
        return;

    float horizontalStretchFactor = rect.width() / (float)(m_visibleDimension.width() * m_tileSize);
    float verticalStretchFactor = rect.height() / (float)(m_visibleDimension.height() * m_tileSize);
    //Size tileStretchedSize = Size(m_tileSize * horizontalStretchFactor, m_tileSize * verticalStretchFactor);

    // avoid drawing texts on map in far zoom outs
    if(m_viewRange == NEAR_VIEW) {
        for(const CreaturePtr& creature : m_cachedFloorVisibleCreatures) {
            Position pos = creature->getPosition();

            Point p = transformPositionTo2D(pos) - drawOffset;
            p += (creature->getDrawOffset() + Point(8, -10)) * scaleFactor;
            p.x = p.x * horizontalStretchFactor;
            p.y = p.y * verticalStretchFactor;
            p += rect.topLeft();

            creature->drawInformation(p, g_map.isCovered(pos, m_cachedFirstVisibleFloor), rect);
        }

        for(const StaticTextPtr& staticText : g_map.getStaticTexts()) {
            Position pos = staticText->getPosition();

            // ony draw static texts from current camera floor, unless yells
            if(pos.z != getCameraPosition().z && !staticText->isYell())
                continue;

            Point p = transformPositionTo2D(pos) - drawOffset;
            p.x = p.x * horizontalStretchFactor;
            p.y = p.y * verticalStretchFactor;
            p += rect.topLeft();
            staticText->draw(p, rect);
        }

        for(const AnimatedTextPtr& animatedText : g_map.getAnimatedTexts()) {
            Position pos = animatedText->getPosition();

            // only draw animated texts from visible floors
            if(pos.z < m_cachedFirstVisibleFloor || pos.z > m_cachedLastVisibleFloor)
                continue;

            // dont draw animated texts from covered tiles
            if(pos.z != cameraPosition.z && g_map.isCovered(pos, m_cachedFirstVisibleFloor))
                continue;

            Point p = transformPositionTo2D(pos) - drawOffset;
            p.x = p.x * horizontalStretchFactor;
            p.y = p.y * verticalStretchFactor;
            p += rect.topLeft();
            animatedText->draw(p, rect);
        }
    } else {
        // draw a cross in the center instead of our creature
        Rect vRect(0, 0, 2, 10);
        Rect hRect(0, 0, 10, 2);
        vRect.moveCenter(rect.center());
        hRect.moveCenter(rect.center());
        g_painter.setColor(Color::white);
        g_painter.drawFilledRect(vRect);
        g_painter.drawFilledRect(hRect);
    }
}

void MapView::updateVisibleTilesCache(int start)
{
    if(start == 0) {
        if(m_updateTilesCacheEvent) {
            m_updateTilesCacheEvent->cancel();
            m_updateTilesCacheEvent = nullptr;
        }

        m_cachedFirstVisibleFloor = getFirstVisibleFloor();
        m_cachedLastVisibleFloor = getLastVisibleFloor();

        if(m_cachedLastVisibleFloor < m_cachedFirstVisibleFloor)
            m_cachedLastVisibleFloor = m_cachedFirstVisibleFloor;

        m_cachedFloorVisibleCreatures.clear();
        m_cachedVisibleTiles.clear();

        m_mustCleanFramebuffer = true;
        m_mustDrawVisibleTilesCache = true;
        m_mustUpdateVisibleTilesCache = false;
    } else
        m_mustCleanFramebuffer = false;

    // there is no tile to render on invalid positions
    Position cameraPosition = getCameraPosition();
    if(!cameraPosition.isValid())
        return;

    int count = 0;
    bool stop = false;

    // clear current visible tiles cache
    m_cachedVisibleTiles.clear();
    m_mustDrawVisibleTilesCache = true;

    // cache visible tiles in draw order
    // draw from last floor (the lower) to first floor (the higher)
    for(int iz = m_cachedLastVisibleFloor; iz >= m_cachedFirstVisibleFloor && !stop; --iz) {
        if(m_viewRange <= FAR_VIEW) {
            const int numDiagonals = m_drawDimension.width() + m_drawDimension.height() - 1;
            // loop through / diagonals beginning at top left and going to top right
            for(int diagonal = 0; diagonal < numDiagonals && !stop; ++diagonal) {
                // loop current diagonal tiles
                for(int iy = std::min(diagonal, m_drawDimension.width() - 1), ix = std::max(diagonal - m_drawDimension.width() + 1, 0); iy >= 0 && ix < m_drawDimension.width() && !stop; --iy, ++ix) {
                    // only start really looking tiles in the desired start
                    if(count < start) {
                        count++;
                        continue;
                    }

                    // avoid rendering too much tiles at once on far views
                    if(count - start + 1 > MAX_TILE_UPDATES && m_viewRange >= HUGE_VIEW) {
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
                    count++;
                }
            }
        } else {
            static std::vector<Point> points;
            points.clear();
            //assert(m_drawDimension.width() % 2 == 0 && m_drawDimension.height() % 2 == 0);
            Point quadTopLeft(m_drawDimension.width()/2 - 1, m_drawDimension.height()/2 - 1);
            for(int step = 1; !(quadTopLeft.x < 0 && quadTopLeft.y < 0) && !stop; ++step) {
                int quadWidth = std::min(2*step, m_drawDimension.width());
                int quadHeight = std::min(2*step, m_drawDimension.height());
                int fillWidth = (quadTopLeft.x >= 0) ? quadWidth-1 : quadWidth;
                int fillHeight = (quadTopLeft.x >= 0) ? quadHeight-1 : quadHeight;
                if(quadTopLeft.y >= 0) {
                    for(int qx=0;qx<fillWidth;++qx) {
                        //TODO: remvoe this repeated code
                        // only start really looking tiles in the desired start
                        if(count < start) {
                            count++;
                            continue;
                        }

                        // avoid rendering too much tiles at once on far views
                        if(count - start + 1 > MAX_TILE_UPDATES) {
                            stop = true;
                            break;
                        }
                        points.push_back(Point(std::max(quadTopLeft.x, 0) + qx, quadTopLeft.y));
                        count++;
                    }
                }
                if(quadTopLeft.x >= 0) {
                    for(int qy=0;qy<fillHeight;++qy) {
                        // only start really looking tiles in the desired start
                        if(count < start) {
                            count++;
                            continue;
                        }

                        // avoid rendering too much tiles at once on far views
                        if(count - start + 1 > MAX_TILE_UPDATES) {
                            stop = true;
                            break;
                        }
                        points.push_back(Point(quadTopLeft.x + quadWidth-1, std::max(quadTopLeft.y, 0) + qy));
                        count++;
                    }
                }
                if(quadTopLeft.y >= 0) {
                    for(int qx=0;qx<fillWidth;++qx) {
                        // only start really looking tiles in the desired start
                        if(count < start) {
                            count++;
                            continue;
                        }

                        // avoid rendering too much tiles at once on far views
                        if(count - start + 1 > MAX_TILE_UPDATES) {
                            stop = true;
                            break;
                        }
                        points.push_back(Point(std::max(quadTopLeft.x, 0) + quadWidth-qx-1, quadTopLeft.y + quadHeight-1));
                        count++;
                    }
                }
                if(quadTopLeft.x >= 0) {
                    for(int qy=0;qy<fillHeight;++qy) {
                        // only start really looking tiles in the desired start
                        if(count < start) {
                            count++;
                            continue;
                        }

                        // avoid rendering too much tiles at once on far views
                        if(count - start + 1 > MAX_TILE_UPDATES) {
                            stop = true;
                            break;
                        }
                        points.push_back(Point(quadTopLeft.x, std::max(quadTopLeft.y, 0) + quadHeight-qy-1));
                        count++;
                    }
                }
                quadTopLeft -= Point(1,1);
            }

            for(const Point& point : points) {
                Position tilePos = cameraPosition.translated(point.x - m_virtualCenterOffset.x, point.y - m_virtualCenterOffset.y);
                // adjust tilePos to the wanted floor
                tilePos.coveredUp(cameraPosition.z - iz);
                if(const TilePtr& tile = g_map.getTile(tilePos)) {
                    // skip tiles that have nothing
                    if(tile->isEmpty())
                        continue;
                    m_cachedVisibleTiles.push_back(tile);
                }
            }
        }
    }

    if(stop) {
        // schedule next update continuation
        m_updateTilesCacheEvent = g_eventDispatcher.addEvent(std::bind(&MapView::updateVisibleTilesCache, asMapView(), count));
    }
    if(start == 0)
        m_cachedFloorVisibleCreatures = g_map.getSpectators(cameraPosition, false);
}

void MapView::onTileUpdate(const Position& pos)
{
    //if(m_viewRange == NEAR_VIEW)
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

void MapView::followCreature(const CreaturePtr& creature)
{
    m_followingCreature = creature;
    requestVisibleTilesCacheUpdate();
}

void MapView::setCameraPosition(const Position& pos)
{
    m_customCameraPosition = pos;
    m_followingCreature = nullptr;
    requestVisibleTilesCacheUpdate();
}

void MapView::setVisibleDimension(const Size& visibleDimension)
{
    if(visibleDimension.width() % 2 != 1 || visibleDimension.height() % 2 != 1) {
        logTraceError("visible dimension must be odd");
        return;
    }

    if(visibleDimension < Size(3,3)) {
        logTraceError("reach max zoom in");
        return;
    }

    int possiblesTileSizes[] = {32,16,8,4,2,1};
    int tileSize = 0;
    Size drawDimension = visibleDimension + Size(3,3);
    Size framebufferSize = m_framebuffer->getSize();
    for(int candidateTileSize : possiblesTileSizes) {
        Size candidateDrawSize = drawDimension * candidateTileSize;

        // found a valid size
        if(candidateDrawSize.width() <= framebufferSize.width() && candidateDrawSize.height() <= framebufferSize.height()) {
            tileSize = candidateTileSize;
            break;
        }
    }

    if(tileSize == 0) {
        logTraceError("reached max zoom out");
        return;
    }

    Point virtualCenterOffset = (drawDimension/2 - Size(1,1)).toPoint();
    Point visibleCenterOffset = virtualCenterOffset;

    ViewRange viewRange;
    if(tileSize >= 32 && visibleDimension.area() <= NEAR_VIEW_AREA)
        viewRange = NEAR_VIEW;
    else if(tileSize >= 16 && visibleDimension.area() <= MID_VIEW_AREA)
        viewRange = MID_VIEW;
    else if(tileSize >= 8 && visibleDimension.area() <= FAR_VIEW_AREA)
        viewRange = FAR_VIEW;
    else
        viewRange = HUGE_VIEW;

    // draw actually more than what is needed to avoid massive recalculations on far views
    if(viewRange >= FAR_VIEW) {
        Size oldDimension = drawDimension;
        drawDimension = (m_framebuffer->getSize() / tileSize);
        virtualCenterOffset += (drawDimension - oldDimension).toPoint() / 2;
    }

    bool mustUpdate = (m_drawDimension != drawDimension ||
                       m_viewRange != viewRange ||
                       m_virtualCenterOffset != virtualCenterOffset ||
                       m_visibleCenterOffset != visibleCenterOffset ||
                       m_tileSize != tileSize);

    m_visibleDimension = visibleDimension;
    m_drawDimension = drawDimension;
    m_tileSize = tileSize;
    m_viewRange = viewRange;
    m_virtualCenterOffset = virtualCenterOffset;
    m_visibleCenterOffset = visibleCenterOffset;

    if(mustUpdate)
        requestVisibleTilesCacheUpdate();
}

int MapView::getFirstVisibleFloor()
{
    // return forced first visible floor
    if(m_lockedFirstVisibleFloor != -1)
        return m_lockedFirstVisibleFloor;

    Position cameraPosition = getCameraPosition();

    // avoid rendering multifloors in far views
    if(m_viewRange >= FAR_VIEW)
        return cameraPosition.z;

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

    return firstFloor;
}

int MapView::getLastVisibleFloor()
{
    Position cameraPosition = getCameraPosition();

    // avoid rendering multifloors in far views
    if(m_viewRange >= FAR_VIEW)
        return cameraPosition.z;

    // view only underground floors when below sea level
    if(cameraPosition.z > Otc::SEA_FLOOR)
        return std::min(cameraPosition.z + Otc::AWARE_UNDEGROUND_FLOOR_RANGE, (int)Otc::MAX_Z);
    else
        return Otc::SEA_FLOOR;
}

Position MapView::getCameraPosition()
{
    if(m_followingCreature)
        return m_followingCreature->getPosition();
    return m_customCameraPosition;
}

TilePtr MapView::getTile(const Point& mousePos, const Rect& mapRect)
{
    Point relativeMousePos = mousePos - mapRect.topLeft();
    Size visibleSize = getVisibleSize();
    Position cameraPosition = getCameraPosition();

    float scaleFactor = m_tileSize / (float)Otc::TILE_PIXELS;



    float horizontalStretchFactor = visibleSize.width() / (float)mapRect.width();
    float verticalStretchFactor = visibleSize.height() / (float)mapRect.height();

    Point tilePos2D = Point(relativeMousePos.x * horizontalStretchFactor, relativeMousePos.y * verticalStretchFactor);

    if(m_followingCreature)
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

Point MapView::transformPositionTo2D(const Position& position)
{
    Position cameraPosition = getCameraPosition();
    return Point((m_virtualCenterOffset.x + (position.x - cameraPosition.x) - (cameraPosition.z - position.z)) * m_tileSize,
                 (m_virtualCenterOffset.y + (position.y - cameraPosition.y) - (cameraPosition.z - position.z)) * m_tileSize);
}
