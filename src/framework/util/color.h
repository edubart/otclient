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

#ifndef COLOR_H
#define COLOR_H

#include "../stdext/types.h"
#include "../stdext/cast.h"
#include "../stdext/string.h"
#include "../const.h"
#include <iomanip>

class Color
{
public:
    Color() : m_r(1.0f), m_g(1.0f), m_b(1.0f), m_a(1.0f) { }
    Color(uint32 rgba) { setRGBA(rgba); }
    Color(uint8 r, uint8 g, uint8 b, uint8 a = 0xFF) : m_r(r/255.0f), m_g(g/255.0f), m_b(b/255.0f), m_a(a/255.0f) { }
    Color(int r, int g, int b, int a = 0xFF) : m_r(r/255.0f), m_g(g/255.0f), m_b(b/255.0f), m_a(a/255.0f) { }
    Color(float r, float g, float b, float a = 1.0f) : m_r(r), m_g(g), m_b(b), m_a(a) { }
    Color(const std::string& coltext);

    uint8 a() const { return m_a*255.0f; }
    uint8 b() const { return m_b*255.0f; }
    uint8 g() const { return m_g*255.0f; }
    uint8 r() const { return m_r*255.0f; }

    float aF() const { return m_a; }
    float bF() const { return m_b; }
    float gF() const { return m_g; }
    float rF() const { return m_r; }

    uint32 rgba() const { return uint32(a() | b() << 8 | g() << 16 | r() << 24); }

    void setRed(int r) { m_r = uint8(r)/255.0f; }
    void setGreen(int g) { m_g = uint8(g)/255.0f; }
    void setBlue(int b) { m_b = uint8(b)/255.0f; }
    void setAlpha(int a) { m_a = uint8(a)/255.0f; }

    void setRed(float r) { m_r = r; }
    void setGreen(float g) { m_g = g; }
    void setBlue(float b) { m_b = b; }
    void setAlpha(float a) { m_a = a; }

    void setRGBA(uint8 r, uint8 g, uint8 b, uint8 a = 0xFF) { m_r = r/255.0f; m_g = g/255.0f; m_b = b/255.0f; m_a = a/255.0f; }
    void setRGBA(uint32 rgba) { setRGBA((rgba >> 0) & 0xff, (rgba >> 8) & 0xff, (rgba >> 16) & 0xff, (rgba >> 24) & 0xff); }

    Color operator+(const Color& other) const { return Color(m_r + other.m_r, m_g + other.m_g, m_b + other.m_b, m_a + other.m_a); }
    Color operator-(const Color& other) const { return Color(m_r - other.m_r, m_g - other.m_g, m_b - other.m_b, m_a - other.m_a); }

    Color operator*(float v) const { return Color(m_r*v, m_g*v, m_b*v, m_a*v); }
    Color operator/(float v) const { return Color(m_r/v, m_g/v, m_b/v, m_a/v); }

    Color& operator=(uint32_t rgba) { setRGBA(rgba); return *this; }
    bool operator==(uint32_t rgba) const { return this->rgba() == rgba; }

    Color& operator=(const Color& other) { m_r = other.m_r; m_g = other.m_g; m_b = other.m_b; m_a = other.m_a; return *this; }
    bool operator==(const Color& other) const { return other.rgba() == rgba(); }
    bool operator!=(const Color& other) const { return other.rgba() != rgba(); }

    static uint8 to8bit(const Color& color) {
        uint8 c = 0;
        c += (color.r() / 51) * 36;
        c += (color.g() / 51) * 6;
        c += (color.b() / 51);
        return c;
    }

    static Color from8bit(int color) {
        if(color >= 216 || color <= 0)
            return Color(0, 0, 0);

        int r = int(color / 36) % 6 * 51;
        int g = int(color / 6) % 6 * 51;
        int b = color % 6 * 51;
        return Color(r, g, b);
    }

    static const Color alpha;
    static const Color white;
    static const Color black;
    static const Color red;
    static const Color darkRed;
    static const Color green;
    static const Color darkGreen;
    static const Color blue;
    static const Color darkBlue;
    static const Color pink;
    static const Color darkPink;
    static const Color yellow;
    static const Color darkYellow;
    static const Color teal;
    static const Color darkTeal;
    static const Color gray;
    static const Color darkGray;
    static const Color lightGray;
    static const Color orange;

private:
    float m_r;
    float m_g;
    float m_b;
    float m_a;
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
            color.setRed((uint8)stdext::hex_to_dec(tmp.substr(0, 2)));
            color.setGreen((uint8)stdext::hex_to_dec(tmp.substr(2, 2)));
            color.setBlue((uint8)stdext::hex_to_dec(tmp.substr(4, 2)));
            if(tmp.length() == 8)
                color.setAlpha((uint8)stdext::hex_to_dec(tmp.substr(6, 2)));
            else
                color.setAlpha(255);
        } else
            in.seekg(-(std::istream::streampos)tmp.length()-1, ios_base::cur);
    } else {
        in.unget();
        in >> tmp;

        if(tmp == "alpha") {
            color = Color::alpha;
        } else if(tmp == "black") {
            color = Color::black;
        } else if(tmp == "white") {
            color = Color::white;
        } else if(tmp == "red") {
            color = Color::red;
        } else if(tmp == "darkRed") {
            color = Color::darkRed;
        } else if(tmp == "green") {
            color = Color::green;
        } else if(tmp == "darkGreen") {
            color = Color::darkGreen;
        } else if(tmp == "blue") {
            color = Color::blue;
        } else if(tmp == "darkBlue") {
            color = Color::darkBlue;
        } else if(tmp == "pink") {
            color = Color::pink;
        } else if(tmp == "darkPink") {
            color = Color::darkPink;
        } else if(tmp == "yellow") {
            color = Color::yellow;
        } else if(tmp == "darkYellow") {
            color = Color::darkYellow;
        } else if(tmp == "teal") {
            color = Color::teal;
        } else if(tmp == "darkTeal") {
            color = Color::darkTeal;
        } else if(tmp == "gray") {
            color = Color::gray;
        } else if(tmp == "darkGray") {
            color = Color::darkGray;
        } else if(tmp == "lightGray") {
            color = Color::lightGray;
        } else if(tmp == "orange") {
            color = Color::orange;
        } else {
            in.seekg(-tmp.length(), ios_base::cur);
        }
    }
    return in;
}

#endif
