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


#ifndef MINIMAP_H
#define MINIMAP_H

#include "declarations.h"
#include <framework/graphics/declarations.h>

enum {
    MMBLOCK_SIZE = 64
};

enum MinimapTileFlags {
    MinimapTilePathable = 1,
    MinimapTileWalkable = 2,
    MinimapTileChangesFloor = 4
};

#pragma pack(push,1) // disable memory alignment
struct MinimapTile
{
    MinimapTile() : flags(0), color(0) { }
    uint8 flags;
    uint8 color;

    bool operator==(const MinimapTile& other) { return color == other.color && flags == other.flags; }
};
#pragma pack(pop)

class MinimapBlock
{
public:
    void updateImage();
    void updateTexture();
    void clean();
    void update();
    void updateTile(int x, int y, const MinimapTile& tile);
    MinimapTile& getTile(int x, int y) { return m_tiles[getTileIndex(x,y)]; }
    void resetTile(int x, int y) { m_tiles[getTileIndex(x,y)] = MinimapTile(); }
    uint getTileIndex(int x, int y) { return ((y % MMBLOCK_SIZE) * MMBLOCK_SIZE) + (x % MMBLOCK_SIZE); }
    const TexturePtr& getTexture() { return m_texture; }
    std::array<MinimapTile, MMBLOCK_SIZE *MMBLOCK_SIZE> getTiles() { return m_tiles; }
    bool shouldDraw() { return m_shouldDraw; }

private:
    ImagePtr m_image;
    TexturePtr m_texture;
    stdext::boolean<false> m_shouldDraw;
    std::array<MinimapTile, MMBLOCK_SIZE *MMBLOCK_SIZE> m_tiles;
    stdext::boolean<true> m_mustUpdate;
};

class Minimap
{

public:
    void init();
    void terminate();

    void clean();

    void draw(const Rect& screenRect, const Position& mapCenter, float scale);
    Position getPosition(const Point& point, const Rect& screenRect, const Position& mapCenter, float scale);

    void updateTile(const Position& pos, const TilePtr& tile);
    bool checkTileProperty(const Position& pos, int flags);

    void loadOtmm(const std::string& fileName);
    void saveOtmm(const std::string& fileName);

private:
    MinimapBlock& getBlock(const Position& pos) { return m_tileBlocks[pos.z][getBlockIndex(pos)]; }
    Point getBlockOffset(const Point& pos) { return Point(pos.x - pos.x % MMBLOCK_SIZE,
                                                          pos.y - pos.y % MMBLOCK_SIZE); }
    uint getBlockIndex(const Position& pos) { return ((pos.y / MMBLOCK_SIZE) * (65536 / MMBLOCK_SIZE)) + (pos.x / MMBLOCK_SIZE); }
    std::unordered_map<uint, MinimapBlock> m_tileBlocks[Otc::MAX_Z+1];
};

extern Minimap g_minimap;

#endif
