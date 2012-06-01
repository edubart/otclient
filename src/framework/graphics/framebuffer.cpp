/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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
#include <framework/platform/platformwindow.h>

uint FrameBuffer::boundFbo = 0;

FrameBuffer::FrameBuffer()
{
    internalCreate();
}

FrameBuffer::FrameBuffer(const Size& size)
{
    internalCreate();
    resize(size);
}

void FrameBuffer::internalCreate()
{
    m_prevBoundFbo = 0;
    m_fbo = 0;
    if(g_graphics.canUseFBO()) {
        glGenFramebuffers(1, &m_fbo);
        if(!m_fbo)
            g_logger.fatal("Unable to create framebuffer object");
    }
}

FrameBuffer::~FrameBuffer()
{
    if(m_fbo != 0)
        glDeleteFramebuffers(1, &m_fbo);
}

void FrameBuffer::resize(const Size& size)
{
    if(!size.isValid())
        return;

    if(m_texture && m_texture->getSize() == size)
        return;

    m_texture = TexturePtr(new Texture(size.width(), size.height(), 4));
    m_texture->setSmooth(true);

    if(m_fbo) {
        internalBind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->getId(), 0);

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if(status != GL_FRAMEBUFFER_COMPLETE)
            g_logger.fatal("Unable to setup framebuffer object");
        internalRelease();
    } else {
        m_screenBackup = TexturePtr(new Texture(size.width(), size.height()));
    }
}

void FrameBuffer::clear(const Color& color, const Rect& rect)
{
    bool clip = rect.isValid();
    if(clip)
        g_painter->setClipRect(Rect(0, 0, m_texture->getSize()));

    glClearColor(color.rF(), color.gF(), color.bF(), color.aF());
    glClear(GL_COLOR_BUFFER_BIT);

    if(clip)
        g_painter->resetClipRect();
}

void FrameBuffer::bind()
{
    internalBind();

    Matrix3 projectionMatrix = { 2.0f/m_texture->getWidth(),  0.0f,                         0.0f,
                                 0.0f,                        2.0f/m_texture->getHeight(),  0.0f,
                                -1.0f,                       -1.0f,                         1.0f };
    g_painter->saveAndResetState();
    g_painter->setProjectionMatrix(projectionMatrix);

    m_oldViewportSize = g_graphics.getViewportSize();
    g_graphics.setViewportSize(m_texture->getSize());
}

void FrameBuffer::release()
{
    internalRelease();
    g_painter->restoreSavedState();
    g_graphics.setViewportSize(m_oldViewportSize);
}

void FrameBuffer::draw()
{
    Rect rect(0,0, getSize());
    g_painter->drawTexturedRect(rect, m_texture, rect);
}

void FrameBuffer::draw(const Rect& dest, const Rect& src)
{
    g_painter->drawTexturedRect(dest, m_texture, src);
}

void FrameBuffer::draw(const Rect& dest)
{
    g_painter->drawTexturedRect(dest, m_texture, Rect(0,0, getSize()));
}

void FrameBuffer::internalBind()
{
    if(m_fbo) {
        assert(boundFbo != m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        m_prevBoundFbo = boundFbo;
        boundFbo = m_fbo;
    } else {
        // backup screen color buffer into a texture
        m_screenBackup->copyFromScreen(Rect(0, 0, getSize()));
    }
}

void FrameBuffer::internalRelease()
{
    if(m_fbo) {
        assert(boundFbo == m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_prevBoundFbo);
        boundFbo = m_prevBoundFbo;
    } else {
        Rect screenRect(0, 0, getSize());

        // copy the drawn color buffer into the framebuffer texture
        m_texture->copyFromScreen(screenRect);

        // restore screen original content
        g_painter->setCompositionMode(Painter::CompositionMode_Replace);
        g_painter->drawTexturedRect(screenRect, m_screenBackup, screenRect);
        g_painter->resetCompositionMode();
    }
}

Size FrameBuffer::getSize()
{
    if(m_fbo == 0) {
        // the buffer size is limited by the window size
        return Size(std::min(m_texture->getWidth(), g_window.getWidth()),
                    std::min(m_texture->getHeight(), g_window.getHeight()));
    }
    return m_texture->getSize();
}
