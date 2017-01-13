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

#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include "bitmapfont.h"

//@bindsingleton g_fonts
class FontManager
{
public:
    FontManager();

    void terminate();
    void clearFonts();

    bool importFont(std::string file);

    bool fontExists(const std::string& fontName);
    BitmapFontPtr getFont(const std::string& fontName);
    BitmapFontPtr getDefaultFont() { return m_defaultFont; }

    void setDefaultFont(const std::string& fontName) { m_defaultFont = getFont(fontName); }

private:
    std::vector<BitmapFontPtr> m_fonts;
    BitmapFontPtr m_defaultFont;
};

extern FontManager g_fonts;

#endif
