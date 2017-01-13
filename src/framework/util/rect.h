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

#ifndef RECT_H
#define RECT_H

#include "../stdext/types.h"
#include "../const.h"
#include <sstream>

template<class T>
class TPoint;

template<class T>
class TSize;

template<class T>
class TRect
{
public:
    TRect() : x1(0), y1(0), x2(-1), y2(-1) { }
    TRect(T x, T y, T width, T height) : x1(x), y1(y), x2(x+width-1), y2(y+height-1) { }
    TRect(const TPoint<T>& topLeft, const TPoint<T>& bottomRight) : x1(topLeft.x), y1(topLeft.y), x2(bottomRight.x), y2(bottomRight.y) { }
    TRect(const TRect<T>& other) : x1(other.x1), y1(other.y1), x2(other.x2), y2(other.y2) { }
    TRect(T x, T y, const TSize<T>& size) : x1(x), y1(y), x2(x+size.width()-1), y2(y+size.height()-1) { }
    TRect(const TPoint<T>& topLeft, const TSize<T>& size) : x1(topLeft.x), y1(topLeft.y), x2(x1+size.width()-1), y2(y1+size.height()-1) { }
    TRect(const TPoint<T>& topLeft, int width, int height) : x1(topLeft.x), y1(topLeft.y), x2(x1+width-1), y2(y1+height-1) { }

    bool isNull() const { return x2 == x1 - 1 && y2 == y1 - 1; }
    bool isEmpty() const { return x1 > x2 || y1 > y2; }
    bool isValid() const { return x1 <= x2 && y1 <= y2; }

    T left() const { return x1; }
    T top() const { return y1; }
    T right() const { return x2; }
    T bottom() const { return y2; }
    T horizontalCenter() const { return x1 + (x2 - x1)/2; }
    T verticalCenter() const { return y1 + (y2 - y1)/2; }
    T x() const { return x1; }
    T y() const { return y1; }
    TPoint<T> topLeft() const { return TPoint<T>(x1, y1); }
    TPoint<T> bottomRight() const { return TPoint<T>(x2, y2); }
    TPoint<T> topRight() const { return TPoint<T>(x2, y1); }
    TPoint<T> bottomLeft() const { return TPoint<T>(x1, y2); }
    TPoint<T> topCenter() const { return TPoint<T>((x1+x2)/2, y1); }
    TPoint<T> bottomCenter() const { return TPoint<T>((x1+x2)/2, y2); }
    TPoint<T> centerLeft() const { return TPoint<T>(x1, (y1+y2)/2); }
    TPoint<T> centerRight() const { return TPoint<T>(x2, (y1+y2)/2); }
    TPoint<T> center() const { return TPoint<T>((x1+x2)/2, (y1+y2)/2); }
    T width() const { return  x2 - x1 + 1; }
    T height() const { return  y2 - y1 + 1; }
    TSize<T> size() const { return TSize<T>(width(), height()); }
    void reset() { x1 = y1 = 0; x2 = y2 = -1; }
    void clear() { x2 = x1 - 1; y2 = y1 - 1; }

    void setLeft(T pos) { x1 = pos; }
    void setTop(T pos) { y1 = pos; }
    void setRight(T pos) { x2 = pos; }
    void setBottom(T pos) { y2 = pos; }
    void setX(T x) { x1 = x; }
    void setY(T y) { y1 = y; }
    void setTopLeft(const TPoint<T> &p) { x1 = p.x; y1 = p.y; }
    void setBottomRight(const TPoint<T> &p) { x2 = p.x; y2 = p.y; }
    void setTopRight(const TPoint<T> &p) { x2 = p.x; y1 = p.y; }
    void setBottomLeft(const TPoint<T> &p) { x1 = p.x; y2 = p.y; }
    void setWidth(T width) { x2 = x1 + width - 1; }
    void setHeight(T height) { y2 = y1 + height- 1; }
    void setSize(const TSize<T>& size) { x2 = x1 + size.width() - 1; y2 = y1 + size.height() - 1; }
    void setRect(T x, T y, T width, T height) { x1 = x; y1 = y; x2 = (x + width - 1); y2 = (y + height - 1); }
    void setCoords(int left, int top, int right, int bottom) { x1 = left; y1 = top; x2 = right; y2 = bottom; }

    void expandLeft(T add) { x1 -= add; }
    void expandTop(T add) { y1 -= add; }
    void expandRight(T add) { x2 += add; }
    void expandBottom(T add) { y2 += add; }
    void expand(T top, T right, T bottom, T left) { x1 -= left; y1 -= top; x2 += right; y2 += bottom; }
    void expand(T add) { x1 -= add; y1 -= add; x2 += add; y2 += add; }

