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

#include "cachedtext.h"
#include "fontmanager.h"
#include <framework/graphics/drawpool.h>

CachedText::CachedText() : m_font(g_fonts.getDefaultFont()), m_align(Fw::AlignCenter) {}

void CachedText::draw(const Rect& rect, const Color color)
{
    if (!m_font)
        return;

    if (m_textScreenCoords != rect) {
        m_textScreenCoords = rect;

        m_TextureCoords.clear();
        m_TextureCoords = m_font->getDrawTextCoords(m_text, m_textSize, m_align, rect, m_glyphsPositions);
    }

    for (const auto& [screenCoords, textureCoords] : m_TextureCoords)
        g_drawPool.addTexturedRect(screenCoords, m_font->getTexture(), textureCoords, color);
}

void CachedText::update()
{
    if (m_font) {
        m_glyphsPositions = m_font->calculateGlyphsPositions(m_text, Fw::AlignTopLeft, &m_textSize);
    }

    m_textScreenCoords = {};
}

void CachedText::wrapText(int maxWidth)
{
    if (!m_font)
        return;

    m_text = m_font->wrapText(m_text, maxWidth);
    update();
}

void CachedText::setFont(const BitmapFontPtr& font)
{
    if (m_font == font)
        return;

    m_font = font;
    update();
}
void CachedText::setText(const std::string_view text)
{
    if (m_text == text)
        return;

    m_text = text;
    update();
}
void CachedText::setAlign(const Fw::AlignmentFlag align)
{
    if (m_align == align)
        return;

    m_align = align;
    update();
}
