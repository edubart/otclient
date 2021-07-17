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

    void addFillCoords(CoordsBuffer& coordsBuffer, const Color color = Color::white);
    void addTextureCoords(CoordsBuffer& coordsBuffer, const TexturePtr& texture, const Color color = Color::white, Painter::DrawMode drawMode = Painter::DrawMode::Triangles);

    void addTexturedRect(const Rect& dest, const TexturePtr& texture, const Color color = Color::white);
    void addTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src, const Color color = Color::white, const Point& originalDest = Point());
    void addUpsideDownTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src, const Color color = Color::white);
    void addRepeatedTexturedRect(const Rect& dest, const TexturePtr& texture, const Color color = Color::white);
    void addRepeatedTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src, const Color color = Color::white);
    void addRepeatedFilledRect(const Rect& dest, const Color color = Color::white);
    void addFilledRect(const Rect& dest, const Color color = Color::white);
    void addFilledTriangle(const Point& a, const Point& b, const Point& c, const Color color = Color::white);
    void addBoundingRect(const Rect& dest, const Color color = Color::white, int innerLineWidth = 1);
    void addAction(std::function<void()> action);

    size_t getSize();

    // -1 = LAST POSITION
    void setColor(const Color color, const int pos = -1);
    void setCompositionMode(const Painter::CompositionMode mode, const int pos = -1);
    void setClipRect(const Rect& clipRect, const int pos = -1);
    void setOpacity(const float opacity, const int pos = -1);

    void resetClipRect() { m_state.clipRect = Rect(); }
    void resetCompositionMode() { m_state.compositionMode = Painter::CompositionMode_Normal; }
    void resetOpacity() { m_state.opacity = 1.f; }
    void resetState() { resetClipRect(); resetCompositionMode(); resetOpacity(); }

    void draw();
    void registerThread(const PoolPtr& pool, const std::function<void()> f);
    bool isOnThread();
    bool multiThreadEnabled() const { return m_multiThread; }
    void setMultiThread(const bool v) { m_multiThread = v; }

private:
    struct State {
        Painter::CompositionMode compositionMode;
        Rect clipRect;
        float opacity;
    };

    PoolFramedPtr poolFramed();

    void drawObject(Pool::DrawObject& obj);
    void add(const Painter::PainterState& state, const Pool::DrawMethod& method, const Painter::DrawMode drawMode = Painter::DrawMode::Triangles);
    void addRepeated(const Painter::PainterState& state, const Pool::DrawMethod& method, const Painter::DrawMode drawMode = Painter::DrawMode::Triangles);;

    Painter::PainterState generateState();

    CoordsBuffer m_coordsbuffer;
    std::array<PoolPtr, PoolType::LAST> m_pools;

    State m_state;

    bool m_multiThread;
};

#endif
