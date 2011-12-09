/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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

FrameBuffer::FrameBuffer(int width, int height)
{
    // create FBO texture
    m_texture = TexturePtr(new Texture(width, height, 4));
    m_texture->enableBilinearFilter();

    // generate FBO
    glGenFramebuffers(1, &m_fbo);
    if(!m_fbo)
        logFatal("Unable to create framebuffer object");

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // attach 2D texture to this FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->getId(), 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE)
        logFatal("Unable to create framebuffer object");

    // restore back buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &m_fbo);
}

void FrameBuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, m_texture->getWidth(), m_texture->getHeight());
    g_painter.updateProjectionMatrix(m_texture->getSize(), true);
}

void FrameBuffer::release()
{
    // bind back buffer again
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // restore graphics viewport
    glViewport(0, 0, g_graphics.getViewportSize().width(), g_graphics.getViewportSize().height());
    g_painter.updateProjectionMatrix(g_graphics.getViewportSize());
}

void FrameBuffer::draw(const Rect& dest)
{
    g_painter.drawTexturedRect(dest, m_texture);
}
