#ifndef POINT_H
#define POINT_H

#include "types.h"
#include <sstream>
#include <cmath>

template<class T>
class TSize;

template<class T>
class TPoint
{
public:
    TPoint() : x(0), y(0) {};
    TPoint(T x, T y) : x(x), y(y) { };
    TPoint(const TPoint<T>& other) : x(other.x), y(other.y) { };

    bool isNull() const { return x==0 && y==0; }
    TSize<T> toSize() const { return TSize<T>(x, y); }

    TPoint<T> operator-() const { return TPoint<T>(-x, -y); }
    TPoint<T> operator+(const TPoint<T>& other) const { return TPoint<T>(x + other.x, y + other.y);   }
    TPoint<T>& operator+=(const TPoint<T>& other) { x+=other.x; y+=other.y; return *this; }
    TPoint<T> operator-(const TPoint<T>& other) const { return TPoint<T>(x - other.x, y - other.y);   }
    TPoint<T>& operator-=(const TPoint<T>& other) { x-=other.x; y-=other.y; return *this; }
    TPoint<T> operator*(const TPoint<T>& other) const { return TPoint<T>(x * other.x, y * other.y);   }
    TPoint<T>& operator*=(const TPoint<T>& other) { x*=other.x; y*=other.y; return *this; }
    TPoint<T> operator*(const T v) const { return TPoint<T>(x * v, y * v);  }
    TPoint<T>& operator*=(const T v) { x*=v; y*=v; return *this; }
    TPoint<T> operator/(const TPoint<T>& other) const { return TPoint<T>(x/other.x, y/other.y);   }
    TPoint<T>& operator/=(const TPoint<T>& other)   { x/=other.x; y/=other.y; return *this; }
    TPoint<T> operator/(const T v) const { return TPoint<T>(x/v, y/v);  }
    TPoint<T>& operator/=(const T v) { x/=v; y/=v; return *this; }

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
        return TPoint<T>(x - other.x, y - other.y).getLength();
    }

    T x, y;
};

typedef TPoint<int> Point;
typedef TPoint<float> PointF;

template<class T>
std::ostream& operator<<(std::ostream& out, const TPoint<T>& point)
{
    out << "Point(" << point.x << ","
                    << point.y <<  ")";
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
