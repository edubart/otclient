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

#ifndef COORDSBUFFER_H
#define COORDSBUFFER_H

#include "vertexarray.h"
#include "hardwarebuffer.h"

class CoordsBuffer
{
public:
    void clear()
    {
        m_textureCoordArray.clear();
        m_vertexArray.clear();
        m_hash = 0;
    }

    void addTriangle(const Point& a, const Point& b, const Point& c)
    {
        m_vertexArray.addTriangle(a, b, c);

        if(m_generateHash) {
            boost::hash_combine(m_hash, a.hash());
            boost::hash_combine(m_hash, b.hash());
            boost::hash_combine(m_hash, c.hash());
        }
    }
    void addRect(const Rect& dest)
    {
        m_vertexArray.addRect(dest);
        generateHash(dest);
    }
    void addRect(const Rect& dest, const Rect& src)
    {
        m_vertexArray.addRect(dest);
        m_textureCoordArray.addRect(src);
        generateHash(dest);
    }
    void addQuad(const Rect& dest, const Rect& src)
    {
        m_vertexArray.addQuad(dest);
        m_textureCoordArray.addQuad(src);
        generateHash(dest);
    }
    void addUpsideDownQuad(const Rect& dest, const Rect& src)
    {
        m_vertexArray.addUpsideDownQuad(dest);
        m_textureCoordArray.addQuad(src);
        generateHash(dest);
    }

    void addUpsideDownRect(const Rect& dest, const Rect& src)
    {
        m_vertexArray.addUpsideDownRect(dest);
        m_textureCoordArray.addRect(src);
        generateHash(dest);
    }

    void addBoudingRect(const Rect& dest, int innerLineWidth);
    void addRepeatedRects(const Rect& dest, const Rect& src);
    void disableHashGeneration() { m_generateHash = false; }

    float* getVertexArray() { return m_vertexArray.vertices(); }
    float* getTextureCoordArray() { return m_textureCoordArray.vertices(); }
    int getVertexCount() const { return m_vertexArray.vertexCount(); }
    int getTextureCoordCount() const { return m_textureCoordArray.vertexCount(); }

    size_t hashCode() const { return m_hash; }

private:
    void generateHash(const Rect& dest) { if(m_generateHash) boost::hash_combine(m_hash, dest.hash()); }

    VertexArray m_vertexArray;
    VertexArray m_textureCoordArray;

    size_t m_hash{ 0 };
    bool m_generateHash{ true };
};

#endif
