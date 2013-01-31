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
#include <framework/core/resourcemanager.h>
#include <framework/core/filestream.h>
#include <boost/concept_check.hpp>
#include <zlib.h>

Minimap g_minimap;

void MinimapBlock::clean()
{
    m_tiles.fill(MinimapTile());
    m_texture.reset();
    m_mustUpdate = false;
}

void MinimapBlock::update()
{
    if(!m_mustUpdate)
        return;

    ImagePtr image(new Image(Size(MMBLOCK_SIZE, MMBLOCK_SIZE)));

    bool shouldDraw = false;
    for(int x=0;x<MMBLOCK_SIZE;++x) {
        for(int y=0;y<MMBLOCK_SIZE;++y) {
            uint32 col = Color::from8bit(getTile(x, y).color).rgba();
            image->setPixel(x, y, col);
            if(col != 0)
                shouldDraw = true;
        }
    }

    if(shouldDraw) {
        if(!m_texture) {
            m_texture = TexturePtr(new Texture(image, true));
        } else {
            m_texture->uploadPixels(image, true);
        }
    } else
        m_texture.reset();

    m_mustUpdate = false;
}

void MinimapBlock::updateTile(int x, int y, const MinimapTile& tile)
{
    if(m_tiles[getTileIndex(x,y)].color != tile.color)
        m_mustUpdate = true;

    m_tiles[getTileIndex(x,y)] = tile;
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
    if(screenRect.isEmpty() || MMBLOCK_SIZE*scale <= 1 || !mapCenter.isMapPosition())
        return ;

    Rect mapRect = calcMapRect(screenRect, mapCenter, scale);
    g_painter->saveState();
    g_painter->setColor(Color::black);
    g_painter->drawFilledRect(screenRect);
    g_painter->resetColor();
    g_painter->setClipRect(screenRect);

    Point blockOff = getBlockOffset(mapRect.topLeft());
    Point off = Point((mapRect.size() * scale).toPoint() - screenRect.size().toPoint())/2;
    Point start = screenRect.topLeft() -(mapRect.topLeft() - blockOff)*scale - off;

    for(int y = blockOff.y, ys = start.y;ys<screenRect.bottom();y += MMBLOCK_SIZE, ys += MMBLOCK_SIZE*scale) {
        if(y < 0 || y >= 65536)
            continue;

        for(int x = blockOff.x, xs = start.x;xs<screenRect.right();x += MMBLOCK_SIZE, xs += MMBLOCK_SIZE*scale) {
            if(x < 0 || x >= 65536)
                continue;

            Position blockPos(x, y, mapCenter.z);
            if(!hasBlock(blockPos))
                continue;

            MinimapBlock& block = getBlock(Position(x, y, mapCenter.z));
            block.update();

            const TexturePtr& tex = block.getTexture();
            if(tex) {
                Rect src(0, 0, MMBLOCK_SIZE, MMBLOCK_SIZE);
                Rect dest(Point(xs,ys), src.size() * scale);

                tex->setSmooth(scale < 1.0f);
                g_painter->drawTexturedRect(dest, tex, src);
            }
        }
    }

    g_painter->restoreSavedState();
}

Point Minimap::getPoint(const Position& pos, const Rect& screenRect, const Position& mapCenter, float scale)
{
    if(screenRect.isEmpty() || MMBLOCK_SIZE*scale <= 1 || !mapCenter.isMapPosition())
        return Point(-1,-1);

    Rect mapRect = calcMapRect(screenRect, mapCenter, scale);
    Point off = Point((mapRect.size() * scale).toPoint() - screenRect.size().toPoint())/2;
    Point posoff = (Point(pos.x,pos.y) - mapRect.topLeft())*scale;
    return posoff + screenRect.topLeft() - off + (Point(1,1)*scale)/2;
}

Position Minimap::getPosition(const Point& point, const Rect& screenRect, const Position& mapCenter, float scale)
{
    if(screenRect.isEmpty() || MMBLOCK_SIZE*scale <= 1 || !mapCenter.isMapPosition())
        return Position();

    Rect mapRect = calcMapRect(screenRect, mapCenter, scale);
    Point off = Point((mapRect.size() * scale).toPoint() - screenRect.size().toPoint())/2;
    Point pos2d = (point - screenRect.topLeft() + off)/scale + mapRect.topLeft();
    return Position(pos2d.x, pos2d.y, mapCenter.z);
}

Rect Minimap::calcMapRect(const Rect& screenRect, const Position& mapCenter, float scale)
{
    int w, h;
    do {
        w = std::ceil(screenRect.width() / scale);
        h = std::ceil(screenRect.height() / scale);
        if(w % 2 == 0)
            w++;
        if(h % 2 == 0)
            h++;
        scale *= 2;
    } while(w > 8192 || h > 8192);
    Rect mapRect(0,0,w,h);
    mapRect.moveCenter(Point(mapCenter.x, mapCenter.y));
    return mapRect;
}


