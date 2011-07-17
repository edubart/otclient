#ifndef RECT_H
#define RECT_H

#include "types.h"
#include <ostream>

template <class T>
class TPoint;

template <class T>
class TSize;

template <class T>
class TRect
{
public:
    inline TRect() : x1(0), y1(0), x2(-1), y2(-1) { }
    inline TRect(T x, T y, T width, T height) : x1(x), y1(y), x2(x+width-1), y2(y+height-1) { }
    inline TRect(const TPoint<T>& topLeft, const TPoint<T>& bottomRight) : x1(topLeft.x), y1(topLeft.y), x2(bottomRight.x), y2(bottomRight.y) { }
    inline TRect(const TRect<T>& other) : x1(other.x1), y1(other.y1), x2(other.x2), y2(other.y2) { }
    inline TRect(T x, T y, const TSize<T>& size) : x1(x), y1(y), x2(x+size.width()-1), y2(y+size.height()-1) { }
    inline TRect(const TPoint<T>& topLeft, const TSize<T>& size) : x1(topLeft.x), y1(topLeft.y), x2(x1+size.width()-1), y2(y1+size.height()-1) { }

    inline bool isNull() const { return x2 == x1 - 1 && y2 == y1 - 1; }
    inline bool isEmpty() const { return x1 > x2 || y1 > y2; }
    inline bool isValid() const { return x1 <= x2 && y1 <= y2; }

    inline T left() const { return x1; }
    inline T top() const { return y1; }
    inline T right() const { return x2; }
    inline T bottom() const { return y2; }
    inline T horizontalCenter() const { return x1 + (x2 - x1)/2; }
    inline T verticalCenter() const { return y1 + (y2 - y1)/2; }
    inline T x() const { return x1; }
    inline T y() const { return y1; }
    inline TPoint<T> topLeft() const { return TPoint<T>(x1, y1); }
    inline TPoint<T> bottomRight() const { return TPoint<T>(x2, y2); }
    inline TPoint<T> topRight() const { return TPoint<T>(x2, y1); }
    inline TPoint<T> bottomLeft() const { return TPoint<T>(x1, y2); }
    inline TPoint<T> center() const { return TPoint<T>((x1+x2)/2, (y1+y2)/2); }
    inline T width() const { return  x2 - x1 + 1; }
    inline T height() const { return  y2 - y1 + 1; }
    inline TSize<T> size() const { return TSize<T>(width(), height()); }
    inline void reset() { x1 = y1 = 0; x2 = y2 = -1; }
    inline void clear() { x2 = x1 - 1; y2 = y1 - 1; }

    inline void setLeft(T pos) { x1 = pos; }
    inline void setTop(T pos) { y1 = pos; }
    inline void setRight(T pos) { x2 = pos; }
    inline void setBottom(T pos) { y2 = pos; }
    inline void setX(T x) { x1 = x; }
    inline void setY(T y) { y1 = y; }
    inline void setTopLeft(const TPoint<T> &p) { x1 = p.x; y1 = p.y; }
    inline void setBottomRight(const TPoint<T> &p) { x2 = p.x; y2 = p.y; }
    inline void setTopRight(const TPoint<T> &p) { x2 = p.x; y1 = p.y; }
    inline void setBottomLeft(const TPoint<T> &p) { x1 = p.x; y2 = p.y; }
    inline void setWidth(T width) { x2 = x1 + width - 1; }
    inline void setHeight(T height) { y2 = y1 + height- 1; }
    inline void setSize(T width, T height) { x2 = x1 + width - 1; y2 = y1 + height - 1; }
    inline void setSize(const TSize<T>& size) { x2 = x1 + size.width() - 1; y2 = y1 + size.height() - 1; }
    inline void setRect(T x, T y, T width, T height) { x1 = x; y1 = y; x2 = (x + width - 1); y2 = (y + height - 1); }
    inline void setCoords(int left, int top, int right, int bottom) { x1 = left; y1 = top; x2 = right; y2 = bottom; }

    inline void addLeft(T add) { x1 -= add; }
    inline void addTop(T add) { y1 -= add; }
    inline void addRight(T add) { x2 += add; }
    inline void addBottom(T add) { y2 += add; }

