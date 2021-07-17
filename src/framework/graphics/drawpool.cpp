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
#include "declarations.h"
#include <framework/core/declarations.h>
#include <framework/graphics/framebuffermanager.h>
#include <framework/graphics/graphics.h>
#include "painter.h"

DrawPool g_drawPool;

void DrawPool::init()
{
    n_unknowPool = g_drawPool.createPool(PoolType::UNKNOW);
    use(n_unknowPool);
}
void DrawPool::terminate()
{
    m_currentPool = nullptr;
    for(int8 i = -1; ++i <= PoolType::UNKNOW;)
        m_pools[i] = nullptr;
}

PoolFramedPtr DrawPool::createPoolF(const PoolType type)
{
    auto pool = std::make_shared<PoolFramed>();

    pool->m_framebuffer = g_framebuffers.createFrameBuffer(true);
    pool->m_state.alphaWriting = false;

    if(type == PoolType::MAP) pool->m_framebuffer->disableBlend();
    else if(type == PoolType::LIGHT) pool->m_framebuffer->setCompositionMode(Painter::CompositionMode_Light);

    m_pools[type] = pool;

    return pool;
}

void DrawPool::addRepeated(const Painter::PainterState& state, const Pool::DrawMethod& method, const Painter::DrawMode drawMode)
{
    const auto itFind = std::find_if(m_currentPool->m_objects.begin(), m_currentPool->m_objects.end(), [state]
    (const std::shared_ptr<Pool::DrawObject>& action) { return action->state == state; });

    if(itFind != m_currentPool->m_objects.end()) {
        (*itFind)->drawMethods.push_back(method);
    } else
        m_currentPool->m_objects.push_back(std::make_shared<Pool::DrawObject>(Pool::DrawObject{ state, nullptr, drawMode, {method} }));
}

void DrawPool::add(const Painter::PainterState& state, const Pool::DrawMethod& method, const Painter::DrawMode drawMode)
{
    if(m_currentPool->isFramed())
        poolFramed()->updateHash(state.texture, method);

    auto& list = m_currentPool->m_objects;

    if(!list.empty()) {
        const auto& prevObj = list.back();

        const bool sameState = prevObj->state == state,
            hasDest = !method.dest.isNull();

        if(hasDest) {
            // Look for identical or opaque textures that are greater than or
            // equal to the size of the previous texture, if so, remove it from the list so they don't get drawn.
            for(auto itm = prevObj->drawMethods.begin(); itm != prevObj->drawMethods.end(); ++itm) {
                auto& prevMtd = *itm;
                if(prevMtd.dest == method.dest &&
                   (sameState && prevMtd.rects.second == method.rects.second || state.texture->isOpaque() && prevObj->state.texture->canSuperimposed())) {
                    prevObj->drawMethods.erase(itm);
                    break;
                }
            }
        }

        if(sameState) {
            prevObj->drawMode = Painter::DrawMode::Triangles;
            prevObj->drawMethods.push_back(method);
            return;
        }
    }

    list.push_back(std::make_shared<Pool::DrawObject>(Pool::DrawObject{ state, nullptr, drawMode, {method} }));
}

void DrawPool::draw()
{
    for(const auto& pool : m_pools) {
        if(pool->m_objects.empty()) continue;

        if(pool->isFramed()) {
            const auto pf = std::dynamic_pointer_cast<PoolFramed>(pool);
            const auto& frameBuffer = pf->m_framebuffer;
            if(frameBuffer->isDrawable()) {
                g_painter->saveAndResetState();
                if(pf->hasModification()) {
                    pf->updateStatus();
                    frameBuffer->bind();

                    for(auto& obj : pool->m_objects)
                        drawObject(*obj);

                    frameBuffer->release();
                }

                if(pf->m_beforeDraw) pf->m_beforeDraw();
                frameBuffer->draw(pf->m_dest, pf->m_src);
                if(pf->m_afterDraw) pf->m_afterDraw();

                g_painter->restoreSavedState();
            }
        } else {
            for(auto& obj : pool->m_objects)
                drawObject(*obj);
        }

        pool->m_objects.clear();
    }
}

