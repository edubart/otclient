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

#include "cachedtext.h"
#include <framework/graphics/drawpool.h>
#include "bitmapfont.h"
#include "fontmanager.h"

CachedText::CachedText()
{
    m_font = g_fonts.getDefaultFont();
    m_align = Fw::AlignCenter;
}

void CachedText::draw(const Rect& rect, const Color color)
{
    if (!m_font)
        return;

    if (m_textMustRecache || m_textCachedScreenCoords != rect) {
        m_textMustRecache = false;
        m_textCachedScreenCoords = rect;

        m_textCoordsCache.clear();
        m_textCoordsCache = m_font->getDrawTextCoords(m_text, rect, m_align);
    }

    for (const auto& fontRect : m_textCoordsCache)
        g_drawPool.addTexturedRect(fontRect.first, m_font->getTexture(), fontRect.second, color);
}

void CachedText::update()
{
    if (m_font)
        m_textSize = m_font->calculateTextRectSize(m_text);

    m_textMustRecache = true;
}

void CachedText::wrapText(int maxWidth)
{
    if (m_font) {
        m_text = m_font->wrapText(m_text, maxWidth);
        update();
    }
}
