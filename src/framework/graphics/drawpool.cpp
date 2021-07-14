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

void DrawPool::init() {}
void DrawPool::terminate()
{
    m_currentFrameBuffer = nullptr;
    m_coordsCache.clear();
}

void DrawPool::setFrameBuffer(const FrameBufferPtr& frameBuffer)
{
    m_currentFrameBuffer = frameBuffer;
    frameBuffer->resetCurrentStatus();
}

void DrawPool::addRepeated(const TexturePtr& texture, const FrameBuffer::DrawMethod& method, const Painter::DrawMode drawMode)
{
    auto currentState = g_painter->getCurrentState();
    currentState.texture = texture;

    const auto itFind = std::find_if(m_repeatedActions.begin(), m_repeatedActions.end(), [currentState]
    (const std::shared_ptr<FrameBuffer::DrawObject>& action) { return action->state == currentState; });

    if(itFind != m_repeatedActions.end()) {
        (*itFind)->drawMethods.push_back(method);
    } else
        m_repeatedActions.push_back(std::make_shared<FrameBuffer::DrawObject>(FrameBuffer::DrawObject{ currentState, nullptr, drawMode, {method} }));
}

void DrawPool::add(const TexturePtr& texture, const FrameBuffer::DrawMethod& method, const Painter::DrawMode drawMode)
{
    auto currentState = g_painter->getCurrentState();
    currentState.texture = texture;

    if(!m_currentFrameBuffer) {
        if(!m_actions.empty()) {
            auto& prevDrawObject = m_actions.back();

            if(prevDrawObject->state == currentState) {
                prevDrawObject->drawMode = Painter::DrawMode::Triangles;
                prevDrawObject->drawMethods.push_back(method);

                return;
            }
        }

        m_actions.push_back(
            std::make_shared<FrameBuffer::DrawObject>(FrameBuffer::DrawObject{ currentState, nullptr, drawMode, {method} })
        );

        return;
    }

    if(!m_currentFrameBuffer->isDrawable()) return;

    m_currentFrameBuffer->updateHash(texture, method);

    if(!m_currentFrameBuffer->m_actions.empty()) {
        auto& prevDrawObject = m_currentFrameBuffer->m_actions.back();

        if(prevDrawObject->state == currentState) {
            prevDrawObject->drawMode = Painter::DrawMode::Triangles;

            // Search for identical objects in the same position
            bool push = false;
            if(!method.dest.isNull()) {
                const auto itFind = std::find_if(prevDrawObject->drawMethods.begin(), prevDrawObject->drawMethods.end(),
                                                 [&](const FrameBuffer::DrawMethod& prevMethod) { return prevMethod.dest == method.dest; });
                push = itFind != prevDrawObject->drawMethods.end();
            }

            if(!push) {
                prevDrawObject->drawMethods.push_back(method);
                auto& list = m_currentFrameBuffer->m_coordsActionObjects[method.dest.hash()];
                list.push_back(prevDrawObject);
            }

            return;
        }
    }

    const auto& actionObject = std::make_shared<FrameBuffer::DrawObject>(FrameBuffer::DrawObject{ currentState, nullptr, drawMode, {method} });

    // Look for identical or opaque textures that are greater than or equal to the size of the previous texture and remove.
    if(method.type == FrameBuffer::DrawMethodType::DRAW_TEXTURED_RECT && !method.dest.isNull() &&
       currentState.compositionMode != Painter::CompositionMode_Multiply &&
       texture && currentState.opacity >= 1.f && currentState.color.aF() >= 1.f) {
        auto& list = m_currentFrameBuffer->m_coordsActionObjects[method.dest.hash()];
        for(auto& prevAction : list) {
            //when it's the same texture, check if the source is the same, because in the case of outfit, the addons use "the same texture"
            const bool isSameTexture = prevAction->state.texture == texture;

            if(isSameTexture || texture->isOpaque() && prevAction->state.texture->canSuperimposed()) {
                for(auto itm = prevAction->drawMethods.begin(); itm != prevAction->drawMethods.end(); ++itm) {
                    auto& prevMethod = *itm;
                    if(prevMethod.dest == method.dest && (!isSameTexture || prevMethod.rects.second == method.rects.second)) {
                        prevAction->drawMethods.erase(itm);
                        break;
                    }
                }
            }
        }

        list.push_back(actionObject);
    }

    m_currentFrameBuffer->m_actions.push_back(actionObject);
}

