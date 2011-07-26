#ifndef POINT_H
#define POINT_H

#include "types.h"
#include <ostream>
#include <cmath>

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

    inline float length() const { return sqrt((float)(x*x + y*y)); }
    inline T manhattanLength() const { return std::abs(x) + std::abs(y); }

    inline float distanceFrom(const TPoint<T>& other) const {
        return TPoint<T>(x - other.x, y - other.y).getLength();
    }

    T x, y;
};

typedef TPoint<int> Point;
typedef TPoint<float> PointF;

template <class T>
inline std::ostream& operator<<(std::ostream& out, const TPoint<T>& point)
{
    out << "Point(" << point.x << ","
                    << point.y <<  ")";
    return out;
}

#endif