    inline void translate(T x, T y) { x1 += x; y1 += y; x2 += x; y2 += y; }
    inline void translate(const TPoint<T> &p) { x1 += p.x; y1 += p.y; x2 += p.x; y2 += p.y; }
    inline void moveTo(T x, T y) { x2 += x - x1; y2 += y - y1; x1 = x; y1 = y; }
    inline void moveTo(const TPoint<T> &p) { x2 += p.x - x1; y2 += p.y - y1; x1 = p.x; y1 = p.y; }
    inline void moveLeft(T pos) { x2 += (pos - x1); x1 = pos; }
    inline void moveTop(T pos) { y2 += (pos - y1); y1 = pos; }
    inline void moveRight(T pos) { x1 += (pos - x2); x2 = pos; }
    inline void moveBottom(T pos) { y1 += (pos - y2); y2 = pos; }
    inline void moveTopLeft(const TPoint<T> &p) { moveLeft(p.x); moveTop(p.y); }
    inline void moveBottomRight(const TPoint<T> &p) { moveRight(p.x); moveBottom(p.y); }
    inline void moveTopRight(const TPoint<T> &p) { moveRight(p.x); moveTop(p.y); }
    inline void moveBottomLeft(const TPoint<T> &p) { moveLeft(p.x); moveBottom(p.y); }

    inline TRect<T> translated(int x, int y) const { return TRect<T>(TPoint<T>(x1 + x, y1 + y), TPoint<T>(x2 + x, y2 + y)); }
    inline TRect<T> translated(const TPoint<T> &p) const { return TRect<T>(TPoint<T>(x1 + p.x, y1 + p.y), TPoint<T>(x2 + p.x, y2 + p.y)); }

    inline TRect<T> expanded(T pixels) const { return TRect<T>(TPoint<T>(x1 - pixels, y1 - pixels), TPoint<T>(x2 + pixels, y2 + pixels)); }

    inline void moveCenter(const TPoint<T> &p) {
        T w = x2 - x1;
        T h = y2 - y1;
        x1 = p.x - w/2;
        y1 = p.y - h/2;
        x2 = x1 + w;
        y2 = y1 + h;
    }
    inline void moveHorizontalCenter(T x) {
        T w = x2 - x1;
        x1 = x - w/2;
        x2 = x1 + w;
    }
    inline void moveVerticalCenter(T y) {
        T h = y2 - y1;
        y1 = y - h/2;
        y2 = y1 + h;
    }

    inline bool contains(const TPoint<T> &p, bool insideOnly = false) const {
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

    inline bool intersects(const TRect<T> &r) const {
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

        if (l1 > r2 || l2 > r1)
            return false;

        int t1 = y1;
        int b1 = y1;
        if(y2 - y1 + 1 < 0)
            t1 = y2;
        else
            b1 = y2;

        int t2 = r.y1;
        int b2 = r.y1;
        if (r.y2 - r.y1 + 1 < 0)
            t2 = r.y2;
        else
            b2 = r.y2;

        if(t1 > b2 || t2 > b1)
            return false;

        return true;
    }

    inline TRect<T> united(const TRect<T> &r) const {
        TRect<T> tmp;
        tmp.x1 = std::min(x1, r.x1);
        tmp.x2 = std::max(x2, r.x2);
        tmp.y1 = std::min(y1, r.y1);
        tmp.y2 = std::max(y2, r.y2);
        return tmp;
    }

    inline TRect<T> intersection(const TRect<T> &r) const {
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
        tmp.x1 = std::min(l1, l2);
        tmp.x2 = std::max(r1, r2);
        tmp.y1 = std::min(t1, t2);
        tmp.y2 = std::max(b1, b2);
        return tmp;
    }

    inline TRect<T>& operator=(const TRect<T>& other) { x1 = other.x1; y1 = other.y1; x2 = other.x2; y2 = other.y2; return *this;  }
    inline bool operator==(const TRect<T>& other) const { return (x1 == other.x1 && y1 == other.y1 && x2 == other.x2 && y2 == other.y2); }
    inline bool operator!=(const TRect<T>& other) const { return (x1 != other.x1 || y1 != other.y1 || x2 != other.x2 || y2 != other.y2); }

    inline  TRect<T>& operator|=(const TRect<T>& other) { *this = united(other); return *this; }
    inline  TRect<T>& operator&=(const TRect<T>& other) { *this = intersection(other); return *this; }

private:
    T x1, y1, x2, y2;
};

typedef TRect<int> Rect;
typedef TRect<float> RectF;

template <class T>
inline std::ostream& operator<<(std::ostream& out, const TRect<T>& rect)
{
    out << "Rect(" << rect.left() << ","
                   << rect.top() << ","
                   << rect.width() << ","
                   << rect.height() << ")";
    return out;
}

#endif // RECT_H
