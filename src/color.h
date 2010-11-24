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

#include "prerequisites.h"

typedef uint32 RGBA;

class Color
{
public:
    Color() : color(0) { }
    Color(uint8 r, uint8 g, uint8 b, uint8 a = 0xFF) : color(((r & 0xff)<<24) | ((g & 0xff)<<16) | ((b & 0xff)<<8) | (a & 0xff)) { }
    Color(const Color& other) : color(other.color) { }
    Color(RGBA rgba) : color(rgba) { }

    uint8 red()   const { return (color >> 24) & 0xFF; }
    uint8 green() const { return (color >> 16) & 0xFF; }
    uint8 blue()  const { return (color >> 8) & 0xFF; }
    uint8 alpha() const { return color & 0xFF; }
    RGBA rgba() const { return color; }
    const uint8* rgbaPtr() const { return (const uint8*)&color; }

    void setRed(uint8 r) { color = ((r & 0xff)<<24) | (color & 0x00ffffff); }
    void setGreen(uint8 g)   { color = ((g & 0xff)<<16) | (color & 0xff00ffff); }
    void setBlue(uint8 b) { color = ((b & 0xff)<<8) | (color & 0xffff00ff); }
    void setAlpha(uint8 a)  { color = (a & 0xff) | (color & 0xffffff00); }
    void setRGBA(uint8 r, uint8 g, uint8 b, uint8 a = 0xFF) { color = ((r & 0xff)<<24) | ((g & 0xff)<<16) | ((b & 0xff)<<8) | (a & 0xff); }
    void setRGBA(uint32 rgba) { color = rgba; }

    Color& operator=(const Color& other) { color = other.color;  return *this; }
    bool operator==(const Color& other) const { return other.color == color; }
    bool operator!=(const Color& other) const { return other.color != color; }

private:
    RGBA color;
};

#endif // COLOR_H
