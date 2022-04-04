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

#include "drawpool.h"
#include <framework/graphics/framebuffermanager.h>
#include "declarations.h"

#include <utility>
#include "painter.h"

DrawPool g_drawPool;

void DrawPool::init()
{
    n_unknowPool = g_drawPool.createPool(UNKNOW);
    use(n_unknowPool);
}

void DrawPool::terminate()
{
    m_currentPool = nullptr;
    for (int8 i = -1; ++i <= UNKNOW;)
        m_pools[i] = nullptr;
}

PoolFramedPtr DrawPool::createPoolF(const PoolType type)
{
    auto pool = std::make_shared<FramedPool>();

    pool->m_framebuffer = g_framebuffers.createFrameBuffer(true);

    if (type == MAP) pool->m_framebuffer->disableBlend();
    else if (type == LIGHT) pool->m_framebuffer->setCompositionMode(Painter::CompositionMode_Light);

    m_pools[type] = pool;

    return pool;
}

void DrawPool::add(const Painter::PainterState& state, const Pool::DrawMethod& method, const Painter::DrawMode drawMode)
{
    updateHash(state, method);

    auto& list = m_currentPool->m_objects;

    if (m_forceGrouping) {
        const auto itFind = std::find_if(list.begin() + m_currentPool->m_indexToStartSearching, list.end(), [state]
        (const Pool::DrawObject& action) { return action.state == state; });

        if (itFind != list.end()) {
            (*itFind).drawMethods.push_back(method);
        } else {
            list.push_back(Pool::DrawObject{ state, Painter::DrawMode::Triangles, {method} });
        }

        return;
    }

    if (!list.empty()) {
        auto& prevObj = list.back();

        const bool sameState = prevObj.state == state;
        if (!method.dest.isNull()) {
            // Look for identical or opaque textures that are greater than or
            // equal to the size of the previous texture, if so, remove it from the list so they don't get drawn.
            for (auto itm = prevObj.drawMethods.begin(); itm != prevObj.drawMethods.end(); ++itm) {
                auto& prevMtd = *itm;
                if (prevMtd.dest == method.dest &&
                   ((sameState && prevMtd.rects.second == method.rects.second) || (state.texture->isOpaque() && prevObj.state.texture->canSuperimposed()))) {
                    prevObj.drawMethods.erase(itm);
                    break;
                }
            }
        }

        if (sameState) {
            prevObj.drawMode = Painter::DrawMode::Triangles;
            prevObj.drawMethods.push_back(method);
            return;
        }
    }

    list.push_back(Pool::DrawObject{ state, drawMode, {method} });
}

void DrawPool::draw()
{
    // Pre Draw
    for (const auto& pool : m_pools) {
        if (!pool->isEnabled() || !pool->hasFrameBuffer()) continue;

        const auto& pf = pool->toFramedPool();
        if (pf->hasModification(true) && !pool->m_objects.empty()) {
            pf->m_framebuffer->bind();
            for (auto& obj : pool->m_objects)
                drawObject(obj);
            pf->m_framebuffer->release();
        }
    }

    // Draw
    for (const auto& pool : m_pools) {
        if (!pool->isEnabled()) continue;
        if (pool->hasFrameBuffer()) {
            const auto pf = pool->toFramedPool();

            g_painter->saveAndResetState();
            if (pf->m_beforeDraw) pf->m_beforeDraw();
            pf->m_framebuffer->draw(pf->m_dest, pf->m_src);
            if (pf->m_afterDraw) pf->m_afterDraw();
            g_painter->restoreSavedState();
        } else for (auto& obj : pool->m_objects)
            drawObject(obj);

        pool->m_objects.clear();
    }
}

void DrawPool::drawObject(Pool::DrawObject& obj)
{
    if (obj.action) {
        obj.action();
        return;
    }

    if (obj.drawMethods.empty()) return;

    g_painter->executeState(obj.state);

    if (obj.state.texture) {
        obj.state.texture->create();
        g_painter->setTexture(obj.state.texture.get());
    }

    for (const auto& method : obj.drawMethods) {
        if (method.type == Pool::DrawMethodType::BOUNDING_RECT) {
            m_coordsBuffer.addBoudingRect(method.rects.first, method.intValue);
        } else if (method.type == Pool::DrawMethodType::RECT) {
            if (obj.drawMode == Painter::DrawMode::Triangles)
                m_coordsBuffer.addRect(method.rects.first, method.rects.second);
            else
                m_coordsBuffer.addQuad(method.rects.first, method.rects.second);
        } else if (method.type == Pool::DrawMethodType::TRIANGLE) {
            m_coordsBuffer.addTriangle(std::get<0>(method.points), std::get<1>(method.points), std::get<2>(method.points));
        } else if (method.type == Pool::DrawMethodType::UPSIDEDOWN_RECT) {
            if (obj.drawMode == Painter::DrawMode::Triangles)
                m_coordsBuffer.addUpsideDownRect(method.rects.first, method.rects.second);
            else
                m_coordsBuffer.addUpsideDownQuad(method.rects.first, method.rects.second);
        } else if (method.type == Pool::DrawMethodType::REPEATED_RECT) {
            m_coordsBuffer.addRepeatedRects(method.rects.first, method.rects.second);
        }
    }

    g_painter->drawCoords(m_coordsBuffer, obj.drawMode);
    m_coordsBuffer.clear();
}

void DrawPool::addTexturedRect(const Rect& dest, const TexturePtr& texture, const Color color)
{
    addTexturedRect(dest, texture, Rect(Point(), texture->getSize()), color);
}