    void translate(T x, T y) { x1 += x; y1 += y; x2 += x; y2 += y; }
    void translate(const TPoint<T> &p) { x1 += p.x; y1 += p.y; x2 += p.x; y2 += p.y; }
    void resize(const TSize<T>& size) { x2 = x1 + size.width() - 1; y2 = y1 + size.height() - 1; }
    void resize(T width, T height) { x2 = x1 + width - 1; y2 = y1 + height - 1; }
    void move(T x, T y) { x2 += x - x1; y2 += y - y1; x1 = x; y1 = y; }
    void move(const TPoint<T> &p) { x2 += p.x - x1; y2 += p.y - y1; x1 = p.x; y1 = p.y; }
    void moveLeft(T pos) { x2 += (pos - x1); x1 = pos; }
    void moveTop(T pos) { y2 += (pos - y1); y1 = pos; }
    void moveRight(T pos) { x1 += (pos - x2); x2 = pos; }
    void moveBottom(T pos) { y1 += (pos - y2); y2 = pos; }
    void moveTopLeft(const TPoint<T> &p) { moveLeft(p.x); moveTop(p.y); }
    void moveBottomRight(const TPoint<T> &p) { moveRight(p.x); moveBottom(p.y); }
    void moveTopRight(const TPoint<T> &p) { moveRight(p.x); moveTop(p.y); }
    void moveBottomLeft(const TPoint<T> &p) { moveLeft(p.x); moveBottom(p.y); }
    void moveTopCenter(const TPoint<T> &p) { moveHorizontalCenter(p.x); moveTop(p.y); }
    void moveBottomCenter(const TPoint<T> &p) { moveHorizontalCenter(p.x); moveBottom(p.y); }
    void moveCenterLeft(const TPoint<T> &p) { moveLeft(p.x); moveVerticalCenter(p.y); }
    void moveCenterRight(const TPoint<T> &p) { moveRight(p.x); moveVerticalCenter(p.y); }

    TRect<T> translated(int x, int y) const { return TRect<T>(TPoint<T>(x1 + x, y1 + y), TPoint<T>(x2 + x, y2 + y)); }
    TRect<T> translated(const TPoint<T> &p) const { return TRect<T>(TPoint<T>(x1 + p.x, y1 + p.y), TPoint<T>(x2 + p.x, y2 + p.y)); }

    TRect<T> expanded(T add) const { return TRect<T>(TPoint<T>(x1 - add, y1 - add), TPoint<T>(x2 + add, y2 + add)); }

    void moveCenter(const TPoint<T> &p) {
        T w = x2 - x1;
        T h = y2 - y1;
        x1 = p.x - w/2;
        y1 = p.y - h/2;
        x2 = x1 + w;
        y2 = y1 + h;
    }
    void moveHorizontalCenter(T x) {
        T w = x2 - x1;
        x1 = x - w/2;
        x2 = x1 + w;
    }
    void moveVerticalCenter(T y) {
        T h = y2 - y1;
        y1 = y - h/2;
        y2 = y1 + h;
    }

    bool contains(const TPoint<T> &p, bool insideOnly = false) const {
        T l, r;
        if(x2 < x1 - 1) {
            l = x2;
            r = x1;
        } else {
            l = x1;
            r = x2;
        }
        if(insideOnly) {
            if(p.x <= l || p.x >= r)
                return false;
        } else {
            if(p.x < l || p.x > r)
                return false;
        }
        T t, b;
        if(y2 < y1 - 1) {
            t = y2;
            b = y1;
        } else {
            t = y1;
            b = y2;
        }
        if(insideOnly) {
            if(p.y <= t || p.y >= b)
                return false;
        } else {
            if(p.y < t || p.y > b)
                return false;
        }
        return true;
    }

    bool contains(const TRect<T> &r, bool insideOnly = false) const {
        if(contains(r.topLeft(), insideOnly) && contains(r.bottomRight(), insideOnly))
            return true;
        return false;
    }

