#ifndef COLOR_H
#define COLOR_H

#include "types.h"
#include <sstream>

typedef uint32 RGBA;

class Color
{
public:
    Color() : color(0) { }
    Color(uint8 r, uint8 g, uint8 b, uint8 a = 0xFF) : color(((a & 0xff)<<24) | ((b & 0xff)<<16) | ((g & 0xff)<<8) | (r & 0xff)) { }
    Color(const Color& other) : color(other.color) { }
    Color(RGBA rgba) : color(rgba) { }

    uint8 a()   const { return (color >> 24) & 0xFF; }
    uint8 b() const { return (color >> 16) & 0xFF; }
    uint8 g()  const { return (color >> 8) & 0xFF; }
    uint8 r() const { return color & 0xFF; }
    RGBA rgba() const { return color; }
    const uint8* rgbaPtr() const { return (const uint8*)&color; }

    void setAlpha(uint8 a) { color = ((a & 0xff)<<24) | (color & 0x00ffffff); }
    void setBlue(uint8 b)   { color = ((b & 0xff)<<16) | (color & 0xff00ffff); }
    void setGreen(uint8 g) { color = ((g & 0xff)<<8) | (color & 0xffff00ff); }
    void setRed(uint8 r)  { color = (r & 0xff) | (color & 0xffffff00); }
    void setRGBA(uint8 r, uint8 g, uint8 b, uint8 a = 0xFF) { color = ((a & 0xff)<<24) | ((b & 0xff)<<16) | ((g & 0xff)<<8) | (r & 0xff); }
    void setRGBA(uint32 rgba) { color = rgba; }

    Color& operator=(const Color& other) { color = other.color;  return *this; }
    bool operator==(const Color& other) const { return other.color == color; }
    bool operator!=(const Color& other) const { return other.color != color; }

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

inline std::istream& operator>>(std::istream& in, Color& color)
{
    int r, g, b, a;
    in >> r >> g >> b >> a;
    color.setRGBA(r, g, b, a);
    return in;
}

#endif
