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

#ifndef COORDSBUFFER_H
#define COORDSBUFFER_H

#include "vertexarray.h"
#include "hardwarebuffer.h"

class CoordsBuffer
{
public:
    CoordsBuffer();
    ~CoordsBuffer();

    void clear() {
        m_textureVertexBuffer.clear();
        m_vertexBuffer.clear();
        m_hardwareCached = false;
    }

    void addRect(const Rect& dest) {
        m_vertexBuffer.addRect(dest);
        m_hardwareCached = false;
    }
    void addRect(const Rect& dest, const Rect& src) {
        m_vertexBuffer.addRect(dest);
        m_textureVertexBuffer.addRect(src);
        m_hardwareCached = false;
    }

    void addBoudingRect(const Rect& dest, int innerLineWidth);
    void addRepeatedRects(const Rect& dest, const Rect& src);

    void enableHardwareCaching(HardwareBuffer::UsagePattern usagePattern = HardwareBuffer::DynamicDraw);
    void updateCaches();
    bool isHardwareCached() { return m_hardwareCached; }

    float *getVertexBuffer() const { return m_vertexBuffer.vertices(); }
    float *getTextureVertexBuffer() const { return m_textureVertexBuffer.vertices(); }
    int getVertexCount() const { return m_vertexBuffer.vertexCount(); }
    int getTextureVertexCount() const { return m_textureVertexBuffer.vertexCount(); }

    HardwareBuffer *getHardwareVertexBuffer() { return m_hardwareVertexBuffer; }
    HardwareBuffer *getHardwareTextureVertexBuffer() { return m_hardwareTextureVertexBuffer; }

private:
    HardwareBuffer *m_hardwareVertexBuffer;
    HardwareBuffer *m_hardwareTextureVertexBuffer;
    HardwareBuffer::UsagePattern m_hardwareCacheMode;
    VertexArray m_vertexBuffer;
    VertexArray m_textureVertexBuffer;
    bool m_hardwareCached;
    bool m_hardwareCaching;
};

#endif