void DrawPool::draw()
{
    for(auto& obj : m_actions)
        drawObject(*obj);
    for(auto& obj : m_repeatedActions)
        drawObject(*obj);

    m_actions.clear();
    m_repeatedActions.clear();
}

void DrawPool::draw(const FrameBufferPtr& frameBuffer, const Rect& dest, const Rect& src)
{
    if(!frameBuffer->isDrawable()) return;

    g_painter->saveAndResetState();
    if(frameBuffer->hasModification()) {
        frameBuffer->updateStatus();
        frameBuffer->bind();

        for(auto& obj : frameBuffer->m_actions)
            drawObject(*obj);

        frameBuffer->release();
    }

    frameBuffer->m_coordsActionObjects.clear();
    frameBuffer->m_actions.clear();

    frameBuffer->draw(dest, src);
    m_currentFrameBuffer = nullptr;

    g_painter->restoreSavedState();
}

void DrawPool::drawObject(const FrameBuffer::DrawObject& obj)
{
    if(obj.action) {
        obj.action();
        return;
    }

    if(obj.drawMethods.empty()) return;

    g_painter->executeState(obj.state);

    if(obj.coordsBuffer != nullptr) {
        g_painter->drawCoords(*obj.coordsBuffer, obj.drawMode);
        return;
    }

    for(const auto& method : obj.drawMethods) {
        if(method.type == FrameBuffer::DrawMethodType::DRAW_BOUNDING_RECT) {
            m_tempCoordsBuffer.addBoudingRect(method.rects.first, method.intValue);
        } else if(method.type == FrameBuffer::DrawMethodType::DRAW_FILLED_RECT || method.type == FrameBuffer::DrawMethodType::DRAW_REPEATED_FILLED_RECT) {
            m_tempCoordsBuffer.addRect(method.rects.first);
        } else if(method.type == FrameBuffer::DrawMethodType::DRAW_FILLED_TRIANGLE) {
            m_tempCoordsBuffer.addTriangle(std::get<0>(method.points), std::get<1>(method.points), std::get<2>(method.points));
        } else if(method.type == FrameBuffer::DrawMethodType::DRAW_TEXTURED_RECT || method.type == FrameBuffer::DrawMethodType::DRAW_REPEATED_TEXTURED_RECT) {
            if(obj.drawMode == Painter::DrawMode::Triangles)
                m_tempCoordsBuffer.addRect(method.rects.first, method.rects.second);
            else
                m_tempCoordsBuffer.addQuad(method.rects.first, method.rects.second);
        } else if(method.type == FrameBuffer::DrawMethodType::DRAW_UPSIDEDOWN_TEXTURED_RECT) {
            if(obj.drawMode == Painter::DrawMode::Triangles)
                m_tempCoordsBuffer.addUpsideDownRect(method.rects.first, method.rects.second);
            else
                m_tempCoordsBuffer.addUpsideDownQuad(method.rects.first, method.rects.second);
        }
    }

    g_painter->executeState(obj.state);
    g_painter->drawCoords(m_tempCoordsBuffer, obj.drawMode);
    m_tempCoordsBuffer.clear();
}

void DrawPool::addFillCoords(CoordsBuffer& coordsBuffer)
{
    FrameBuffer::DrawMethod method;
    method.type = FrameBuffer::DrawMethodType::DRAW_FILL_COORDS;
    method.intValue = coordsBuffer.getVertexHash();

    const auto& action = std::make_shared<FrameBuffer::DrawObject>(
        FrameBuffer::DrawObject{ g_painter->getCurrentState(), std::shared_ptr<CoordsBuffer>(&coordsBuffer, [](CoordsBuffer*) {}), Painter::DrawMode::Triangles, {method} });

    if(m_currentFrameBuffer) {
        m_currentFrameBuffer->updateHash(nullptr, method);
        m_currentFrameBuffer->m_actions.push_back(action);
    } else
        m_actions.push_back(action);
}

