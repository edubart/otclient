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

#ifndef POINT_H
#define POINT_H

#include "../stdext/types.h"
#include <sstream>
#include <cmath>

template<class T>
class TSize;

template<class T>
class TPoint
{
public:
    TPoint() : x(0), y(0) {}
    TPoint(T x, T y) : x(x), y(y) { }
    TPoint(const TPoint<T>& other) : x(other.x), y(other.y) { }

    bool isNull() const { return x==0 && y==0; }
    TSize<T> toSize() const { return TSize<T>(x, y); }

    TPoint<T> operator-() const { return TPoint<T>(-x, -y); }

    TPoint<T> operator+(const TPoint<T>& other) const { return TPoint<T>(x + other.x, y + other.y); }
    TPoint<T>& operator+=(const TPoint<T>& other) { x+=other.x; y+=other.y; return *this; }
    TPoint<T> operator-(const TPoint<T>& other) const { return TPoint<T>(x - other.x, y - other.y); }
    TPoint<T>& operator-=(const TPoint<T>& other) { x-=other.x; y-=other.y; return *this; }
    TPoint<T> operator*(const TPoint<T>& other) const { return TPoint<T>(x * other.x, y * other.y); }
    TPoint<T>& operator*=(const TPoint<T>& other) { x*=other.x; y*=other.y; return *this; }
    TPoint<T> operator/(const TPoint<T>& other) const { return TPoint<T>(x/other.x, y/other.y); }
    TPoint<T>& operator/=(const TPoint<T>& other)   { x/=other.x; y/=other.y; return *this; }

    TPoint<T> operator+(T other) const { return TPoint<T>(x + other, y + other); }
    TPoint<T>& operator+=(T other) { x+=other; y+=other; return *this; }
    TPoint<T> operator-(T other) const { return TPoint<T>(x - other, y - other); }
    TPoint<T>& operator-=(T other) { x-=other; y-=other; return *this; }
    TPoint<T> operator*(float v) const { return TPoint<T>(x*v, y*v); }
    TPoint<T>& operator*=(float v) { x*=v; y*=v; return *this; }
    TPoint<T> operator/(float v) const { return TPoint<T>(x/v, y/v); }
    TPoint<T>& operator/=(float v) { x/=v; y/=v; return *this; }

    TPoint<T> operator&(int a) { return TPoint<T>(x & a, y & a); }
    TPoint<T>& operator&=(int a) { x &= a; y &= a; return *this; }

    bool operator<=(const TPoint<T>&other) const { return x<=other.x && y<=other.y; }
    bool operator>=(const TPoint<T>&other) const { return x>=other.x && y>=other.y; }
    bool operator<(const TPoint<T>&other) const { return x<other.x && y<other.y; }
    bool operator>(const TPoint<T>&other) const { return x>other.x && y>other.y; }

    TPoint<T>& operator=(const TPoint<T>& other) { x = other.x; y = other.y; return *this; }
    bool operator==(const TPoint<T>& other) const { return other.x==x && other.y==y; }
    bool operator!=(const TPoint<T>& other) const { return other.x!=x || other.y!=y; }

    float length() const { return sqrt((float)(x*x + y*y)); }
    T manhattanLength() const { return std::abs(x) + std::abs(y); }

    float distanceFrom(const TPoint<T>& other) const {
        return TPoint<T>(x - other.x, y - other.y).length();
    }

    T x, y;
};

typedef TPoint<int> Point;
typedef TPoint<float> PointF;

template<class T>
std::ostream& operator<<(std::ostream& out, const TPoint<T>& point)
{
    out << point.x << " " << point.y;
    return out;
}

template<class T>
std::istream& operator>>(std::istream& in, TPoint<T>& point)
{
    in >> point.x;
    in >> point.y;
    return in;
}

#endif
