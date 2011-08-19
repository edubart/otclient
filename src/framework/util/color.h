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

    void setRed(int r)  { color = (r & 0xff) | (color & 0xffffff00); }
    void setGreen(int g) { color = ((g & 0xff)<<8) | (color & 0xffff00ff); }
    void setBlue(int b) { color = ((b & 0xff)<<16) | (color & 0xff00ffff); }
    void setAlpha(int a) { color = ((a & 0xff)<<24) | (color & 0x00ffffff); }

    void setRed(float r) { setRed(int(r*255.0f)); }
    void setGreen(float g) { setGreen(int(g*255.0f)); }
    void setBlue(float b) { setBlue(int(b*255.0f)); }
    void setAlpha(float a) { setAlpha(int(a*255.0f)); }

    void setRGBA(uint8 r, uint8 g, uint8 b, uint8 a = 0xFF) { color = ((a & 0xff)<<24) | ((b & 0xff)<<16) | ((g & 0xff)<<8) | (r & 0xff); }
    void setABGR(uint32 abgr) { color = ((abgr>>24) & 0xff) | ((abgr>>16) & 0xff) << 8 | ((abgr>>8) & 0xff) << 16 | (abgr & 0xff) << 24; }
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
        uint32 tmp;
        in >> hex >> tmp;
        color.setABGR(tmp);
        in >> dec;
    }
    return in;
}

#endif
