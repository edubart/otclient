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

#include <utility>

#include "declarations.h"
#include "framebuffer.h"
#include "framework/core/timer.h"
#include "texture.h"

enum class PoolType : uint8_t
{
    MAP,
    CREATURE_INFORMATION,
    LIGHT,
    TEXT,
    FOREGROUND,
    UNKNOW
};

class Pool
{
public:
    void setEnable(const bool v) { m_enabled = v; }
    bool isEnabled() const { return m_enabled; }
    PoolType getType() const { return m_type; }

protected:
    enum class DrawMethodType
    {
        RECT,
        TRIANGLE,
        REPEATED_RECT,
        BOUNDING_RECT,
        UPSIDEDOWN_RECT,
    };

    struct DrawMethod
    {
        DrawMethodType type;
        std::pair<Rect, Rect> rects;
        std::tuple<Point, Point, Point> points;
        Point dest;

        uint16_t intValue{ 0 };
    };

    struct DrawObject
    {
        ~DrawObject() { drawMethods.clear(); state.texture = nullptr; action = nullptr; }

        Painter::PainterState state;
        DrawMode drawMode;
        std::vector<DrawMethod> drawMethods;
        bool isGroupable{ false };

        std::shared_ptr<CoordsBuffer> coordsBuffer;
        std::function<void()> action{ nullptr };
    };

private:
    struct State
    {
        ~State() { shaderProgram = nullptr; action = nullptr; }

        CompositionMode compositionMode{ CompositionMode::NORMAL };
        BlendEquation blendEquation{ BlendEquation::ADD };
        Rect clipRect;
        float opacity{ 1.f };
        PainterShaderProgram* shaderProgram{ nullptr };
        std::function<void()> action{ nullptr };
    };

    float getOpacity(const int pos = -1) { return pos == -1 ? m_state.opacity : m_objects[pos - 1].state.opacity; }
    Rect getClipRect(const int pos = -1) { return pos == -1 ? m_state.clipRect : m_objects[pos - 1].state.clipRect; }

    void setCompositionMode(CompositionMode mode, int pos = -1);
    void setBlendEquation(BlendEquation equation, int pos = -1);
    void setClipRect(const Rect& clipRect, int pos = -1);
    void setOpacity(float opacity, int pos = -1);
    void setShaderProgram(const PainterShaderProgramPtr& shaderProgram, int pos = -1, const std::function<void()>& action = nullptr);

    void resetState();
    void resetOpacity() { m_state.opacity = 1.f; }
    void resetClipRect() { m_state.clipRect = {}; }
    void resetShaderProgram() { m_state.shaderProgram = nullptr; }
    void resetCompositionMode() { m_state.compositionMode = CompositionMode::NORMAL; }
    void resetBlendEquation() { m_state.blendEquation = BlendEquation::ADD; }

    void next() { m_drawingPointer.clear(); }

    virtual bool hasFrameBuffer() const { return false; };
    virtual PoolFramed* toPoolFramed() { return nullptr; }

    bool hasModification(bool autoUpdateStatus = false);
    void updateStatus() { m_status.first = m_status.second; m_refreshTime.restart(); }

    bool m_enabled{ true },
        m_forceGrouping{ false },
        m_autoUpdate{ false };

    State m_state;

    PoolType m_type{ PoolType::UNKNOW };

    Timer m_refreshTime;

    std::pair<size_t, size_t> m_status{ 0,0 };

    std::vector<DrawObject> m_objects, m_cachedObjects;

    std::unordered_map<uint16_t, size_t> m_drawingPointer;

    friend class DrawPool;
};

class PoolFramed : public Pool
{
public:
    void onBeforeDraw(std::function<void()> f) { m_beforeDraw = std::move(f); }
    void onAfterDraw(std::function<void()> f) { m_afterDraw = std::move(f); }
    void setSmooth(bool enabled) { m_framebuffer->setSmooth(enabled); }
    void resize(const Size& size) { m_framebuffer->resize(size); }
    Size getSize() { return m_framebuffer->getSize(); }

protected:
    PoolFramed(const FrameBufferPtr& fb) : m_framebuffer(fb) {};

    friend class DrawPool;

private:
    bool hasFrameBuffer() const override { return true; }
    PoolFramed* toPoolFramed() override { return this; }

    FrameBufferPtr m_framebuffer;
    Rect m_dest, m_src;

    std::function<void()> m_beforeDraw, m_afterDraw;
};

extern DrawPool g_drawPool;
