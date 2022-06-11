/*
 * Copyright (c) 2010-2022 OTClient <https://github.com/edubart/otclient>
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

#pragma once

#include <framework/core/graphicalapplication.h>
#include <framework/graphics/declarations.h>
#include <framework/graphics/framebuffer.h>
#include <framework/graphics/graphics.h>
#include <framework/graphics/pool.h>

class DrawPool
{
public:
    template <class T>
    T* get(const PoolType type) { return static_cast<T*>(m_pools[static_cast<uint8>(type)]); }

    void use(PoolType type);
    void use(PoolType type, const Rect& dest, const Rect& src, Color colorClear = Color::alpha);

    void addTexturedRect(const Rect& dest, const TexturePtr& texture, Color color = Color::white);
    void addTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src, Color color = Color::white, const Point& originalDest = {});
    void addUpsideDownTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src, Color color = Color::white);
    void addTexturedRepeatedRect(const Rect& dest, const TexturePtr& texture, const Rect& src, Color color = Color::white);
    void addFilledRect(const Rect& dest, Color color = Color::white);
    void addFilledTriangle(const Point& a, const Point& b, const Point& c, Color color = Color::white);
    void addBoundingRect(const Rect& dest, Color color = Color::white, int innerLineWidth = 1);
    void addAction(std::function<void()> action);

    void setOpacity(const float opacity, const int pos = -1) { m_currentPool->setOpacity(opacity, pos); }
    void setClipRect(const Rect& clipRect, const int pos = -1) { m_currentPool->setClipRect(clipRect, pos); }
    void setBlendEquation(Painter::BlendEquation equation, const int pos = -1) { m_currentPool->setBlendEquation(equation, pos); }
    void setCompositionMode(const Painter::CompositionMode mode, const int pos = -1) { m_currentPool->setCompositionMode(mode, pos); }
    void setShaderProgram(const PainterShaderProgramPtr& shaderProgram, const int pos = -1, const std::function<void()>& action = nullptr) { m_currentPool->setShaderProgram(shaderProgram, pos, action); }

    float getOpacity(const int pos = -1) { return m_currentPool->getOpacity(pos); }

    void resetState() { m_currentPool->resetState(); }
    void resetOpacity() { m_currentPool->resetOpacity(); }
    void resetClipRect() { m_currentPool->resetClipRect(); }
    void resetShaderProgram() { m_currentPool->resetShaderProgram(); }
    void resetCompositionMode() { m_currentPool->resetCompositionMode(); }

    void forceGrouping(const bool force) { m_currentPool->m_forceGrouping = force; }
    bool isForcingGrouping() const { return m_currentPool->m_forceGrouping; }

    void next() { m_currentPool->next(); }

    size_t size() { return m_currentPool->m_objects.size(); }

private:
    void draw();
    void init();
    void terminate();
    void createPools();
    void drawObject(Pool::DrawObject& obj);
    void updateHash(const Painter::PainterState& state, const Pool::DrawMethod& method, size_t& stateHash);
    void add(const Color& color, const TexturePtr& texture, const Pool::DrawMethod& method, Painter::DrawMode drawMode = Painter::DrawMode::Triangles);

    PoolFramed* poolFramed() { return m_currentPool->toPoolFramed(); }

    CoordsBuffer m_coordsBuffer;
    std::array<Pool*, static_cast<uint8>(PoolType::UNKNOW) + 1> m_pools{};

    Pool* m_currentPool{ nullptr };

    Painter::PainterState NULL_STATE;

    friend class GraphicalApplication;
};

extern DrawPool g_drawPool;
