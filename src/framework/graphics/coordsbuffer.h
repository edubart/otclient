/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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

class CoordsBuffer
{
public:
    void clear();

    // no texture
    void addRect(const Rect& dest);
    void addBoudingRect(const Rect& dest, int innerLineWidth);

    // textured
    void addRect(const Rect& dest, const Rect& src);
    void addRepeatedRects(const Rect& dest, const Rect& src);

    void cacheVertexArrays();

    float *getVertices() const { return m_vertices.vertices(); }
    float *getTextureCoords() const { return m_textureCoords.vertices(); }
    int getVertexCount() const { return m_vertices.vertexCount(); }
    int getTextureCoordsCount() const { return m_textureCoords.vertexCount(); }

private:
    DataBuffer<Rect> m_destRects;
    DataBuffer<Rect> m_srcRects;
    VertexArray m_vertices;
    VertexArray m_textureCoords;
    Boolean<true> m_updateCache;
};

#endif
