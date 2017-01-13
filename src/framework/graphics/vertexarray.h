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

#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include "declarations.h"
#include <framework/util/databuffer.h>

class VertexArray
{
public:
    inline void addVertex(float x, float y) { m_buffer << x << y; }
    inline void addTriangle(const Point& a, const Point& b, const Point& c) {
        addVertex(a.x, a.y);
        addVertex(b.x, b.y);
        addVertex(c.x, c.y);
    }
    inline void addRect(const Rect& rect) {
        float top = rect.top();
        float right = rect.right()+1;
        float bottom = rect.bottom()+1;
        float left = rect.left();

        addVertex(left, top);
        addVertex(right, top);
        addVertex(left, bottom);
        addVertex(left, bottom);
        addVertex(right, top);
        addVertex(right, bottom);
    }

    inline void addQuad(const Rect& rect) {
        float top = rect.top();
        float right = rect.right()+1;
        float bottom = rect.bottom()+1;
        float left = rect.left();

        addVertex(left, top);
        addVertex(right, top);
        addVertex(left, bottom);
        addVertex(right, bottom);
    }

    inline void addUpsideDownQuad(const Rect& rect) {
        float top = rect.top();
        float right = rect.right()+1;
        float bottom = rect.bottom()+1;
        float left = rect.left();

        addVertex(left, bottom);
        addVertex(right, bottom);
        addVertex(left, top);
        addVertex(right, top);
    }

    void clear() { m_buffer.reset(); }
    float *vertices() const { return m_buffer.data(); }
    int vertexCount() const { return m_buffer.size() / 2; }
    int size() const { return m_buffer.size(); }

private:
    DataBuffer<float> m_buffer;
};

#endif
