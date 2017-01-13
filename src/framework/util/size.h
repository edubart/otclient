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

#ifndef SIZE_H
#define SIZE_H

#include "point.h"
#include "../const.h"

template<class T>
class TSize
{
public:
    TSize() : wd(-1), ht(-1) {};
    TSize(T width, T height) : wd(width), ht(height) { };
    TSize(const TSize<T>& other) : wd(other.wd), ht(other.ht) { };

    TPoint<T> toPoint() const { return TPoint<T>(wd, ht); }

    bool isNull() const { return wd==0 && ht==0; }
    bool isEmpty() const { return wd<1 || ht<1; }
    bool isValid() const { return wd>=0 && ht>=0; }

    int width() const { return wd; }
    int height() const { return ht; }

    void resize(T w, T h) { wd = w; ht = h; }
    void setWidth(T w) { wd = w; }
    void setHeight(T h) { ht = h; }

    TSize<T> operator-() const { return TSize<T>(-wd, -ht); }
    TSize<T> operator+(const TSize<T>& other) const { return TSize<T>(wd + other.wd, ht + other.ht);   }
    TSize<T>& operator+=(const TSize<T>& other) { wd+=other.wd; ht+=other.ht; return *this; }
    TSize<T> operator-(const TSize<T>& other) const { return TSize<T>(wd - other.wd, ht - other.ht);   }
    TSize<T>& operator-=(const TSize<T>& other) { wd-=other.wd; ht-=other.ht; return *this; }
    TSize<T> operator*(const TSize<T>& other) const { return TSize<T>((T)other.wd*wd, (T)ht*other.ht);  }
    TSize<T>& operator*=(const TSize<T>& other) { wd=(T)other.wd*wd; ht=(T)ht*other.ht; return *this; }
    TSize<T> operator/(const TSize<T>& other) const { return TSize<T>((T)wd/other.wd, (T)ht/other.ht);  }
    TSize<T>& operator/=(const TSize<T>& other) { (T)wd/=other.wd; (T)ht/=other.ht; return *this; }
    TSize<T> operator*(const float v) const { return TSize<T>((T)wd*v, (T)ht*v);  }
    TSize<T>& operator*=(const float v) { wd=(T)wd*v; ht=(T)ht*v; return *this; }
    TSize<T> operator/(const float v) const { return TSize<T>((T)wd/v, (T)ht/v);  }
    TSize<T>& operator/=(const float v) { wd/=v; ht/=v; return *this; }

    bool operator<=(const TSize<T>&other) const { return wd<=other.wd || ht<=other.ht; }
    bool operator>=(const TSize<T>&other) const { return wd>=other.wd || ht>=other.ht; }
    bool operator<(const TSize<T>&other) const { return wd<other.wd || ht<other.ht; }
    bool operator>(const TSize<T>&other) const { return wd>other.wd || ht>other.ht; }

    TSize<T>& operator=(const TSize<T>& other) { wd = other.wd; ht = other.ht; return *this; }
    bool operator==(const TSize<T>& other) const { return other.wd==wd && other.ht==ht; }
    bool operator!=(const TSize<T>& other) const { return other.wd!=wd || other.ht!=ht; }

    TSize<T> expandedTo(const TSize<T>& other) const { return TSize<T>(std::max<T>(wd, other.wd), std::max<T>(ht, other.ht)); }
    TSize<T> boundedTo(const TSize<T>& other) const { return TSize<T>(std::min<T>(wd, other.wd), std::min<T>(ht, other.ht)); }

    void scale(const TSize<T>& s, Fw::AspectRatioMode mode) {
        if(mode == Fw::IgnoreAspectRatio || wd == 0 || ht == 0) {
            wd = s.wd;
            ht = s.ht;
        } else {
            bool useHeight;
            T rw = (s.ht * wd) / ht;

            if(mode == Fw::KeepAspectRatio)
                useHeight = (rw <= s.wd);
            else // mode == Fw::KeepAspectRatioByExpanding
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
    void scale(int w, int h, Fw::AspectRatioMode mode) { scale(TSize<T>(w, h), mode); }

    float ratio() const { return (float)wd/ht; }
    T area() const { return wd*ht; }

private:
    T wd, ht;
};

typedef TSize<int> Size;
typedef TSize<float> SizeF;

template<class T>
std::ostream& operator<<(std::ostream& out, const TSize<T>& size)
{
    out << size.width() << " " << size.height();
    return out;
}

template<class T>
std::istream& operator>>(std::istream& in, TSize<T>& size)
{
    T w, h;
    in >> w >> h;
    size.resize(w, h);
    return in;
}

#endif
