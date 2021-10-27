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

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "declarations.h"
#include "texture.h"
#include "painter.h"
#include <framework/core/scheduledevent.h>
#include <framework/core/timer.h>
#include <client/const.h>

enum class DrawMethodType {
    DRAW_FILL_COORDS,
    DRAW_TEXTURE_COORDS,
    DRAW_TEXTURED_RECT,
    DRAW_UPSIDEDOWN_TEXTURED_RECT,
    DRAW_REPEATED_TEXTURED_RECT,
    DRAW_FILLED_RECT,
    DRAW_FILLED_TRIANGLE,
    DRAW_BOUNDING_RECT
};

class FrameBuffer : public stdext::shared_object
{
public:
    ~FrameBuffer() override;

    void release();
    void resize(const Size& size);
    void bind();
    void draw(const Rect& dest, const Rect& src);

    void setSmooth(bool enabled) { m_smooth = enabled; m_texture = nullptr; }
    void setBackuping(bool enabled) { m_backuping = enabled; }

    TexturePtr getTexture() { return m_texture; }
    Size getSize();

    bool isBackuping() { return m_backuping; }
    bool isSmooth() { return m_smooth; }

    void setCompositionMode(const Painter::CompositionMode mode) { m_compositeMode = mode; }
    void disableBlend() { m_disableBlend = true; }

protected:
    FrameBuffer(bool useAlphaWriting);

    friend class FrameBufferManager;
    friend class Pool;
    friend class FramedPool;

private:
    void internalCreate();
    void internalBind();
    void internalRelease();

    static uint boundFbo;

    TexturePtr m_texture, m_screenBackup;

    Size m_oldViewportSize;

    uint32 m_fbo, m_prevBoundFbo;

    Painter::CompositionMode m_compositeMode{ Painter::CompositionMode_Normal };

    bool m_backuping{ true },
        m_smooth{ true },
        m_useAlphaWriting{ false },
        m_disableBlend{ false };
};

#endif
