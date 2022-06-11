/*
 * Copyright (c) 2010-2022 OTClient <https://github.com/edubart/otclient>
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

#pragma once

#include "declarations.h"

class CachedText
{
public:
    CachedText();

    void draw(const Rect& rect, Color color);

    void wrapText(int maxWidth);
    void setFont(const BitmapFontPtr& font);
    void setText(const std::string_view text);
    void setAlign(const Fw::AlignmentFlag align);

    Size getTextSize() { return m_textSize; }
    std::string getText() const { return m_text; }
    BitmapFontPtr getFont() const { return m_font; }
    Fw::AlignmentFlag getAlign() { return m_align; }

private:
    void update();

    std::vector<std::pair<Rect, Rect>> m_TextureCoords;
    std::vector<Point> m_glyphsPositions;

    std::string m_text;
    Size m_textSize;
    Rect m_textScreenCoords;
    BitmapFontPtr m_font;
    Fw::AlignmentFlag m_align;
};
