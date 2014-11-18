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

//PhiadariaSoft Mod: Minimap drawing fix to prevent drwaing multiple floors
#include "game.h"

#include <framework/graphics/image.h>
#include <framework/graphics/texture.h>
#include <framework/graphics/painter.h>
#include <framework/graphics/image.h>
#include <framework/graphics/framebuffermanager.h>
#include <framework/core/resourcemanager.h>
#include <framework/core/filestream.h>
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
            uint8 c = getTile(x, y).color;
            uint32 col;
            if(c != 255) {
                col = Color::from8bit(c).rgba();
                shouldDraw = true;
            } else
                col = Color::alpha.rgba();
            image->setPixel(x, y, col);
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
        
    m_minimapFloorCache.clear();
	m_minimapDrawCache.clear();
}

void Minimap::draw(const Rect& screenRect, const Position& mapCenter, float scale, const Color& color)
{
    if(screenRect.isEmpty())
        return;

    Rect mapRect = calcMapRect(screenRect, mapCenter, scale);
    g_painter->saveState();
    g_painter->setColor(color);
    g_painter->drawFilledRect(screenRect);
    g_painter->resetColor();
    g_painter->setClipRect(screenRect);

    if(MMBLOCK_SIZE*scale <= 1 || !mapCenter.isMapPosition()) {
        g_painter->restoreSavedState();
        return;
    }

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
            //g_painter->drawBoundingRect(Rect(xs,ys, MMBLOCK_SIZE * scale, MMBLOCK_SIZE * scale));
        }
    }

    g_painter->restoreSavedState();
}

Point Minimap::getTilePoint(const Position& pos, const Rect& screenRect, const Position& mapCenter, float scale)
{
    if(screenRect.isEmpty() || pos.z != mapCenter.z)
        return Point(-1,-1);

    Rect mapRect = calcMapRect(screenRect, mapCenter, scale);
    Point off = Point((mapRect.size() * scale).toPoint() - screenRect.size().toPoint())/2;
    Point posoff = (Point(pos.x,pos.y) - mapRect.topLeft())*scale;
    return posoff + screenRect.topLeft() - off + (Point(1,1)*scale)/2;
}

Position Minimap::getTilePosition(const Point& point, const Rect& screenRect, const Position& mapCenter, float scale)
{
    if(screenRect.isEmpty())
        return Position();

    Rect mapRect = calcMapRect(screenRect, mapCenter, scale);
    Point off = Point((mapRect.size() * scale).toPoint() - screenRect.size().toPoint())/2;
    Point pos2d = (point - screenRect.topLeft() + off)/scale + mapRect.topLeft();
    return Position(pos2d.x, pos2d.y, mapCenter.z);
}

Rect Minimap::getTileRect(const Position& pos, const Rect& screenRect, const Position& mapCenter, float scale)
{
    if(screenRect.isEmpty() || pos.z != mapCenter.z)
        return Rect();

    int tileSize = 32 * scale;
    Rect tileRect(0,0,tileSize, tileSize);
    tileRect.moveCenter(getTilePoint(pos, screenRect, mapCenter, scale));
    return tileRect;
}

Rect Minimap::calcMapRect(const Rect& screenRect, const Position& mapCenter, float scale)
{
    int w = screenRect.width() / scale, h = std::ceil(screenRect.height() / scale);
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
        minimapTile.speed = std::min<int>((int)std::ceil(tile->getGroundSpeed() / 10.0f), 255);
    }

#ifndef EXPLORE_MODE
	if (minimapTile != MinimapTile()) {
		MinimapBlock& block = getBlock(pos);
		Point offsetPos = getBlockOffset(Point(pos.x, pos.y));
		block.updateTile(pos.x - offsetPos.x, pos.y - offsetPos.y, minimapTile);
		block.justSaw();
	}
