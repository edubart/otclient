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


#ifndef COLOR_H
#define COLOR_H

#include <prerequisites.h>

typedef uint32 RGBA;

class Color
{
public:
    inline Color() : color(0) { }
    inline Color(uint8 r, uint8 g, uint8 b, uint8 a = 0xFF) : color(((a & 0xff)<<24) | ((b & 0xff)<<16) | ((g & 0xff)<<8) | (r & 0xff)) { }
    inline Color(const Color& other) : color(other.color) { }
    inline Color(RGBA rgba) : color(rgba) { }

    inline uint8 a()   const { return (color >> 24) & 0xFF; }
    inline uint8 b() const { return (color >> 16) & 0xFF; }
    inline uint8 g()  const { return (color >> 8) & 0xFF; }
    inline uint8 r() const { return color & 0xFF; }
    inline RGBA rgba() const { return color; }
    inline const uint8* rgbaPtr() const { return (const uint8*)&color; }

    inline void setAlpha(uint8 a) { color = ((a & 0xff)<<24) | (color & 0x00ffffff); }
    inline void setBlue(uint8 b)   { color = ((b & 0xff)<<16) | (color & 0xff00ffff); }
    inline void setGreen(uint8 g) { color = ((g & 0xff)<<8) | (color & 0xffff00ff); }
    inline void setRed(uint8 r)  { color = (r & 0xff) | (color & 0xffffff00); }
    inline void setRGBA(uint8 r, uint8 g, uint8 b, uint8 a = 0xFF) { color = ((a & 0xff)<<24) | ((b & 0xff)<<16) | ((g & 0xff)<<8) | (r & 0xff); }
    inline void setRGBA(uint32 rgba) { color = rgba; }

    inline Color& operator=(const Color& other) { color = other.color;  return *this; }
    inline bool operator==(const Color& other) const { return other.color == color; }
    inline bool operator!=(const Color& other) const { return other.color != color; }

    static Color white;
    static Color black;
    static Color alpha;
    static Color red;
    static Color green;
    static Color blue;
    static Color pink;

private:
    RGBA color;
};

inline std::ostream& operator<<(std::ostream& out, const Color& color)
{
    out << "Color(" << (int)color.r() << ","
                    << (int)color.g() << ","
                    << (int)color.b() << ","
                    << (int)color.a() << ")";
    return out;
}

inline bool operator>>(const FML::Node& node, Color& color)
{
    int r, g, b, a;
    if(node.readAt(0, &r) &&
       node.readAt(1, &g) &&
       node.readAt(2, &b)) {
        a = 255;
        node.readAt(3, &a);
        color.setRGBA(r,g,b,a);
        return true;
    }
    return false;
}

#endif // COLOR_H