void Minimap::updateTile(const Position& pos, const TilePtr& tile)
{
    MinimapTile minimapTile;
    if(tile) {
        minimapTile.color = tile->getMinimapColorByte();
        minimapTile.flags |= MinimapTileWasSeen;
        if(!tile->isWalkable(true))
            minimapTile.flags |= MinimapTileNotWalkable;
        if(!tile->isPathable())
            minimapTile.flags |= MinimapTileNotPathable;
        minimapTile.speed = std::min((int)std::ceil(tile->getGroundSpeed() / 10.0f), 255);
    }

    if(minimapTile != MinimapTile()) {
        MinimapBlock& block = getBlock(pos);
        Point offsetPos = getBlockOffset(Point(pos.x, pos.y));
        block.updateTile(pos.x - offsetPos.x, pos.y - offsetPos.y, minimapTile);
    }
}

const MinimapTile& Minimap::getTile(const Position& pos)
{
    static MinimapTile nulltile;
    if(pos.z <= Otc::MAX_Z && hasBlock(pos)) {
        MinimapBlock& block = getBlock(pos);
        Point offsetPos = getBlockOffset(Point(pos.x, pos.y));
        return block.getTile(pos.x - offsetPos.x, pos.y - offsetPos.y);
    }
    return nulltile;
}

bool Minimap::loadOtmm(const std::string& fileName)
{
    try {
        FileStreamPtr fin = g_resources.openFile(fileName);
        if(!fin)
            stdext::throw_exception("unable to open file");

        fin->cache();

        uint32 signature = fin->getU32();
        if(signature != OTMM_SIGNATURE)
            stdext::throw_exception("invalid OTMM file");

        uint16 start = fin->getU16();
        uint16 version = fin->getU16();
        fin->getU32(); // flags

        switch(version) {
            case 1: {
                fin->getString(); // description
                break;
            }
            default:
                stdext::throw_exception("OTMM version not supported");
        }

        fin->seek(start);

        uint blockSize = MMBLOCK_SIZE * MMBLOCK_SIZE * sizeof(MinimapTile);
        std::vector<uchar> compressBuffer(compressBound(blockSize));

        while(true) {
            Position pos;
            pos.x = fin->getU16();
            pos.y = fin->getU16();
            pos.z = fin->getU8();

            // end of file
            if(!pos.isValid())
                break;

            MinimapBlock& block = getBlock(pos);
            ulong len = fin->getU16();
            ulong destLen = blockSize;
            fin->read(compressBuffer.data(), len);
            int ret = uncompress((uchar*)&block.getTiles(), &destLen, compressBuffer.data(), len);
            assert(ret == Z_OK);
            assert(destLen == blockSize);
            block.mustUpdate();
        }

        fin->close();
        return true;
    } catch(stdext::exception& e) {
        g_logger.error(stdext::format("failed to load OTMM minimap: %s", e.what()));
        return false;
    }
}

void Minimap::saveOtmm(const std::string& fileName)
{
    try {
        stdext::timer saveTimer;

        FileStreamPtr fin = g_resources.createFile(fileName);
        fin->cache();

        //TODO: compression flag with zlib
        uint32 flags = 0;

        // header
        fin->addU32(OTMM_SIGNATURE);
        fin->addU16(0); // data start, will be overwritten later
        fin->addU16(OTMM_VERSION);
        fin->addU32(flags);

        // version 1 header
        fin->addString("OTMM 1.0"); // description

        // go back and rewrite where the map data starts
        uint32 start = fin->tell();
        fin->seek(4);
        fin->addU16(start);
        fin->seek(start);

        uint blockSize = MMBLOCK_SIZE * MMBLOCK_SIZE * sizeof(MinimapTile);
        std::vector<uchar> compressBuffer(compressBound(blockSize));
        const int COMPRESS_LEVEL = 3;

        for(uint8_t z = 0; z <= Otc::MAX_Z; ++z) {
            for(auto& it : m_tileBlocks[z]) {
                int index = it.first;
                MinimapBlock& block = it.second;
                Position pos = getIndexPosition(index, z);
                fin->addU16(pos.x);
                fin->addU16(pos.y);
                fin->addU8(pos.z);

                ulong len = blockSize;
                int ret = compress2(compressBuffer.data(), &len, (uchar*)&block.getTiles(), blockSize, COMPRESS_LEVEL);
                assert(ret == Z_OK);
                fin->addU16(len);
                fin->write(compressBuffer.data(), len);
            }
        }

        // end of file
        Position invalidPos;
        fin->addU16(invalidPos.x);
        fin->addU16(invalidPos.y);
        fin->addU8(invalidPos.z);

        fin->flush();

        fin->close();
    } catch(stdext::exception& e) {
        g_logger.error(stdext::format("failed to save OTMM minimap: %s", e.what()));
    }
}