#else
	/*
	* Exploration mode:
	* By default OTClient is drawing all tiles which are send by the server to the minimap view
	* Some server admins might not want this and will receive a working patch for this. Simply add EXPLORE_MODE to build dependencies to make it work
	* While still all tiles are received from the server, only the visible ones on the same floor as the player including tiles inside the view range
	* will be drawn. All other tiles will be added to a caching system and drawn later in case the player walks up or down a floor
	* To prevent the Client from freezing or slowing done after a long play time, the cache will be flushed of all tiles which have been drawn already
	* or which are far out from our view range
	*/

	//First we need to get the player object
	LocalPlayerPtr localPlayer = g_game.getLocalPlayer();

	// Do nothing in case the player object is not valid
	if (localPlayer == nullptr) {
		return;
	}

	// Get the current Z-Position and cache the current Player offset based on the player position and tile position
	short playerPosZ = localPlayer->getPosition().z;

	//Ensure each position is only cached once, but still check for a possible change (might happen in case we add a wall for example by script)
	if (m_minimapDrawCache[pos] == minimapTile) {
		return;
	}
	m_minimapDrawCache[pos] = minimapTile;

	//Like the old system, check if there is actually a minimap tile available to draw
	if (minimapTile != MinimapTile()) {
		MinimapBlock& block = getBlock(pos);
		Point offsetPos = getBlockOffset(Point(pos.x, pos.y));

		//Create new cache object
		MinimapCache minimapCache;
		minimapCache.drawn = false;
		minimapCache.minimapTile = minimapTile;
		minimapCache.pos = pos;
		minimapCache.block = &block;
		minimapCache.offsetPos = offsetPos;

		//In case the player object is valid and we are capable to view this tile, draw it without caching
		if (playerPosZ == pos.z && std::abs(localPlayer->getPosition().x - pos.x) <= 9 && std::abs(localPlayer->getPosition().y - pos.y) <= 7) {
			block.updateTile(pos.x - offsetPos.x, pos.y - offsetPos.y, minimapTile);
			block.justSaw();
			return;
		}

		//In case we are not able to view the map tile we should use our cache
		m_minimapFloorCache[pos.z].push_back(minimapCache);
	}

	//Take a look inside our cache for the current players floor and check if there is data we can use
	auto cacheIt = m_minimapFloorCache.find(playerPosZ);
	if (cacheIt != m_minimapFloorCache.end()) {
		//Now loop through the entire tile cache for this floor and draw the minimap for all tiles in range
		auto &cache = m_minimapFloorCache[playerPosZ];
		for (auto &it = cache.begin(); it != cache.end(); it++) {
			//We might already have drawn this tile. Lets remove it to speed up everything
			if (it->drawn == true) {
				m_minimapFloorCache[playerPosZ].erase(it);
				it = cache.begin();
				continue;
			}

			//To prevent the Client from slowing down after a long playtime, remove all cached data which are outside our view range
			if (std::abs(localPlayer->getPosition().x - it->pos.x) > 18 || std::abs(localPlayer->getPosition().y - it->pos.y) > 14) {
				m_minimapDrawCache[it->pos] = MinimapTile();
				m_minimapFloorCache[playerPosZ].erase(it);
				it = cache.begin();
				continue;
			}

			//Check if we are actually capable to view this tile from our current position and skip in case we are not
			if (std::abs(localPlayer->getPosition().x - it->pos.x) > 9 || std::abs(localPlayer->getPosition().y - it->pos.y) > 7) {
				continue;
			}

			//Looks like we passed all checks and are capable to view this tile. Lets draw the minimap data
			it->block->updateTile(it->pos.x - it->offsetPos.x, it->pos.y - it->offsetPos.y, it->minimapTile);
			it->block->justSaw();
			it->drawn = true;
			m_minimapFloorCache[playerPosZ].erase(it);
			it = cache.begin();
		}
	}
#endif
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

bool Minimap::loadImage(const std::string& fileName, const Position& topLeft, float colorFactor)
{
    if(colorFactor <= 0.01f)
        colorFactor = 1.0f;

    try {
        ImagePtr image = Image::load(fileName);

        uint8 waterc = Color::to8bit(std::string("#3300cc"));

        // non pathable colors
        Color nonPathableColors[] = {
            std::string("#ffff00"), // yellow
        };

        // non walkable colors
        Color nonWalkableColors[] = {
            std::string("#000000"), // oil, black
            std::string("#006600"), // trees, dark green
            std::string("#ff3300"), // walls, red
            std::string("#666666"), // mountain, grey
            std::string("#ff6600"), // lava, orange
            std::string("#00ff00"), // positon
            std::string("#ccffff"), // ice, very light blue
        };

        for(int y=0;y<image->getHeight();++y) {
            for(int x=0;x<image->getWidth();++x) {
                Color color = *(uint32*)image->getPixel(x,y);
                uint8 c = Color::to8bit(color * colorFactor);
                int flags = 0;

                if(c == waterc || color.a() == 0) {
                    flags |= MinimapTileNotWalkable;
                    c = 255; // alpha
                }

                if(flags != 0) {
                    for(Color &col : nonWalkableColors) {
                        if(col == color) {
                            flags |= MinimapTileNotWalkable;
                            break;
                        }
                    }
                }

                if(flags != 0) {
                    for(Color &col : nonPathableColors) {
                        if(col == color) {
                            flags |= MinimapTileNotPathable;
                            break;
                        }
                    }
                }

                if(c == 255)
                    continue;

                Position pos(topLeft.x + x, topLeft.y + y, topLeft.z);
                MinimapBlock& block = getBlock(pos);
                Point offsetPos = getBlockOffset(Point(pos.x, pos.y));
                MinimapTile& tile = block.getTile(pos.x - offsetPos.x, pos.y - offsetPos.y);
                if(!(tile.flags & MinimapTileWasSeen)) {
                    tile.color = c;
                    tile.flags = flags;
                    block.mustUpdate();
                }
            }
        }
        return true;
    } catch(stdext::exception& e) {
        g_logger.error(stdext::format("failed to load OTMM minimap: %s", e.what()));
        return false;
    }
}

void Minimap::saveImage(const std::string& fileName, const Rect& mapRect)
{
    //TODO
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
            block.justSaw();
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
                if(!block.wasSeen())
                    continue;

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
