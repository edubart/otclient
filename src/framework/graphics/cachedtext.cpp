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
#include "painter.h"
#include "fontmanager.h"
#include "bitmapfont.h"
#include <framework/graphics/drawpool.h>

CachedText::CachedText()
{
	m_font = g_fonts.getDefaultFont();
	m_align = Fw::AlignCenter;
}

void CachedText::draw(const Rect& rect)
{
	if(!m_font)
		return;

	for(const auto& fontRect : m_font->getDrawTextCoords(m_text, rect, Fw::AlignCenter))
		g_drawPool.addRepeatedTexturedRect(fontRect.first, m_font->getTexture(), fontRect.second);
}

void CachedText::update()
{
	if(m_font)
		m_textSize = m_font->calculateTextRectSize(m_text);
}

void CachedText::wrapText(int maxWidth)
{
	if(m_font) {
		m_text = m_font->wrapText(m_text, maxWidth);
		update();
	}
}
