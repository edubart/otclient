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


#ifndef POINT_H
#define POINT_H

#include "prerequisites.h"

template <class T>
class TSize;

template <class T>
class TPoint
{
public:
    inline TPoint() : x(0), y(0) {};
    inline TPoint(T x, T y) : x(x), y(y) { };
    inline TPoint(const TPoint<T>& other) : x(other.x), y(other.y) { };

    inline bool isNull() const { return x==0 && y==0; }
    inline TSize<T> toSize() const { return TSize<T>(x, y); }

    inline TPoint<T> operator-() const { return TPoint<T>(-x, -y); }
    inline TPoint<T> operator+(const TPoint<T>& other) const { return TPoint<T>(x + other.x, y + other.y);   }
    inline TPoint<T>& operator+=(const TPoint<T>& other) { x+=other.x; y+=other.y; return *this; }
    inline TPoint<T> operator-(const TPoint<T>& other) const { return TPoint<T>(x - other.x, y - other.y);   }
    inline TPoint<T>& operator-=(const TPoint<T>& other) { x-=other.x; y-=other.y; return *this; }
    inline TPoint<T> operator*(const TPoint<T>& other) const { return TPoint<T>(x * other.x, y * other.y);   }
    inline TPoint<T>& operator*=(const TPoint<T>& other) { x*=other.x; y*=other.y; return *this; }
    inline TPoint<T> operator*(const T v) const { return TPoint<T>(x * v, y * v);  }
    inline TPoint<T>& operator*=(const T v) { x*=v; y*=v; return *this; }
    inline TPoint<T> operator/(const TPoint<T>& other) const { return TPoint<T>(x/other.x, y/other.y);   }
    inline TPoint<T>& operator/=(const TPoint<T>& other)   { x/=other.x; y/=other.y; return *this; }
    inline TPoint<T> operator/(const T v) const { return TPoint<T>(x/v, y/v);  }
    inline TPoint<T>& operator/=(const T v) { x/=v; y/=v; return *this; }

    inline bool operator<=(const TPoint<T>&other) const { return x<=other.x && y<=other.y; }
    inline bool operator>=(const TPoint<T>&other) const { return x>=other.x && y>=other.y; }
    inline bool operator<(const TPoint<T>&other) const { return x<other.x && y<other.y; }
    inline bool operator>(const TPoint<T>&other) const { return x>other.x && y>other.y; }

    inline TPoint<T>& operator=(const TPoint<T>& other) { x = other.x; y = other.y; return *this; }
    inline bool operator==(const TPoint<T>& other) const { return other.x==x && other.y==y; }
    inline bool operator!=(const TPoint<T>& other) const { return other.x!=x || other.y!=y; }

    inline float length() const { return sqrtf((float)(x*x + y*y)); }
    inline T manhattanLength() const { return std::abs(x) + std::abs(y); }

    inline float distanceFrom(const TPoint<T>& other) const {
        return TPoint<T>(x - other.x, y - other.y).getLength();
    }

    T x, y;
};

typedef TPoint<int> Point;
typedef TPoint<float> PointF;

#endif
