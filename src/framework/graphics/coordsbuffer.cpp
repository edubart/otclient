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

CoordsBuffer::CoordsBuffer()
{
    m_hardwareCacheMode = HardwareBuffer::DynamicDraw;
    m_hardwareVertexBuffer = nullptr;
    m_hardwareTextureVertexBuffer = nullptr;
    m_hardwareCached = false;
    m_hardwareCaching = false;
}

CoordsBuffer::~CoordsBuffer()
{
    if(m_hardwareVertexBuffer)
        delete m_hardwareVertexBuffer;
    if(m_hardwareTextureVertexBuffer)
        delete m_hardwareTextureVertexBuffer;
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
            m_vertexBuffer.addRect(partialDest);
            m_textureVertexBuffer.addRect(partialSrc);
        }
    }
    m_hardwareCached = false;
}

void CoordsBuffer::enableHardwareCaching(HardwareBuffer::UsagePattern usagePattern)
{
#ifndef OPENGL_ES2
    if(!GL_ARB_vertex_buffer_object)
        return;
#endif
    m_hardwareCacheMode = usagePattern;
    m_hardwareCaching = true;
    m_hardwareCached = false;
}

void CoordsBuffer::updateCaches()
{
    if(!m_hardwareCaching || m_hardwareCached)
        return;

    if(m_vertexBuffer.vertexCount() > 0) {
        if(!m_hardwareVertexBuffer && m_vertexBuffer.vertexCount() > 0)
            m_hardwareVertexBuffer = new HardwareBuffer(HardwareBuffer::VertexBuffer);
        m_hardwareVertexBuffer->bind();
        m_hardwareVertexBuffer->write((void*)m_vertexBuffer.vertices(), m_vertexBuffer.size() * sizeof(float), m_hardwareCacheMode);
    }

    if(m_textureVertexBuffer.vertexCount() > 0) {
        if(!m_hardwareTextureVertexBuffer && m_textureVertexBuffer.vertexCount() > 0)
            m_hardwareTextureVertexBuffer = new HardwareBuffer(HardwareBuffer::VertexBuffer);
        m_hardwareTextureVertexBuffer->bind();
        m_hardwareTextureVertexBuffer->write((void*)m_textureVertexBuffer.vertices(), m_textureVertexBuffer.size() * sizeof(float), m_hardwareCacheMode);
    }

    m_hardwareCached = true;
}
