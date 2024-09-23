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

#ifndef MAPVIEW_OPTIMIZED_H
#define MAPVIEW_OPTIMIZED_H

#include "declarations.h"
#include "const.h"

class MapViewControl {
public:
    static const int32_t maxMapviewX = 8;
    static const int32_t maxMapviewY = 6;

    MapViewControl(const Otc::Direction directionWalking = Otc::InvalidDirection);

    bool isValid(const TilePtr& tile, const Position cameraPosition) const;

    int top() const { return m_top; }
    int right() const { return m_right; }
    int bottom() const { return m_bottom; }
    int left() const { return m_left; }

private:
    int m_top;
    int m_right;
    int m_bottom;
    int m_left;
};

#endif
