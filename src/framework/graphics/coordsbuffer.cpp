/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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

#include "coordsbuffer.h"

void CoordsBuffer::clear()
{
    m_destRects.reset();
    m_srcRects.reset();
    m_textureCoords.clear();
    m_vertices.clear();
    m_updateCache = true;
}

void CoordsBuffer::addRect(const Rect& dest)
{
    m_destRects << dest;
    m_updateCache = true;
}

void CoordsBuffer::addRect(const Rect& dest, const Rect& src)
{
    m_destRects << dest;
    m_srcRects << src;
    m_updateCache = true;
}

void CoordsBuffer::addBoudingRect(const Rect& dest, int innerLineWidth)
{
    int left = dest.left();
    int right = dest.right();
    int top = dest.top();
    int bottom = dest.bottom();
    int width = dest.width();
    int height = dest.height();
    int w = innerLineWidth;

    addRect(Rect(left, top, width - w, w)); // top
    addRect(Rect(right - w + 1, top, w, height - w)); // right
    addRect(Rect(left + w, bottom - w + 1, width - w, w)); // bottom
    addRect(Rect(left, top + w, w, height - w)); // left
    m_updateCache = true;
}

void CoordsBuffer::addRepeatedRects(const Rect& dest, const Rect& src)
{
    if(dest.isEmpty() || src.isEmpty())
        return;

    Rect virtualDest(0, 0, dest.size());
    for(int y = 0; y <= virtualDest.height(); y += src.height()) {
        for(int x = 0; x <= virtualDest.width(); x += src.width()) {
            Rect partialDest(x, y, src.size());
            Rect partialSrc(src);

            // partialCoords to screenCoords bottomRight
            if(partialDest.bottom() > virtualDest.bottom()) {
                partialSrc.setBottom(partialSrc.bottom() + (virtualDest.bottom() - partialDest.bottom()));
                partialDest.setBottom(virtualDest.bottom());
            }
            if(partialDest.right() > virtualDest.right()) {
                partialSrc.setRight(partialSrc.right() + (virtualDest.right() - partialDest.right()));
                partialDest.setRight(virtualDest.right());
            }

            partialDest.translate(dest.topLeft());
            m_destRects << partialDest;
            m_srcRects << partialSrc;
        }
    }
    m_updateCache = true;
}

void CoordsBuffer::cacheVertexArrays()
{
    if(!m_updateCache)
        return;

    int numDestRects = m_destRects.size();
    int numSrcRects = m_srcRects.size();
    m_vertices.clear();
    m_textureCoords.clear();

    for(register int i=0;i<numDestRects;++i) {
        m_vertices.addRect(m_destRects[i]);
        if(numSrcRects == numDestRects)
            m_textureCoords.addRect(m_srcRects[i]);
    }

    m_updateCache = false;
}
