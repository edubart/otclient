/*
 * Copyright (c) 2010-2013 OTClient <https://github.com/edubart/otclient>
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


#include "minimap.h"
#include "tile.h"
#include <framework/graphics/image.h>
#include <framework/graphics/texture.h>
#include <framework/graphics/painter.h>
#include <framework/graphics/framebuffermanager.h>
#include <boost/concept_check.hpp>

Minimap g_minimap;

void MinimapBlock::updateImage()
{
    if(!m_image)
        m_image = ImagePtr(new Image(Size(MMBLOCK_SIZE, MMBLOCK_SIZE)));
    else
        m_image->resize(Size(MMBLOCK_SIZE, MMBLOCK_SIZE));

    for(int x=0;x<MMBLOCK_SIZE;++x)
        for(int y=0;y<MMBLOCK_SIZE;++y)
            m_image->setPixel(x, y, Color::from8bit(getTile(x, y).color).rgba());
}

void MinimapBlock::updateTexture()
{
    if(!m_image)
        return;

    if(!m_texture) {
        m_texture = TexturePtr(new Texture(m_image, true));
    } else {
        m_texture->uploadPixels(m_image, true);
    }
}

void MinimapBlock::clean()
{
    m_tiles.fill(MinimapTile());
    m_image.reset();
    m_texture.reset();
    m_shouldDraw = false;
    m_mustUpdate = false;
}

void MinimapBlock::update()
{
    if(!m_mustUpdate)
        return;

    if(m_shouldDraw) {
        updateImage();
        updateTexture();
    }

    m_mustUpdate = false;
}

void MinimapBlock::updateTile(int x, int y, const MinimapTile& tile)
{
    if(!(m_tiles[getTileIndex(x,y)] == tile)) {
        m_tiles[getTileIndex(x,y)] = tile;

        if(tile.color != 0)
            m_shouldDraw = true;
        m_mustUpdate = true;
    }
}
    
void Minimap::init()
{

}

void Minimap::terminate()
{
    clean();
}

void Minimap::clean()
{
    for(int i=0;i<=Otc::MAX_Z;++i)
        m_tileBlocks[i].clear();
}

void Minimap::draw(const Rect& screenRect, const Position& mapCenter, float scale)
{
    if(screenRect.isEmpty())
        return;

    if(MMBLOCK_SIZE*scale <= 1)
        return;

    Size mapSize = screenRect.size() / scale;
    while(mapSize.width() > 8192 || mapSize.height() > 8192) {
        scale *= 2;
        mapSize = screenRect.size() / scale;
    }
    Rect mapRect(0, 0, mapSize);
    mapRect.moveCenter(Point(mapCenter.x, mapCenter.y));

    g_painter->saveState();
    g_painter->setColor(Color::black);
    g_painter->drawFilledRect(screenRect);
    g_painter->resetColor();
    g_painter->setClipRect(screenRect);
    g_painter->translate(screenRect.topLeft());

    Point p = getBlockOffset(mapRect.topLeft() - Point(1,1));
    g_painter->translate(-(mapRect.topLeft() - p)*scale);

    if(scale > 1.0f)
        g_painter->translate(Point(1,1) * scale * 0.5f);

    for(int y = p.y, ys = 0;y<=mapRect.bottom();y += MMBLOCK_SIZE, ys += MMBLOCK_SIZE*scale) {
        if(y < 0 || y >= 65536 - MMBLOCK_SIZE)
            continue;

        for(int x = p.x, xs = 0;x<=mapRect.right();x += MMBLOCK_SIZE, xs += MMBLOCK_SIZE*scale) {
            if(x < 0 || x >= 65536 - MMBLOCK_SIZE)
                continue;

            MinimapBlock& block = getBlock(Position(x, y, mapCenter.z));
            block.update();

            if(block.shouldDraw()) {
                Rect src(0, 0, MMBLOCK_SIZE, MMBLOCK_SIZE);
                Rect dest(Point(xs,ys), src.size() * scale);
                const TexturePtr& tex = block.getTexture();

                tex->setSmooth(scale < 1.0f);
                g_painter->drawTexturedRect(dest, tex, src);
            }
        }
    }

    g_painter->restoreSavedState();
}

Position Minimap::getPosition(const Point& point, const Rect& screenRect, const Position& mapCenter, float scale)
{
    if(screenRect.isEmpty())
        return Position();

    if(MMBLOCK_SIZE*scale <= 1)
        return Position();

    Position pos(mapCenter);

    Size mapSize = screenRect.size() / scale;
    while(mapSize.width() > 8192 || mapSize.height() > 8192) {
        scale *= 2;
        mapSize = screenRect.size() / scale;
    }
    Rect mapRect(0, 0, mapSize);
    mapRect.moveCenter(Point(mapCenter.x, mapCenter.y));

    Point p = (point - screenRect.topLeft() - Point(1,1) * scale * 0.5f)/scale + mapRect.topLeft();

    pos.x = p.x;
    pos.y = p.y;

    return pos;
}

void Minimap::updateTile(const Position& pos, const TilePtr& tile)
{
    MinimapBlock& block = getBlock(pos);
    Point offsetPos = getBlockOffset(Point(pos.x, pos.y));

    MinimapTile minimapTile;
    if(tile) {
        minimapTile.color = tile->getMinimapColorByte();
        if(tile->isWalkable())
            minimapTile.flags |= MinimapTileWalkable;
        if(tile->isPathable())
            minimapTile.flags |= MinimapTilePathable;
        if(tile->changesFloor())
            minimapTile.flags |= MinimapTileChangesFloor;
    }

    block.updateTile(pos.x - offsetPos.x, pos.y - offsetPos.y, minimapTile);
}

bool Minimap::checkTileProperty(const Position& pos, int flags)
{
    MinimapBlock& block = getBlock(pos);
    Point offsetPos = getBlockOffset(Point(pos.x, pos.y));
    return block.getTile(pos.x - offsetPos.x, pos.y - offsetPos.y).flags & flags;
}

void Minimap::loadOtmm(const std::string& fileName)
{

}

void Minimap::saveOtmm(const std::string& fileName)
{

}