    bool intersects(const TRect<T> &r) const {
        if(isNull() || r.isNull())
            return false;

        int l1 = x1;
        int r1 = x1;
        if(x2 - x1 + 1 < 0)
            l1 = x2;
        else
            r1 = x2;

        int l2 = r.x1;
        int r2 = r.x1;
        if(r.x2 - r.x1 + 1 < 0)
            l2 = r.x2;
        else
            r2 = r.x2;

        if(l1 > r2 || l2 > r1)
            return false;

        int t1 = y1;
        int b1 = y1;
        if(y2 - y1 + 1 < 0)
            t1 = y2;
        else
            b1 = y2;

        int t2 = r.y1;
        int b2 = r.y1;
        if(r.y2 - r.y1 + 1 < 0)
            t2 = r.y2;
        else
            b2 = r.y2;

        if(t1 > b2 || t2 > b1)
            return false;

        return true;
    }

    TRect<T> united(const TRect<T> &r) const {
        TRect<T> tmp;
        tmp.x1 = std::min<T>(x1, r.x1);
        tmp.x2 = std::max<T>(x2, r.x2);
        tmp.y1 = std::min<T>(y1, r.y1);
        tmp.y2 = std::max<T>(y2, r.y2);
        return tmp;
    }

    TRect<T> intersection(const TRect<T> &r) const {
        if(isNull())
            return r;
        if(r.isNull())
            return *this;

        int l1 = x1;
        int r1 = x1;
        if(x2 - x1 + 1 < 0)
            l1 = x2;
        else
            r1 = x2;

        int l2 = r.x1;
        int r2 = r.x1;
        if(r.x2 - r.x1 + 1 < 0)
            l2 = r.x2;
        else
            r2 = r.x2;

        int t1 = y1;
        int b1 = y1;
        if(y2 - y1 + 1 < 0)
            t1 = y2;
        else
            b1 = y2;

        int t2 = r.y1;
        int b2 = r.y1;
        if(r.y2 - r.y1 + 1 < 0)
            t2 = r.y2;
        else
            b2 = r.y2;

        TRect<T> tmp;
        tmp.x1 = std::max<int>(l1, l2);
        tmp.x2 = std::min<int>(r1, r2);
        tmp.y1 = std::max<int>(t1, t2);
        tmp.y2 = std::min<int>(b1, b2);
        return tmp;
    }

    void bind(const TRect<T> &r) {
        if(isNull() || r.isNull())
            return;

        if(right() > r.right())
            moveRight(r.right());
        if(bottom() > r.bottom())
            moveBottom(r.bottom());
        if(left() < r.left())
            moveLeft(r.left());
        if(top() < r.top())
            moveTop(r.top());
    }

    void alignIn(const TRect<T> &r, Fw::AlignmentFlag align) {
        if(align == Fw::AlignTopLeft)
            moveTopLeft(r.topLeft());
        else if(align == Fw::AlignTopRight)
            moveTopRight(r.topRight());
        else if(align == Fw::AlignTopCenter)
            moveTopCenter(r.topCenter());
        else if(align == Fw::AlignBottomLeft)
            moveBottomLeft(r.bottomLeft());
        else if(align == Fw::AlignBottomRight)
            moveBottomRight(r.bottomRight());
        else if(align == Fw::AlignBottomCenter)
            moveBottomCenter(r.bottomCenter());
        else if(align == Fw::AlignLeftCenter)
            moveCenterLeft(r.centerLeft());
        else if(align == Fw::AlignCenter)
            moveCenter(r.center());
        else if(align == Fw::AlignRightCenter)
            moveCenterRight(r.centerRight());
    }

    TRect<T>& operator=(const TRect<T>& other) { x1 = other.x1; y1 = other.y1; x2 = other.x2; y2 = other.y2; return *this;  }
    bool operator==(const TRect<T>& other) const { return (x1 == other.x1 && y1 == other.y1 && x2 == other.x2 && y2 == other.y2); }
    bool operator!=(const TRect<T>& other) const { return (x1 != other.x1 || y1 != other.y1 || x2 != other.x2 || y2 != other.y2); }

    TRect<T>& operator|=(const TRect<T>& other) { *this = united(other); return *this; }
    TRect<T>& operator&=(const TRect<T>& other) { *this = intersection(other); return *this; }

private:
    T x1, y1, x2, y2;
};

typedef TRect<int> Rect;
typedef TRect<float> RectF;

template<class T>
std::ostream& operator<<(std::ostream& out, const TRect<T>& rect)
{
    out << rect.left() << " " << rect.top() << " " << rect.width() << " " << rect.height();
    return out;
}

template<class T>
std::istream& operator>>(std::istream& in, TRect<T>& rect)
{
    T x, y , w, h;
    in >> x >> y >> w >> h;
    rect.setRect(x,y,w,h);
    return in;
}

#endif