void DrawPool::addTextureCoords(CoordsBuffer& coordsBuffer, const TexturePtr& texture, const Painter::DrawMode drawMode)
{
    if(texture && texture->isEmpty())
        return;

    FrameBuffer::DrawMethod method{ FrameBuffer::DrawMethodType::DRAW_TEXTURE_COORDS };
    method.intValue = coordsBuffer.getVertexHash();

    auto currentState = g_painter->getCurrentState();
    currentState.texture = texture;

    const auto& action = std::make_shared<FrameBuffer::DrawObject>(
        FrameBuffer::DrawObject{ currentState, std::shared_ptr<CoordsBuffer>(&coordsBuffer, [](CoordsBuffer*) {}), drawMode, {method} });

    if(m_currentFrameBuffer) {
        m_currentFrameBuffer->updateHash(texture, method);
        m_currentFrameBuffer->m_actions.push_back(action);
    } else {
        m_actions.push_back(action);
    }
}

void DrawPool::addTexturedRect(const Rect& dest, const TexturePtr& texture)
{
    addTexturedRect(dest, texture, Rect(Point(), texture->getSize()));
}

void DrawPool::addTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src, const Point& originalDest)
{
    if(dest.isEmpty() || src.isEmpty() || texture->isEmpty())
        return;

    FrameBuffer::DrawMethod method{ FrameBuffer::DrawMethodType::DRAW_TEXTURED_RECT };
    method.rects = std::make_pair(dest, src);
    method.dest = originalDest;

    add(texture, method, Painter::DrawMode::TriangleStrip);
}

void DrawPool::addUpsideDownTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
    if(dest.isEmpty() || src.isEmpty() || texture->isEmpty())
        return;

    FrameBuffer::DrawMethod method{ FrameBuffer::DrawMethodType::DRAW_UPSIDEDOWN_TEXTURED_RECT };
    method.rects = std::make_pair(dest, src);

    add(texture, method, Painter::DrawMode::TriangleStrip);
}

void DrawPool::addRepeatedTexturedRect(const Rect& dest, const TexturePtr& texture)
{
    addRepeatedTexturedRect(dest, texture, Rect(Point(), texture->getSize()));
}

void DrawPool::addRepeatedTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
    if(dest.isEmpty() || src.isEmpty() || texture->isEmpty())
        return;

    const	std::hash<size_t> toHash;

    size_t hash = toHash(g_painter->getColor().rgba());
    boost::hash_combine(hash, toHash(texture->getId()));

    FrameBuffer::DrawMethod method{ FrameBuffer::DrawMethodType::DRAW_REPEATED_TEXTURED_RECT };
    method.rects = std::make_pair(dest, src);

    addRepeated(texture, method);
}

void DrawPool::addRepeatedFilledRect(const Rect& dest)
{
    if(dest.isEmpty())
        return;

    FrameBuffer::DrawMethod method{ FrameBuffer::DrawMethodType::DRAW_REPEATED_FILLED_RECT };
    method.rects = std::make_pair(dest, Rect());

    addRepeated(nullptr, method);
}

void DrawPool::addFilledRect(const Rect& dest)
{
    if(dest.isEmpty())
        return;

    FrameBuffer::DrawMethod method{ FrameBuffer::DrawMethodType::DRAW_FILLED_RECT };
    method.rects = std::make_pair(dest, Rect());

    add(nullptr, method);
}

void DrawPool::addFilledTriangle(const Point& a, const Point& b, const Point& c)
{
    if(a == b || a == c || b == c)
        return;

    FrameBuffer::DrawMethod method{ FrameBuffer::DrawMethodType::DRAW_FILLED_TRIANGLE };
    method.points = std::make_tuple(a, b, c);

    add(nullptr, method);
}

void DrawPool::addBoundingRect(const Rect& dest, int innerLineWidth)
{
    if(dest.isEmpty() || innerLineWidth == 0)
        return;

    FrameBuffer::DrawMethod method{ FrameBuffer::DrawMethodType::DRAW_BOUNDING_RECT };
    method.rects = std::make_pair(dest, Rect());
    method.intValue = innerLineWidth;

    add(nullptr, method);
}

void DrawPool::addAction(std::function<void()> action)
{
    auto& list = m_currentFrameBuffer ? m_currentFrameBuffer->m_actions : m_actions;

    list
        .push_back(std::make_shared<FrameBuffer::DrawObject>(FrameBuffer::DrawObject{ {}, nullptr, Painter::DrawMode::None, {}, action }));
}