void DrawPool::drawObject(Pool::DrawObject& obj)
{
    const auto& addCoords = [&](CoordsBuffer& coords) {
        for(const auto& method : obj.drawMethods) {
            if(method.type == Pool::DrawMethodType::DRAW_BOUNDING_RECT) {
                coords.addBoudingRect(method.rects.first, method.intValue);
            } else if(method.type == Pool::DrawMethodType::DRAW_FILLED_RECT || method.type == Pool::DrawMethodType::DRAW_REPEATED_FILLED_RECT) {
                coords.addRect(method.rects.first);
            } else if(method.type == Pool::DrawMethodType::DRAW_FILLED_TRIANGLE) {
                coords.addTriangle(std::get<0>(method.points), std::get<1>(method.points), std::get<2>(method.points));
            } else if(method.type == Pool::DrawMethodType::DRAW_TEXTURED_RECT || method.type == Pool::DrawMethodType::DRAW_REPEATED_TEXTURED_RECT) {
                if(obj.drawMode == Painter::DrawMode::Triangles)
                    coords.addRect(method.rects.first, method.rects.second);
                else
                    coords.addQuad(method.rects.first, method.rects.second);
            } else if(method.type == Pool::DrawMethodType::DRAW_UPSIDEDOWN_TEXTURED_RECT) {
                if(obj.drawMode == Painter::DrawMode::Triangles)
                    coords.addUpsideDownRect(method.rects.first, method.rects.second);
                else
                    coords.addUpsideDownQuad(method.rects.first, method.rects.second);
            }
        }

        return &coords;
    };

    if(obj.action) {
        obj.action();
        return;
    }

    if(obj.drawMethods.empty()) return;

    g_painter->executeState(obj.state);

    if(obj.state.texture) {
        obj.state.texture->create();
        g_painter->setTexture(obj.state.texture.get());
    }

    if(obj.coordsBuffer != nullptr) {
        g_painter->drawCoords(*obj.coordsBuffer, obj.drawMode);
        return;
    }

    g_painter->drawCoords(*addCoords(m_coordsbuffer), obj.drawMode);
    m_coordsbuffer.clear();
}

void DrawPool::addFillCoords(CoordsBuffer& coordsBuffer, const Color color)
{
    Pool::DrawMethod method;
    method.type = Pool::DrawMethodType::DRAW_FILL_COORDS;
    method.intValue = coordsBuffer.getVertexHash();

    auto state = generateState();
    state.color = color;

    const auto& action = std::make_shared<Pool::DrawObject>(
        Pool::DrawObject{ state, std::shared_ptr<CoordsBuffer>(&coordsBuffer, [](CoordsBuffer*) {}), Painter::DrawMode::Triangles, {method} });

    if(m_currentPool->isFramed())
        poolFramed()->updateHash(nullptr, method);

    m_currentPool->m_objects.push_back(action);
}

void DrawPool::addTextureCoords(CoordsBuffer& coordsBuffer, const TexturePtr& texture, const Color color, const Painter::DrawMode drawMode)
{
    Pool::DrawMethod method{ Pool::DrawMethodType::DRAW_TEXTURE_COORDS };
    method.intValue = coordsBuffer.getVertexHash();

    auto state = generateState();
    state.texture = texture;
    state.color = color;

    const auto& action = std::make_shared<Pool::DrawObject>(
        Pool::DrawObject{ state, std::shared_ptr<CoordsBuffer>(&coordsBuffer, [](CoordsBuffer*) {}), drawMode, {method} });

    if(m_currentPool->isFramed())
        poolFramed()->updateHash(texture, method);

    m_currentPool->m_objects.push_back(action);
}

void DrawPool::addTexturedRect(const Rect& dest, const TexturePtr& texture, const Color color)
{
    addTexturedRect(dest, texture, Rect(Point(), texture->getSize()), color);
}

