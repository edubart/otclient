/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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

#include "color.h"
#include <iomanip>

// NOTE: AABBGGRR order
const Color Color::alpha      = 0x00000000U;
const Color Color::white      = 0xffffffffU;
const Color Color::black      = 0xff000000U;
const Color Color::red        = 0xff0000ffU;
const Color Color::darkRed    = 0xff000080U;
const Color Color::green      = 0xff00ff00U;
const Color Color::darkGreen  = 0xff008000U;
const Color Color::blue       = 0xffff0000U;
const Color Color::darkBlue   = 0xff800000U;
const Color Color::pink       = 0xffff00ffU;
const Color Color::darkPink   = 0xff800080U;
const Color Color::yellow     = 0xff00ffffU;
const Color Color::darkYellow = 0xff008080U;
const Color Color::teal       = 0xffffff00U;
const Color Color::darkTeal   = 0xff808000U;
const Color Color::gray       = 0xffa0a0a0U;
const Color Color::darkGray   = 0xff808080U;
const Color Color::lightGray  = 0xffc0c0c0U;
const Color Color::orange     = 0xff008cffU;

Color::Color(const std::string& coltext)
{
    std::stringstream ss(coltext);
    ss >> *this;
}

std::ostream& operator<<(std::ostream& out, const Color& color)
{
    return out << '#'
           << std::hex << std::setfill('0')
           << std::setw(2) << (int)color.r()
           << std::setw(2) << (int)color.g()
           << std::setw(2) << (int)color.b()
           << std::setw(2) << (int)color.a()
           << std::dec << std::setfill(' ');
}

std::istream& operator>>(std::istream& in, Color& color)
{
    std::string tmp;

    if(in.peek() == '#') {
        in.ignore() >> tmp;

        if(tmp.length() == 6 || tmp.length() == 8) {
            color.setRed((uint8)stdext::hex_to_dec(tmp.substr(0, 2)));
            color.setGreen((uint8)stdext::hex_to_dec(tmp.substr(2, 2)));
            color.setBlue((uint8)stdext::hex_to_dec(tmp.substr(4, 2)));
            if(tmp.length() == 8)
                color.setAlpha((uint8)stdext::hex_to_dec(tmp.substr(6, 2)));
            else
                color.setAlpha(255);
        } else {
            in.seekg(-tmp.length()-1, std::ios_base::cur);
        }
    } else {
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
            in.seekg(-tmp.length(), std::ios_base::cur);
        }
    }
    return in;
}
