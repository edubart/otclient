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

#ifndef DRAWPOOL_H
#define DRAWPOOL_H

#include <framework/graphics/declarations.h>
#include <framework/graphics/graphics.h>
#include <framework/graphics/framebuffer.h>
#include <framework/graphics/pool.h>

enum PoolType : uint8 {
    MAP,
    CREATURE_INFORMATION,
    LIGHT,
    TEXT,
    FOREGROUND,
    LAST
};

class DrawPool
{
public:
    void init();
    void terminate();

    PoolPtr createPool(const PoolType type);
    PoolFramedPtr createPoolF(const PoolType type);

    void addFillCoords(CoordsBuffer& coordsBuffer);
    void addTextureCoords(CoordsBuffer& coordsBuffer, const TexturePtr& texture, Painter::DrawMode drawMode = Painter::DrawMode::Triangles);

    void addTexturedRect(const Rect& dest, const TexturePtr& texture);
    void addTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src, const Point& originalDest = Point());
    void addUpsideDownTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src);
    void addRepeatedTexturedRect(const Rect& dest, const TexturePtr& texture);
    void addRepeatedTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src);
    void addRepeatedFilledRect(const Rect& dest);
    void addFilledRect(const Rect& dest);
    void addFilledTriangle(const Point& a, const Point& b, const Point& c);
    void addBoundingRect(const Rect& dest, int innerLineWidth = 1);
    void addAction(std::function<void()> action);

    void draw();
    void link(const PoolPtr& pool, const std::function<void()> f);

    bool isOnThread();

private:
    PoolFramedPtr poolFramed();

    void drawObject(const Pool::DrawObject& obj);
    void add(const TexturePtr& texture, const Pool::DrawMethod& method, const Painter::DrawMode drawMode = Painter::DrawMode::Triangles);
    void addRepeated(const TexturePtr& texture, const Pool::DrawMethod& method, const Painter::DrawMode drawMode = Painter::DrawMode::Triangles);;

    CoordsBuffer m_coordsbuffer;

    std::array<PoolPtr, PoolType::LAST> m_pools;
};

#endif
