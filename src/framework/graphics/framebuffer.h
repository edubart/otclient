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
#include <framework/core/scheduledevent.h>
#include <framework/core/timer.h>
#include <client/const.h>

static constexpr int32_t MAX_NODES = 512;

class FrameBuffer : public stdext::shared_object
{
protected:
    FrameBuffer();

    friend class FrameBufferManager;

public:
    const static uint8_t
        MIN_TIME_UPDATE = 16,
        FORCE_UPDATE = 1,
        FLUSH_AMOUNT = 25;

    virtual ~FrameBuffer();

    void resize(const Size& size);
    void bind();
    void release();
    void draw();
    void draw(const Rect& dest);
    void draw(const Rect& dest, const Rect& src);

    void setBackuping(bool enabled) { m_backuping = enabled; }
    void setSmooth(bool enabled) { m_smooth = enabled; }
    void setDrawable(bool enabled) { m_drawable = enabled; }

    TexturePtr getTexture() { return m_texture; }
    Size getSize();
    bool isBackuping() { return m_backuping; }
    bool isSmooth() { return m_smooth; }
    bool isDrawable() { return m_drawable; }

    bool canUpdate();
    void update();
    void schedulePainting(const uint16_t time);
    void removeRenderingTime(const uint16_t time);
    void cleanTexture() { m_texture = nullptr; }

    void useSchedulePainting(const bool use) { m_schedulePaintingEnabled = use; }
    void setMinTimeUpdate(const uint16 time) { m_minTimeUpdate = time; }

private:
    void internalCreate();
    void internalBind();
    void internalRelease();

    uint16_t flushTime();

    TexturePtr m_texture;
    TexturePtr m_screenBackup;
    Size m_oldViewportSize;
    uint m_fbo;
    uint m_prevBoundFbo;
    stdext::boolean<true> m_forceUpdate;
    stdext::boolean<true> m_backuping;
    stdext::boolean<true> m_smooth;
    stdext::boolean<true> m_drawable;
    stdext::boolean<true> m_schedulePaintingEnabled;

    static uint boundFbo;

    std::unordered_map<uint16_t, std::pair<uint16_t, ScheduledEventPtr>> m_schedules;
    Timer m_lastRenderedTime;
    uint16_t m_requestAmount;
    uint16 m_minTimeUpdate;
};

#endif
