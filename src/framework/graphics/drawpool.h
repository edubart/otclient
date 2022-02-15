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

#include <framework/core/graphicalapplication.h>
#include <framework/graphics/declarations.h>
#include <framework/graphics/framebuffer.h>
#include <framework/graphics/graphics.h>
#include <framework/graphics/pool.h>

enum  PoolType : uint8 {
    MAP,
    CREATURE_INFORMATION,
    LIGHT,
    TEXT,
    FOREGROUND,
    UNKNOW
};

enum class PoolEventType : uint8 {
    ON_BEFORE_DRAW,
    ON_AFTER_DRAW
};

class DrawPool
{
public:
    PoolPtr createPool(const PoolType type) { return m_pools[type] = std::make_shared<Pool>(); }
    PoolFramedPtr createPoolF(PoolType type);
    PoolPtr get(const PoolType type) const { return m_pools[type]; }

    void use(const PoolPtr& pool, bool forceGrouping = false);
    void use(const PoolFramedPtr& pool, const Rect& dest, const Rect& src, const Color colorClear);

    void addTexturedRect(const Rect& dest, const TexturePtr& texture, Color color = Color::white);
    void addTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src, Color color = Color::white, const Point& originalDest = Point());
    void addUpsideDownTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src, Color color = Color::white);
    void addTexturedRepeatedRect(const Rect& dest, const TexturePtr& texture, const Rect& src, Color color = Color::white);
    void addFilledRect(const Rect& dest, Color color = Color::white);
    void addFilledTriangle(const Point& a, const Point& b, const Point& c, Color color = Color::white);
    void addBoundingRect(const Rect& dest, Color color = Color::white, int innerLineWidth = 1);
    void addAction(std::function<void()> action);

    void drawTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src);

    void setCompositionMode(const Painter::CompositionMode mode, const int pos = -1) { m_currentPool->setCompositionMode(mode, pos); }
    void setClipRect(const Rect& clipRect, const int pos = -1) { m_currentPool->setClipRect(clipRect, pos); }
    void setOpacity(const float opacity, const int pos = -1) { m_currentPool->setOpacity(opacity, pos); }
    void setShaderProgram(const PainterShaderProgramPtr& shaderProgram, const int pos = -1) { m_currentPool->setShaderProgram(shaderProgram, pos); }

    float getOpacity(const int pos = -1) { return m_currentPool->getOpacity(pos); }

    void resetClipRect() { m_currentPool->resetClipRect(); }
    void resetCompositionMode() { m_currentPool->resetCompositionMode(); }
    void resetOpacity() { m_currentPool->resetOpacity(); }
    void resetState() { m_currentPool->resetState(); }
    void resetShaderProgram() { m_currentPool->resetShaderProgram(); }

    void forceGrouping(const bool force) { m_forceGrouping = force; }
    bool isForcingGrouping() const { return m_forceGrouping; }

    void startPosition() { m_currentPool->startPosition(); }

    size_t size() { return m_currentPool->m_objects.size(); }

private:
    void draw();
    void init();
    void terminate();
    void drawObject(Pool::DrawObject& obj);
    void updateHash(const Painter::PainterState& state, const Pool::DrawMethod& method);
    void add(const Painter::PainterState& state, const Pool::DrawMethod& method, Painter::DrawMode drawMode = Painter::DrawMode::Triangles);

    PoolFramedPtr poolFramed() { return std::dynamic_pointer_cast<FramedPool>(m_currentPool); }

    Painter::PainterState generateState(const Color& color, const TexturePtr& texture = nullptr);

    CoordsBuffer m_coordsBuffer;
    std::array<PoolPtr, UNKNOW + 1> m_pools;

    PoolPtr m_currentPool, n_unknowPool;

    bool m_forceGrouping;

    friend class GraphicalApplication;
};

extern DrawPool g_drawPool;

#endif
