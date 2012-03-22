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
std::vector<bool> auxBuffers;

FrameBuffer::FrameBuffer()
{
    m_clearColor = Color::alpha;
    internalCreate();
}

FrameBuffer::FrameBuffer(const Size& size)
{
    m_clearColor = Color::alpha;
    internalCreate();
    resize(size);
}

void FrameBuffer::internalCreate()
{
    if(g_graphics.canUseFBO()) {
        glGenFramebuffers(1, &m_fbo);
        if(!m_fbo)
            logFatal("Unable to create framebuffer object");
    }
#ifndef OPENGL_ES2
    else { // use auxiliar buffers when FBOs are not supported
        m_fbo = 0;
        if(auxBuffers.size() == 0) {
            int maxAuxs = 0;
            glGetIntegerv(GL_AUX_BUFFERS, &maxAuxs);
            auxBuffers.resize(maxAuxs+1, false);
        }
        for(uint i=1;i<auxBuffers.size();++i) {
            if(auxBuffers[i] == false) {
                m_fbo = i;
                auxBuffers[i] = true;
                break;
            }
        }
        if(!m_fbo)
            logFatal("There is no available auxiliar buffer for a new framebuffer, total AUXs: ", auxBuffers.size()-1);
    }
#endif
}

FrameBuffer::~FrameBuffer()
{
    if(g_graphics.canUseFBO()) {
        glDeleteFramebuffers(1, &m_fbo);
    }
#ifndef OPENGL_ES2
    else {
        auxBuffers[m_fbo] = false;
    }
#endif
}

void FrameBuffer::resize(const Size& size)
{
    if(!size.isValid())
        return;

    if(m_texture && m_texture->getSize() == size)
        return;

    m_texture = TexturePtr(new Texture(size.width(), size.height(), 4));
    m_texture->setSmooth(true);

    if(g_graphics.canUseFBO()) {
        internalBind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->getId(), 0);

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if(status != GL_FRAMEBUFFER_COMPLETE)
            logFatal("Unable to setup framebuffer object");
        internalRelease();
    }

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

void FrameBuffer::draw(const Rect& dest, const Rect& src)
{
    g_painter.drawTexturedRect(dest, m_texture, src);
}

void FrameBuffer::draw(const Rect& dest)
{
    g_painter.drawTexturedRect(dest, m_texture, Rect(0,0, getSize()));
}

void FrameBuffer::internalBind()
{
    if(boundFbo == m_fbo)
        return;
    assert(boundFbo != m_fbo);

    if(g_graphics.canUseFBO()) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    }
#ifndef OPENGL_ES2
    else {
        int buffer = GL_AUX0 + m_fbo - 1;
        glDrawBuffer(buffer);
        glReadBuffer(buffer);
    }
#endif

    m_prevBoundFbo = boundFbo;
    boundFbo = m_fbo;
}

void FrameBuffer::internalRelease()
{
    assert(boundFbo == m_fbo);
    if(g_graphics.canUseFBO()) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_prevBoundFbo);
    }
#ifndef OPENGL_ES2
    else {
        m_texture->bind();

        Size size = getSize();
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, size.width(), size.height());

        int buffer = GL_BACK;
        if(m_prevBoundFbo != 0)
            buffer = GL_AUX0 + m_fbo - 1;

        glDrawBuffer(buffer);
        glReadBuffer(buffer);
    }
#endif

    boundFbo = m_prevBoundFbo;
}

Size FrameBuffer::getSize()
{
#ifndef OPENGL_ES2
    if(!g_graphics.canUseFBO()) {
        // the buffer size is limited by the window size
        return Size(std::min(m_texture->getWidth(), g_window.getWidth()),
                    std::min(m_texture->getHeight(), g_window.getHeight()));
    }
#endif
    return m_texture->getSize();
}
