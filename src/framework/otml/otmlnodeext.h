/* The MIT License
 *
 * Copyright (c) 2010 OTClient, https://github.com/edubart/otclient
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


#ifndef OTMLNODEEXT_H
#define OTMLNODEEXT_H

#include <util/point.h>
#include <util/color.h>
#include <util/rect.h>
#include <util/size.h>

inline bool operator>>(const OTMLNode& node, Color& color)
{
    int r, g, b, a;
    if(node.readAt(0, &r) &&
       node.readAt(1, &g) &&
       node.readAt(2, &b)) {
        a = 255;
        node.readAt(3, &a);
        color.setRGBA(r,g,b,a);
        return true;
    }
    return false;
}

template <class T>
inline bool operator>>(const OTMLNode& node, TPoint<T>& point)
{
    T x, y;
    if(node.readAt(0, &x) && node.readAt(1, &y)) {
        point.x = x;
        point.y = y;
        return true;
    }
    return false;
}

template <class T>
inline bool operator>>(const OTMLNode& node, TSize<T>& size)
{
    T w, h;
    if(node.readAt(0, &w) && node.readAt(1, &h)) {
        size.setSize(w, h);
        return true;
    }
    return false;
}

template <class T>
inline bool operator>>(const OTMLNode& node, TRect<T>& rect)
{
    T x, y, width, height;
    if(node.readAt(0, &x) &&
       node.readAt(1, &y) &&
       node.readAt(2, &width) &&
       node.readAt(3, &height)) {
        rect.setRect(x, y, width, height);
        return true;
    }
    return false;
}

#endif // OTMLNODEEXT_H