void DrawPool::addTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src, const Color color, const Point& originalDest)
{
    if (dest.isEmpty() || src.isEmpty())
        return;

    const Pool::DrawMethod method{
        Pool::DrawMethodType::RECT,
        std::make_pair(dest, src),
        {},
        originalDest
    };

    add(generateState(color, texture), method, Painter::DrawMode::TriangleStrip);
}

void DrawPool::addUpsideDownTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src, const Color color)
{
    if (dest.isEmpty() || src.isEmpty())
        return;

    const Pool::DrawMethod method{ Pool::DrawMethodType::UPSIDEDOWN_RECT, std::make_pair(dest, src) };

    add(generateState(color, texture), method, Painter::DrawMode::TriangleStrip);
}

void DrawPool::addTexturedRepeatedRect(const Rect& dest, const TexturePtr& texture, const Rect& src, const Color color)
{
    if (dest.isEmpty() || src.isEmpty())
        return;

    const Pool::DrawMethod method{ Pool::DrawMethodType::REPEATED_RECT,std::make_pair(dest, src) };

    add(generateState(color, texture), method);
}

void DrawPool::addFilledRect(const Rect& dest, const Color color)
{
    if (dest.isEmpty())
        return;

    const Pool::DrawMethod method{ Pool::DrawMethodType::RECT,std::make_pair(dest, Rect()) };

    add(generateState(color), method);
}

void DrawPool::addFilledTriangle(const Point& a, const Point& b, const Point& c, const Color color)
{
    if (a == b || a == c || b == c)
        return;

    const Pool::DrawMethod method{ Pool::DrawMethodType::TRIANGLE, {}, std::make_tuple(a, b, c) };

    add(generateState(color), method);
}

void DrawPool::addBoundingRect(const Rect& dest, const Color color, int innerLineWidth)
{
    if (dest.isEmpty() || innerLineWidth == 0)
        return;

    const Pool::DrawMethod method{
        Pool::DrawMethodType::BOUNDING_RECT,
        std::make_pair(dest, Rect()),
        {},{},
        static_cast<uint16>(innerLineWidth)
    };

    add(generateState(color), method);
}

void DrawPool::addAction(std::function<void()> action)
{
    m_currentPool->m_objects.push_back(Pool::DrawObject{ {}, Painter::DrawMode::None, {}, std::move(action) });
}

Painter::PainterState DrawPool::generateState(const Color& color, const TexturePtr& texture)
{
    Painter::PainterState state = g_painter->getCurrentState();
    state.clipRect = m_currentPool->m_state.clipRect;
    state.compositionMode = m_currentPool->m_state.compositionMode;
    state.opacity = m_currentPool->m_state.opacity;
    state.alphaWriting = m_currentPool->m_state.alphaWriting;
    state.shaderProgram = m_currentPool->m_state.shaderProgram;
    state.color = color;
    state.texture = texture;

    return state;
}

void DrawPool::use(const PoolPtr& pool, bool forceGrouping)
{
    m_forceGrouping = forceGrouping;

    m_currentPool = pool ? pool : n_unknowPool;
    m_currentPool->resetState();
    if (m_currentPool->hasFrameBuffer()) {
        poolFramed()->resetCurrentStatus();
    }
}

void DrawPool::use(const PoolFramedPtr& pool, const Rect& dest, const Rect& src, const Color colorClear)
{
    use(pool);
    pool->m_dest = dest;
    pool->m_src = src;
    pool->m_state.alphaWriting = false;

    g_drawPool.addFilledRect(Rect(0, 0, pool->getSize()), colorClear);
}

void DrawPool::drawTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
    if (dest.isEmpty() || src.isEmpty() || texture->isEmpty())
        return;

    g_painter->setTexture(texture.get());

    m_coordsBuffer.addQuad(dest, src);
    g_painter->drawCoords(m_coordsBuffer, Painter::DrawMode::TriangleStrip);
    m_coordsBuffer.clear();
}

void DrawPool::updateHash(const Painter::PainterState& state, const Pool::DrawMethod& method)
{
    if (!m_currentPool->hasFrameBuffer()) return;

    size_t hash = 0;

    if (state.texture) {
        // TODO: use uniqueID id when applying multithreading, not forgetting that in the APNG texture, the id changes every frame.
        stdext::hash_combine(hash, state.texture->getId());
    }

    if (state.opacity < 1.f)
        stdext::hash_combine(hash, state.opacity);

    if (state.color != Color::white)
        stdext::hash_combine(hash, state.color.rgba());

    if (state.compositionMode != Painter::CompositionMode_Normal)
        stdext::hash_combine(hash, state.compositionMode);

    if (state.shaderProgram)
        poolFramed()->m_autoUpdate = true;

    if (state.clipRect.isValid()) stdext::hash_combine(hash, state.clipRect.hash());
    if (method.rects.first.isValid()) stdext::hash_combine(hash, method.rects.first.hash());
    if (method.rects.second.isValid()) stdext::hash_combine(hash, method.rects.second.hash());

    const auto& a = std::get<0>(method.points),
        b = std::get<1>(method.points),
        c = std::get<2>(method.points);

    if (!a.isNull()) stdext::hash_combine(hash, a.hash());
    if (!b.isNull()) stdext::hash_combine(hash, b.hash());
    if (!c.isNull()) stdext::hash_combine(hash, c.hash());

    if (method.intValue) stdext::hash_combine(hash, method.intValue);
    if (method.hash) stdext::hash_combine(hash, method.hash);

    stdext::hash_combine(poolFramed()->m_status.second, hash);
}
