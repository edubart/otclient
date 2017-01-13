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

#ifndef COORDSBUFFER_H
#define COORDSBUFFER_H

#include "vertexarray.h"
#include "hardwarebuffer.h"

class CoordsBuffer
{
    enum {
        CACHE_MIN_VERTICES_COUNT = 48
    };
public:
    CoordsBuffer();
    ~CoordsBuffer();

    void clear() {
        m_textureCoordArray.clear();
        m_vertexArray.clear();
        m_hardwareCached = false;
    }

    void addTriangle(const Point& a, const Point& b, const Point& c) {
        m_vertexArray.addTriangle(a, b, c);
        m_hardwareCached = false;
    }
    void addRect(const Rect& dest) {
        m_vertexArray.addRect(dest);
        m_hardwareCached = false;
    }
    void addRect(const Rect& dest, const Rect& src) {
        m_vertexArray.addRect(dest);
        m_textureCoordArray.addRect(src);
        m_hardwareCached = false;
    }
    void addQuad(const Rect& dest, const Rect& src) {
        m_vertexArray.addQuad(dest);
        m_textureCoordArray.addQuad(src);
        m_hardwareCached = false;
    }
    void addUpsideDownQuad(const Rect& dest, const Rect& src) {
        m_vertexArray.addUpsideDownQuad(dest);
        m_textureCoordArray.addQuad(src);
        m_hardwareCached = false;
    }

    void addBoudingRect(const Rect& dest, int innerLineWidth);
    void addRepeatedRects(const Rect& dest, const Rect& src);

    void enableHardwareCaching(HardwareBuffer::UsagePattern usagePattern = HardwareBuffer::DynamicDraw);
    void updateCaches();
    bool isHardwareCached() { return m_hardwareCached; }

    float *getVertexArray() { return m_vertexArray.vertices(); }
    float *getTextureCoordArray() { return m_textureCoordArray.vertices(); }
    int getVertexCount() { return m_vertexArray.vertexCount(); }
    int getTextureCoordCount() { return m_textureCoordArray.vertexCount(); }

    HardwareBuffer *getHardwareVertexArray() { return m_hardwareVertexArray; }
    HardwareBuffer *getHardwareTextureCoordArray() { return m_hardwareTextureCoordArray; }

private:
    HardwareBuffer *m_hardwareVertexArray;
    HardwareBuffer *m_hardwareTextureCoordArray;
    HardwareBuffer::UsagePattern m_hardwareCacheMode;
    VertexArray m_vertexArray;
    VertexArray m_textureCoordArray;
    bool m_hardwareCached;
    bool m_hardwareCaching;
};

#endif
