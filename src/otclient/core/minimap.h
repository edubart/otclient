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


#ifndef MINIMAP_H
#define MINIMAP_H

#include <otclient/global.h>
#include <framework/graphics/image.h>
/*
enum {
    MINIMAP_AREA_SIZE = 32
};

struct MinimapArea
{
    ImagePtr img;
    TexturePtr tex;
    uint8 colors[MINIMAP_AREA_SIZE][MINIMAP_AREA_SIZE];
    Boolean<true> mustUpdate;
};

class Minimap
{

public:
    void init();
    void terminate();

    void loadOtmm();
    void saveOtmm();

    void updateTile(const Position& pos, uint8 color);

private:

    struct MinimaAreaHasher : std::unary_function<Position, std::size_t> {
        std::size_t operator()(const Position& pos) const {
            return ((pos.x/MINIMAP_AREA_SIZE) * 0x8000 + (pos.y/MINIMAP_AREA_SIZE)) * 16 + pos.z;
        }
    };
    std::unordered_map<Position, ImagePtr, MinimaAreaHasher> m_areas;
};

extern Minimap g_minimap;
*/
#endif
