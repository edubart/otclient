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

#include "color.h"

// NOTE: AABBGGRR order
const Color Color::alpha      = 0x00000000U;
const Color Color::white      = 0xffffffff;
const Color Color::black      = 0xff000000;
const Color Color::red        = 0xff0000ff;
const Color Color::darkRed    = 0xff000080;
const Color Color::green      = 0xff00ff00;
const Color Color::darkGreen  = 0xff008000;
const Color Color::blue       = 0xffff0000;
const Color Color::darkBlue   = 0xff800000;
const Color Color::pink       = 0xffff00ff;
const Color Color::darkPink   = 0xff800080;
const Color Color::yellow     = 0xff00ffff;
const Color Color::darkYellow = 0xff008080;
const Color Color::teal       = 0xffffff00;
const Color Color::darkTeal   = 0xff808000;
const Color Color::gray       = 0xffa0a0a0;
const Color Color::darkGray   = 0xff808080;
const Color Color::lightGray  = 0xffc0c0c0;
const Color Color::orange     = 0xff008cff;

Color::Color(const std::string& coltext)
{
    std::stringstream ss(coltext);
    ss >> *this;
}
