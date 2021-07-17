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

#ifndef POOL_H
#define POOL_H

#include "declarations.h"
#include "framebuffer.h"
#include "texture.h"
#include "framebuffermanager.h"

class Pool
{
protected:
    enum class DrawMethodType {
        DRAW_FILL_COORDS,
        DRAW_TEXTURE_COORDS,
        DRAW_TEXTURED_RECT,
        DRAW_UPSIDEDOWN_TEXTURED_RECT,
        DRAW_REPEATED_TEXTURED_RECT,
        DRAW_REPEATED_FILLED_RECT,
        DRAW_FILLED_RECT,
        DRAW_FILLED_TRIANGLE,
        DRAW_BOUNDING_RECT
    };

    struct DrawMethod {
        DrawMethodType type;
        std::pair<Rect, Rect> rects;
        std::tuple<Point, Point, Point> points;
        Point dest;
        uint64 intValue{ 0 };
        float floatValue{ .0f };
    };

    struct DrawObject {
        ~DrawObject() { drawMethods.clear(); coordsBuffer = nullptr; state.texture = nullptr; }

        Painter::PainterState state;
        std::shared_ptr<CoordsBuffer> coordsBuffer;
        Painter::DrawMode drawMode{ Painter::DrawMode::Triangles };
        std::vector<DrawMethod> drawMethods;

        std::function<void()> action;
    };

private:
    struct State {
        Painter::CompositionMode compositionMode;
        Rect clipRect;
        float opacity;
    };

    void setCompositionMode(const Painter::CompositionMode mode, const int pos = -1);
    void setClipRect(const Rect& clipRect, const int pos = -1);
    void setOpacity(const float opacity, const int pos = -1);

    void resetClipRect() { m_state.clipRect = Rect(); }
    void resetCompositionMode() { m_state.compositionMode = Painter::CompositionMode_Normal; }
    void resetOpacity() { m_state.opacity = 1.f; }
    void resetState() { resetClipRect(); resetCompositionMode(); resetOpacity(); }

    virtual bool isFramed() const { return false; };

    std::vector<std::shared_ptr<DrawObject>> m_objects;

    State m_state;

    friend class DrawPool;
};

class PoolFramed : public Pool {
public:
    void onBeforeDraw(std::function<void()> f) { m_beforeDraw = f; }
    void onAfterDraw(std::function<void()> f) { m_afterDraw = f; }
    void setCoords(const Rect& dest, const Rect& src) { m_dest = dest; m_src = src; m_framebuffer->m_useAlphaWriting = false; }
    void resize(const Size& size) { m_framebuffer->resize(size); }
    void setSmooth(bool enabled) { m_framebuffer->setSmooth(enabled); }
    void setColorClear(const Color color) { m_framebuffer->setColorClear(color); }

protected:
    friend class Pool;
    friend class DrawPool;
    friend std::function<void()> f();

private:
    size_t updateHash(const TexturePtr& texture, const DrawMethod& method);
    void updateStatus() { m_status.first = m_status.second; }
    void resetCurrentStatus() { m_status.second = 0; }
    bool hasModification() const { return m_status.first != m_status.second; }
    bool isFramed() const override { return m_framebuffer != nullptr; }

    FrameBufferPtr m_framebuffer;
    Rect m_dest, m_src;

    std::function<void()> m_beforeDraw, m_afterDraw;
    std::pair<size_t, size_t> m_status{ 0,0 };
    std::hash<size_t> HASH_INT;
    std::hash<float> HASH_FLOAT;
};

extern DrawPool g_drawPool;

#endif
