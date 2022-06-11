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

#include "framebuffer.h"
#include "graphics.h"
#include "texture.h"

#include <framework/core/application.h>
#include <framework/core/eventdispatcher.h>
#include <framework/graphics/drawpool.h>
#include <framework/platform/platformwindow.h>

uint FrameBuffer::boundFbo = 0;

FrameBuffer::FrameBuffer(const bool useAlphaWriting) :m_useAlphaWriting(useAlphaWriting)
{
    internalCreate();
}

void FrameBuffer::internalCreate()
{
    m_prevBoundFbo = 0;
    m_fbo = 0;
    if (g_graphics.canUseFBO()) {
        glGenFramebuffers(1, &m_fbo);
        if (!m_fbo)
            g_logger.fatal("Unable to create framebuffer object");
    }
}

FrameBuffer::~FrameBuffer()
{
#ifndef NDEBUG
    assert(!g_app.isTerminated());
#endif
    if (g_graphics.ok() && m_fbo != 0)
        glDeleteFramebuffers(1, &m_fbo);
}

void FrameBuffer::resize(const Size& size)
{
    assert(size.isValid());

    if (m_texture && m_texture->getSize() == size)
        return;

    m_texture = TexturePtr(new Texture(size));
    m_texture->setSmooth(m_smooth);
    m_texture->setUpsideDown(true);

    if (m_fbo) {
        internalBind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->getId(), 0);

        const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
            g_logger.fatal("Unable to setup framebuffer object");

        internalRelease();
    } else if (m_backuping) {
        m_screenBackup = TexturePtr(new Texture(size));
        m_screenBackup->setUpsideDown(true);
    }
}

void FrameBuffer::bind(const Rect& dest, const Rect& src)
{
    Rect _dest(0, 0, getSize()), _src = _dest;
    if (dest.isValid()) _dest = dest;
    if (src.isValid()) _src = src;

    if (_src != m_src || _dest != m_dest) {
        m_src = _src;
        m_dest = _dest;

        m_coordsBuffer.clear();
        m_coordsBuffer.addQuad(m_dest, m_src);

        m_screenCoordsBuffer.clear();
        m_screenCoordsBuffer.addRect(Rect{ 0, 0, getSize() });
    }

    m_bckResolution = g_painter->getResolution();
    internalBind();
    g_painter->setResolution(getSize());
    g_painter->setAlphaWriting(m_useAlphaWriting);

    if (m_colorClear != Color::alpha) {
        g_painter->setTexture(nullptr);
        g_painter->setColor(m_colorClear);
        g_painter->drawCoords(m_screenCoordsBuffer, Painter::DrawMode::TriangleStrip);
    }
}

void FrameBuffer::release()
{
    internalRelease();
    g_painter->setResolution(m_bckResolution);
}

void FrameBuffer::draw()
{
    if (m_disableBlend) glDisable(GL_BLEND);
    g_painter->setCompositionMode(m_compositeMode);
    g_painter->resetColor();
    g_painter->setTexture(m_texture.get());
    g_painter->drawCoords(m_coordsBuffer, Painter::DrawMode::TriangleStrip);
    g_painter->resetCompositionMode();
    if (m_disableBlend) glEnable(GL_BLEND);
}

void FrameBuffer::internalBind()
{
    if (m_fbo) {
        assert(boundFbo != m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        m_prevBoundFbo = boundFbo;
        boundFbo = m_fbo;
    } else if (m_backuping) {
        // backup screen color buffer into a texture
        m_screenBackup->copyFromScreen(Rect(0, 0, getSize()));
    }
}

void FrameBuffer::internalRelease()
{
    if (m_fbo) {
        assert(boundFbo == m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_prevBoundFbo);
        boundFbo = m_prevBoundFbo;
    } else {
        const Rect screenRect(0, 0, getSize());

        // copy the drawn color buffer into the framebuffer texture
        m_texture->copyFromScreen(screenRect);

        // restore screen original content
        if (m_backuping) {
            glDisable(GL_BLEND);
            g_painter->resetColor();
            g_painter->setTexture(m_screenBackup.get());
            g_painter->drawCoords(m_screenCoordsBuffer, Painter::DrawMode::TriangleStrip);
            glEnable(GL_BLEND);
        }
    }
}

Size FrameBuffer::getSize()
{
    if (m_fbo == 0) {
        // the buffer size is limited by the window size
        return {
            std::min<int>(m_texture->getWidth(), g_window.getWidth()),
                    std::min<int>(m_texture->getHeight(), g_window.getHeight())
        };
    }

    return m_texture->getSize();
}
