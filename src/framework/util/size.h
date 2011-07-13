#ifndef SIZE_H
#define SIZE_H

#include "point.h"

enum ESizeScaleMode {
    IGNORE_ASPECT_RATIO,
    KEEP_ASPECT_RATIO,
    KEEP_ASPECT_RATIO_BY_EXPANDING
};

template <class T>
class TSize
{
public:
    inline TSize() : wd(-1), ht(-1) {};
    inline TSize(T width, T height) : wd(width), ht(height) { };
    inline TSize(const TSize<T>& other) : wd(other.wd), ht(other.ht) { };

    inline TPoint<T> toPoint() const { return TPoint<T>(wd, ht); }

    inline bool isNull() const { return wd==0 && ht==0; }
    inline bool isEmpty() const { return wd<1 || ht<1; }
    inline bool isValid() const { return wd>=0 && ht>=0; }

    inline int width() const { return wd; }
    inline int height() const { return ht; }

    inline void setSize(T w, T h) { wd = w; ht = h; }
    inline void setWidth(T w) { wd = w; }
    inline void setHeight(T h) { ht = h; }

    inline TSize<T> operator-() const { return TSize<T>(-wd, -ht); }
    inline TSize<T> operator+(const TSize<T>& other) const { return TSize<T>(wd + other.wd, ht + other.ht);   }
    inline TSize<T>& operator+=(const TSize<T>& other) { wd+=other.wd; ht+=other.ht; return *this; }
    inline TSize<T> operator-(const TSize<T>& other) const { return TSize<T>(wd - other.wd, ht - other.ht);   }
    inline TSize<T>& operator-=(const TSize<T>& other) { wd-=other.wd; ht-=other.ht; return *this; }
    inline TSize<T> operator*(const float v) const { return TSize<T>((T)v*wd, (T)ht*v);  }
    inline TSize<T>& operator*=(const float v) { wd=(T)v*wd; ht=(T)ht*v; return *this; }
    inline TSize<T> operator/(const float v) const { return TSize<T>((T)wd/v, (T)ht/v);  }
    inline TSize<T>& operator/=(const float v) { (T)wd/=v; (T)ht/=v; return *this; }

    inline bool operator<=(const TSize<T>&other) const { return wd<=other.wd || ht<=other.ht; }
    inline bool operator>=(const TSize<T>&other) const { return wd>=other.wd || ht>=other.ht; }
    inline bool operator<(const TSize<T>&other) const { return wd<other.wd || ht<other.ht; }
    inline bool operator>(const TSize<T>&other) const { return wd>other.wd || ht>other.ht; }

    inline TSize<T>& operator=(const TSize<T>& other) { wd = other.wd; ht = other.ht; return *this; }
    inline bool operator==(const TSize<T>& other) const { return other.wd==wd && other.ht==ht; }
    inline bool operator!=(const TSize<T>& other) const { return other.wd!=wd || other.ht!=ht; }

    inline TSize<T> expandedTo(const TSize<T>& other) const { return TSize<T>(std::max(wd,other.wd), std::max(ht,other.ht)); }
    inline TSize<T> boundedTo(const TSize<T>& other) const { return TSize<T>(std::min(wd,other.wd), std::min(ht,other.ht)); }

    inline void scale(const TSize<T>& s, ESizeScaleMode mode) {
        if(mode == IGNORE_ASPECT_RATIO || wd == 0 || ht == 0) {
            wd = s.wd;
            ht = s.ht;
        } else {
            bool useHeight;
            T rw = (s.ht * wd) / ht;

            if(mode == KEEP_ASPECT_RATIO)
                useHeight = (rw <= s.wd);
            else // mode == KEEP_ASPECT_RATIO_BY_EXPANDING
                useHeight = (rw >= s.wd);

            if(useHeight) {
                wd = rw;
                ht = s.ht;
            } else {
                ht = (s.wd * ht)/wd;
                wd = s.wd;
            }
        }
    }
    inline void scale(int w, int h, ESizeScaleMode mode) { scale(TSize<T>(w, h)); }

    inline float ratio() const { return (float)wd/ht; }
    inline T area() const { return wd*ht; }

private:
    T wd, ht;
};

typedef TSize<int> Size;
typedef TSize<float> SizeF;


template <class T>
inline std::ostream& operator<<(std::ostream& out, const TSize<T>& size)
{
    out << "Size(" << size.width() << ","
                    << size.height() <<  ")";
    return out;
}

#endif
