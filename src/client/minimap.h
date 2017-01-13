/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
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


#ifndef MINIMAP_H
#define MINIMAP_H

#include "declarations.h"
#include <framework/graphics/declarations.h>

enum {
    MMBLOCK_SIZE = 64,
    OTMM_SIGNATURE = 0x4D4d544F,
    OTMM_VERSION = 1
};

enum MinimapTileFlags {
    MinimapTileWasSeen = 1,
    MinimapTileNotPathable = 2,
    MinimapTileNotWalkable = 4
};

#pragma pack(push,1) // disable memory alignment
struct MinimapTile
{
    MinimapTile() : flags(0), color(255), speed(10) { }
    uint8 flags;
    uint8 color;
    uint8 speed;
    bool hasFlag(MinimapTileFlags flag) const { return flags & flag; }
    int getSpeed() const { return speed * 10; }
    bool operator==(const MinimapTile& other) const { return color == other.color && flags == other.flags && speed == other.speed; }
    bool operator!=(const MinimapTile& other) const { return !(*this == other); }
};

class MinimapBlock
{
public:
    void clean();
    void update();
    void updateTile(int x, int y, const MinimapTile& tile);
    MinimapTile& getTile(int x, int y) { return m_tiles[getTileIndex(x,y)]; }
    void resetTile(int x, int y) { m_tiles[getTileIndex(x,y)] = MinimapTile(); }
    uint getTileIndex(int x, int y) { return ((y % MMBLOCK_SIZE) * MMBLOCK_SIZE) + (x % MMBLOCK_SIZE); }
    const TexturePtr& getTexture() { return m_texture; }
    std::array<MinimapTile, MMBLOCK_SIZE *MMBLOCK_SIZE>& getTiles() { return m_tiles; }
    void mustUpdate() { m_mustUpdate = true; }
    void justSaw() { m_wasSeen = true; }
    bool wasSeen() { return m_wasSeen; }
private:
    TexturePtr m_texture;
    std::array<MinimapTile, MMBLOCK_SIZE *MMBLOCK_SIZE> m_tiles;
    stdext::boolean<true> m_mustUpdate;
    stdext::boolean<false> m_wasSeen;
};

#pragma pack(pop)

class Minimap
{

public:
    void init();
    void terminate();

    void clean();

    void draw(const Rect& screenRect, const Position& mapCenter, float scale, const Color& color);
    Point getTilePoint(const Position& pos, const Rect& screenRect, const Position& mapCenter, float scale);
    Position getTilePosition(const Point& point, const Rect& screenRect, const Position& mapCenter, float scale);
    Rect getTileRect(const Position& pos, const Rect& screenRect, const Position& mapCenter, float scale);

    void updateTile(const Position& pos, const TilePtr& tile);
    const MinimapTile& getTile(const Position& pos);

    bool loadImage(const std::string& fileName, const Position& topLeft, float colorFactor);
    void saveImage(const std::string& fileName, const Rect& mapRect);
    bool loadOtmm(const std::string& fileName);
    void saveOtmm(const std::string& fileName);

private:
    Rect calcMapRect(const Rect& screenRect, const Position& mapCenter, float scale);
    bool hasBlock(const Position& pos) { return m_tileBlocks[pos.z].find(getBlockIndex(pos)) != m_tileBlocks[pos.z].end(); }
    MinimapBlock& getBlock(const Position& pos) { return m_tileBlocks[pos.z][getBlockIndex(pos)]; }
    Point getBlockOffset(const Point& pos) { return Point(pos.x - pos.x % MMBLOCK_SIZE,
                                                          pos.y - pos.y % MMBLOCK_SIZE); }
    Position getIndexPosition(int index, int z) { return Position((index % (65536 / MMBLOCK_SIZE))*MMBLOCK_SIZE,
                                                                  (index / (65536 / MMBLOCK_SIZE))*MMBLOCK_SIZE, z); }
    uint getBlockIndex(const Position& pos) { return ((pos.y / MMBLOCK_SIZE) * (65536 / MMBLOCK_SIZE)) + (pos.x / MMBLOCK_SIZE); }
    std::unordered_map<uint, MinimapBlock> m_tileBlocks[Otc::MAX_Z+1];
};

extern Minimap g_minimap;

#endif
