/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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

#include "types.h"
#include "tools.h"

union RGBA
{
    bool operator==(const RGBA& otherRgba) const { return rgba == otherRgba.rgba; }

    uint32 rgba;

    struct{
        uint8 r;
        uint8 g;
        uint8 b;
        uint8 a;
    };
};

class Color
{
public:
    Color() { color.rgba = 0; }
    Color(uint8 r, uint8 g, uint8 b, uint8 a = 0xFF) {
        setRGBA(r, g, b, a);
    }

    Color(const Color& other) : color(other.color) { }
    Color(RGBA rgba) : color(rgba) { }

    uint8 a() const { return color.a; }
    uint8 b() const { return color.b; }
    uint8 g() const { return color.g; }
    uint8 r() const { return color.r; }

    RGBA rgba() const { return color; }

    const uint8* rgbaPtr() const { return (const uint8*)&color; }

    void setRed(int r)  { color.r = r; }
    void setGreen(int g) { color.g = g; }
    void setBlue(int b) { color.b = b; }
    void setAlpha(int a) { color.a = a; }

    void setRed(float r) { setRed(int(r*255.0f)); }
    void setGreen(float g) { setGreen(int(g*255.0f)); }
    void setBlue(float b) { setBlue(int(b*255.0f)); }
    void setAlpha(float a) { setAlpha(int(a*255.0f)); }

    void setRGBA(uint8 r, uint8 g, uint8 b, uint8 a = 0xFF) { color.r = r; color.g = g; color.b = b; color.a = a; }
    void setRGBA(uint32 rgba) { color.rgba = rgba; }

    Color& operator=(const Color& other) { color = other.color;  return *this; }
    bool operator==(const Color& other) const { return other.color.rgba == color.rgba; }
    bool operator!=(const Color& other) const { return other.color.rgba != color.rgba; }

    static Color white;
    static Color black;
    static Color alpha;
    static Color red;
    static Color green;
    static Color blue;
    static Color pink;
    static Color yellow;

private:
    RGBA color;
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

    if(in.get() == '#') {
        std::string tmp;
        in >> tmp;

        if(tmp.length() == 6 || tmp.length() == 8) {
            color.setRed((uint8)fw::hex2dec(tmp.substr(0, 2)));
            color.setGreen((uint8)fw::hex2dec(tmp.substr(2, 2)));
            color.setBlue((uint8)fw::hex2dec(tmp.substr(4, 2)));
            if(tmp.length() == 8)
                color.setAlpha((uint8)fw::hex2dec(tmp.substr(6, 2)));
            else
                color.setAlpha(255);
        } else
            in.seekg(-tmp.length()-1, ios_base::cur);
    }
    return in;
}

#endif
