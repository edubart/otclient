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

uint FrameBuffer::boundFbo = 0;

FrameBuffer::FrameBuffer(const Size& size)
{
    m_clearColor = Fw::alpha;

    glGenFramebuffers(1, &m_fbo);
    if(!m_fbo)
        logFatal("Unable to create framebuffer object");

    resize(size);
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &m_fbo);
}

void FrameBuffer::resize(const Size& size)
{
    if(m_texture && m_texture->getSize() == size)
        return;

    internalBind();
    m_texture = TexturePtr(new Texture(size.width(), size.height(), 4));
    m_texture->setSmooth(true);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->getId(), 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE)
        logFatal("Unable to setup framebuffer object");
    internalRelease();
}

void FrameBuffer::bind(bool clear)
{
    internalBind();
    Matrix3 projectionMatrix = { 2.0f/m_texture->getWidth(),  0.0f,                         0.0f,
                                 0.0f,                        2.0f/m_texture->getHeight(),  0.0f,
                                -1.0f,                       -1.0f,                         0.0f };

    m_oldProjectionMatrix = g_painter.getProjectionMatrix();
    m_oldViewportSize = g_graphics.getViewportSize();
    g_painter.setProjectionMatrix(projectionMatrix);
    g_graphics.setViewportSize(m_texture->getSize());

    if(clear) {
        glClearColor(m_clearColor.rF(), m_clearColor.gF(), m_clearColor.bF(), m_clearColor.aF());
        glClear(GL_COLOR_BUFFER_BIT);
    }
}

void FrameBuffer::release()
{
    internalRelease();
    g_painter.setProjectionMatrix(m_oldProjectionMatrix);
    g_graphics.setViewportSize(m_oldViewportSize);
}

void FrameBuffer::draw(const Rect& dest)
{
    g_painter.drawTexturedRect(dest, m_texture);
}

void FrameBuffer::internalBind()
{
    if(boundFbo == m_fbo)
        return;
    assert(boundFbo != m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    m_prevBoundFbo = boundFbo;
    boundFbo = m_fbo;
}

void FrameBuffer::internalRelease()
{
    assert(boundFbo == m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_prevBoundFbo);
    boundFbo = m_prevBoundFbo;
}