void DrawPool::addTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src, const Color color, const Point& originalDest)
{
    if(dest.isEmpty() || src.isEmpty())
        return;

    Pool::DrawMethod method{ Pool::DrawMethodType::DRAW_TEXTURED_RECT };
    method.rects = std::make_pair(dest, src);
    method.dest = originalDest;

    auto state = generateState();
    state.color = color == Color::white && state.alphaWriting ? Color::alpha : color;
    state.texture = texture;

    add(state, method, Painter::DrawMode::TriangleStrip);
}

void DrawPool::addUpsideDownTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src, const Color color)
{
    if(dest.isEmpty() || src.isEmpty())
        return;

    Pool::DrawMethod method{ Pool::DrawMethodType::DRAW_UPSIDEDOWN_TEXTURED_RECT };
    method.rects = std::make_pair(dest, src);

    auto state = generateState();
    state.color = color;
    state.texture = texture;

    add(state, method, Painter::DrawMode::TriangleStrip);
}

void DrawPool::addRepeatedTexturedRect(const Rect& dest, const TexturePtr& texture, const Color color)
{
    addRepeatedTexturedRect(dest, texture, Rect(Point(), texture->getSize()), color);
}

void DrawPool::addRepeatedTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src, const Color color)
{
    if(dest.isEmpty() || src.isEmpty())
        return;

    Pool::DrawMethod method{ Pool::DrawMethodType::DRAW_REPEATED_TEXTURED_RECT };
    method.rects = std::make_pair(dest, src);

    auto state = generateState();
    state.color = color;
    state.texture = texture;

    addRepeated(state, method);
}

void DrawPool::addRepeatedFilledRect(const Rect& dest, const Color color)
{
    if(dest.isEmpty())
        return;

    Pool::DrawMethod method{ Pool::DrawMethodType::DRAW_REPEATED_FILLED_RECT };
    method.rects = std::make_pair(dest, Rect());

    auto state = generateState();
    state.color = color;

    addRepeated(state, method);
}

void DrawPool::addFilledRect(const Rect& dest, const Color color)
{
    if(dest.isEmpty())
        return;

    Pool::DrawMethod method{ Pool::DrawMethodType::DRAW_FILLED_RECT };
    method.rects = std::make_pair(dest, Rect());

    auto state = generateState();
    state.color = color;

    add(state, method);
}

void DrawPool::addFilledTriangle(const Point& a, const Point& b, const Point& c, const Color color)
{
    if(a == b || a == c || b == c)
        return;

    Pool::DrawMethod method{ Pool::DrawMethodType::DRAW_FILLED_TRIANGLE };
    method.points = std::make_tuple(a, b, c);

    auto state = generateState();
    state.color = color;

    add(state, method);
}

void DrawPool::addBoundingRect(const Rect& dest, const Color color, int innerLineWidth)
{
    if(dest.isEmpty() || innerLineWidth == 0)
        return;

    Pool::DrawMethod method{ Pool::DrawMethodType::DRAW_BOUNDING_RECT };
    method.rects = std::make_pair(dest, Rect());
    method.intValue = innerLineWidth;

    auto state = generateState();
    state.color = color;

    add(state, method);
}

void DrawPool::addAction(std::function<void()> action)
{
    m_currentPool->m_objects.push_back(std::make_shared<Pool::DrawObject>(Pool::DrawObject{ {}, nullptr, Painter::DrawMode::None, {}, action }));
}

Painter::PainterState DrawPool::generateState()
{
    Painter::PainterState state = g_painter->getCurrentState();
    state.clipRect = m_currentPool->m_state.clipRect;
    state.compositionMode = m_currentPool->m_state.compositionMode;
    state.opacity = m_currentPool->m_state.opacity;
    state.alphaWriting = m_currentPool->m_state.alphaWriting;

    return state;
}

void DrawPool::use(const PoolPtr& pool)
{
    m_currentPool = pool;
    m_currentPool->resetState();
    if(m_currentPool->isFramed()) {
        poolFramed()->resetCurrentStatus();
    }
}
