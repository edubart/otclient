/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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

#ifndef COLOR_H
#define COLOR_H

#include "../util/types.h"
#include "../util/tools.h"
#include "../const.h"

class Color
{
public:
    Color() : m_rgba(0xFFFFFFFF) { }
    Color(uint8 r, uint8 g, uint8 b, uint8 a = 0xFF) : m_r(r), m_g(g), m_b(b), m_a(a) { }
    Color(const Color& other) : m_rgba(other.m_rgba) { }
    Color(uint32 rgba) : m_rgba(rgba) { }

    uint8 a() const { return m_a; }
    uint8 b() const { return m_b; }
    uint8 g() const { return m_g; }
    uint8 r() const { return m_r; }

    float aF() const { return m_a/255.0f; }
    float bF() const { return m_b/255.0f; }
    float gF() const { return m_g/255.0f; }
    float rF() const { return m_r/255.0f; }

    uint32 rgba() const { return m_rgba; }

    const uint8* rgbaPtr() const { return (const uint8*)&m_rgba; }

    void setRed(int r)  { m_r = r; }
    void setGreen(int g) { m_g = g; }
    void setBlue(int b) { m_b = b; }
    void setAlpha(int a) { m_a = a; }

    void setRed(float r) { setRed(int(r*255.0f)); }
    void setGreen(float g) { setGreen(int(g*255.0f)); }
    void setBlue(float b) { setBlue(int(b*255.0f)); }
    void setAlpha(float a) { setAlpha(int(a*255.0f)); }

    void setRGBA(int r, int g, int b, int a = 0xFF) { m_r = r; m_g = g; m_b = b; m_a = a; }
    void setRGBA(uint32 rgba) { rgba = rgba; }

    Color& operator=(const Color& other) { m_rgba = other.m_rgba; return *this; }
    bool operator==(const Color& other) const { return other.m_rgba == m_rgba; }
    bool operator!=(const Color& other) const { return other.m_rgba != m_rgba; }

    static Color from8bit(int color) {
        if(color >= 216 || color <= 0)
            return Color(0, 0, 0);

        int r = int(color / 36) % 6 * 51;
        int g = int(color / 6) % 6 * 51;
        int b = color % 6 * 51;
        return Color(r, g, b);
    }

private:
    union {
        uint32 m_rgba;
        struct {
            uint8 m_r;
            uint8 m_g;
            uint8 m_b;
            uint8 m_a;
        };
    };
};

inline std::ostream& operator<<(std::ostream& out, const Color& color)
{
    using namespace std;
    out << "#" << hex << setfill('0')
        << setw(2) << (int)color.r()
        << setw(2) << (int)color.g()
        << setw(2) << (int)color.b()
        << setw(2) << (int)color.a();
    out << dec << setfill(' ');
    return out;
}

inline std::istream& operator>>(std::istream& in, Color& color)
{
    using namespace std;
    std::string tmp;

    if(in.get() == '#') {
        in >> tmp;

        if(tmp.length() == 6 || tmp.length() == 8) {
            color.setRed((uint8)Fw::hex2dec(tmp.substr(0, 2)));
            color.setGreen((uint8)Fw::hex2dec(tmp.substr(2, 2)));
            color.setBlue((uint8)Fw::hex2dec(tmp.substr(4, 2)));
            if(tmp.length() == 8)
                color.setAlpha((uint8)Fw::hex2dec(tmp.substr(6, 2)));
            else
                color.setAlpha(255);
        } else
            in.seekg(-tmp.length()-1, ios_base::cur);
    } else {
        in.unget();
        in >> tmp;

        if(tmp == "alpha") {
            color = Fw::alpha;
        } else if(tmp == "black") {
            color = Fw::black;
        } else if(tmp == "white") {
            color = Fw::white;
        } else if(tmp == "red") {
            color = Fw::red;
        } else if(tmp == "darkRed") {
            color = Fw::darkRed;
        } else if(tmp == "green") {
            color = Fw::green;
        } else if(tmp == "darkGreen") {
            color = Fw::darkGreen;
        } else if(tmp == "blue") {
            color = Fw::blue;
        } else if(tmp == "darkBlue") {
            color = Fw::darkBlue;
        } else if(tmp == "pink") {
            color = Fw::pink;
        } else if(tmp == "darkPink") {
            color = Fw::darkPink;
        } else if(tmp == "yellow") {
            color = Fw::yellow;
        } else if(tmp == "darkYellow") {
            color = Fw::darkYellow;
        } else if(tmp == "teal") {
            color = Fw::teal;
        } else if(tmp == "darkTeal") {
            color = Fw::darkTeal;
        } else if(tmp == "gray") {
            color = Fw::gray;
        } else if(tmp == "darkGray") {
            color = Fw::darkGray;
        } else if(tmp == "lightGray") {
            color = Fw::lightGray;
        } else if(tmp == "orange") {
            color = Fw::orange;
        } else {
            in.seekg(-tmp.length(), ios_base::cur);
        }
    }
    return in;
}

#endif
