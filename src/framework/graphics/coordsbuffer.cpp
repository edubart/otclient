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

#include "coordsbuffer.h"
#include "graphics.h"

CoordsBuffer::CoordsBuffer()
{
    m_hardwareCacheMode = HardwareBuffer::DynamicDraw;
    m_hardwareVertexArray = nullptr;
    m_hardwareTextureCoordArray = nullptr;
    m_hardwareCached = false;
    m_hardwareCaching = false;
}

CoordsBuffer::~CoordsBuffer()
{
    if(m_hardwareVertexArray)
        delete m_hardwareVertexArray;
    if(m_hardwareTextureCoordArray)
        delete m_hardwareTextureCoordArray;
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
            m_vertexArray.addRect(partialDest);
            m_textureCoordArray.addRect(partialSrc);
        }
    }
    m_hardwareCached = false;
}

void CoordsBuffer::enableHardwareCaching(HardwareBuffer::UsagePattern usagePattern)
{
    if(!g_graphics.canUseHardwareBuffers())
        return;

    m_hardwareCacheMode = usagePattern;
    m_hardwareCaching = true;
    m_hardwareCached = false;
}

void CoordsBuffer::updateCaches()
{
    if(!m_hardwareCaching || m_hardwareCached)
        return;

    int vertexCount = m_vertexArray.vertexCount();

    // there is only performance improvement when caching a lot of vertices
    if(vertexCount < CACHE_MIN_VERTICES_COUNT)
        return;

    if(vertexCount > 0) {
        if(!m_hardwareVertexArray && m_vertexArray.vertexCount() > 0)
            m_hardwareVertexArray = new HardwareBuffer(HardwareBuffer::VertexBuffer);
        m_hardwareVertexArray->bind();
        m_hardwareVertexArray->write((void*)m_vertexArray.vertices(), m_vertexArray.size() * sizeof(float), m_hardwareCacheMode);
    }

    if(m_textureCoordArray.vertexCount() > 0) {
        if(!m_hardwareTextureCoordArray && m_textureCoordArray.vertexCount() > 0)
            m_hardwareTextureCoordArray = new HardwareBuffer(HardwareBuffer::VertexBuffer);
        m_hardwareTextureCoordArray->bind();
        m_hardwareTextureCoordArray->write((void*)m_textureCoordArray.vertices(), m_textureCoordArray.size() * sizeof(float), m_hardwareCacheMode);
    }

    m_hardwareCached = true;
}
