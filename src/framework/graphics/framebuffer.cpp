/* The MIT License
 *
 * Copyright (c) 2010 OTClient, https://github.com/edubart/otclient
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

#include <global.h>
#include <core/platform.h>
#include <graphics/framebuffer.h>
#include <graphics/graphics.h>

PFNGLGENFRAMEBUFFERSPROC         oglGenFramebuffers = 0;
PFNGLBINDFRAMEBUFFERPROC         oglBindFramebuffer = 0;
PFNGLFRAMEBUFFERTEXTURE2DPROC    oglFramebufferTexture2D = 0;
PFNGLDELETEFRAMEBUFFERSPROC      oglDeleteFramebuffers = 0;
PFNGLCHECKFRAMEBUFFERSTATUSPROC  oglCheckFramebufferStatus = 0;

FrameBuffer::FrameBuffer(int width, int height)
{
    m_fbo = 0;
    m_width = width;
    m_height = height;

    // create FBO texture
    glGenTextures(1, &m_fboTexture);
    glBindTexture(GL_TEXTURE_2D, m_fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // we want bilinear filtering (for a smooth framebuffer)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // use FBO ext only if supported
    if(g_graphics.isExtensionSupported("ARB_framebuffer_object")) {
        m_fallbackOldImp = false;
        if(!oglGenFramebuffers) {
            oglGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)Platform::getExtensionProcAddress("glGenFramebuffers");
            oglBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)Platform::getExtensionProcAddress("glBindFramebuffer");
            oglFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)Platform::getExtensionProcAddress("glFramebufferTexture2D");
            oglDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)Platform::getExtensionProcAddress("glDeleteFramebuffers");
            oglCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)Platform::getExtensionProcAddress("glCheckFramebufferStatus");
        }

        // generate FBO
        oglGenFramebuffers(1, &m_fbo);
        oglBindFramebuffer(GL_FRAMEBUFFER_EXT, m_fbo);

        // attach 2D texture to this FBO
        oglFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_fboTexture, 0);

        GLenum status = oglCheckFramebufferStatus(GL_FRAMEBUFFER_EXT);
        switch(status) {
            case GL_FRAMEBUFFER_COMPLETE_EXT:
                //ok
                break;
            default: // fallback to old implementation
                m_fallbackOldImp = true;
                break;
        }
    } else {
        // otherwise fallback to copy texture from screen implementation
        m_fallbackOldImp = true;
    }
}

FrameBuffer::~FrameBuffer()
{
    glDeleteTextures(1, &m_fboTexture);
    if(m_fbo)
        oglDeleteFramebuffers(1, &m_fbo);
}

void FrameBuffer::bind()
{
    if(!m_fallbackOldImp) {
        // bind framebuffer
        oglBindFramebuffer(GL_FRAMEBUFFER_EXT, m_fbo);
    }

    // setup framebuffer viewport
    glViewport(0, 0, m_width, m_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0f, m_width, 0, m_height);

    // back to model view
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // clear framebuffer
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void FrameBuffer::unbind()
{
    if(!m_fallbackOldImp) {
        // bind back buffer again
        oglBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
        glDrawBuffer(GL_BACK);
        glReadBuffer(GL_BACK);

        // restore graphics viewport
        g_graphics.restoreViewport();
    } else {
        // copy screen to texture
        glBindTexture(GL_TEXTURE_2D, m_fboTexture);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_width, m_height);

        // restore graphics viewport
        g_graphics.restoreViewport();

        // clear screen
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}

void FrameBuffer::draw(int x, int y, int width, int height)
{
    glBindTexture(GL_TEXTURE_2D, m_fboTexture);
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex2i(x,       y);
    glTexCoord2i(0, 1); glVertex2i(x,       y+height);
    glTexCoord2i(1, 1); glVertex2i(x+width, y+height);
    glTexCoord2i(1, 0); glVertex2i(x+width, y);
    glEnd();
}
