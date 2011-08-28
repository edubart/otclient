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

#include <framework/platform/platform.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

PFNGLGENFRAMEBUFFERSPROC         oglGenFramebuffers = 0;
PFNGLBINDFRAMEBUFFERPROC         oglBindFramebuffer = 0;
PFNGLFRAMEBUFFERTEXTURE2DPROC    oglFramebufferTexture2D = 0;
PFNGLDELETEFRAMEBUFFERSPROC      oglDeleteFramebuffers = 0;
PFNGLCHECKFRAMEBUFFERSTATUSPROC  oglCheckFramebufferStatus = 0;

FrameBuffer::FrameBuffer(int width, int height)
{
    m_fbo = 0;

    // create FBO texture
    m_texture = TexturePtr(new Texture(width, height, 4));
    m_texture->enableBilinearFilter();

    // use FBO ext only if supported
    if(false && g_graphics.isExtensionSupported("GL_ARB_framebuffer_object")) {
        m_fallbackOldImp = false;
        if(!oglGenFramebuffers) {
            oglGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)g_platform.getExtensionProcAddress("glGenFramebuffers");
            oglBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)g_platform.getExtensionProcAddress("glBindFramebuffer");
            oglFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)g_platform.getExtensionProcAddress("glFramebufferTexture2D");
            oglDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)g_platform.getExtensionProcAddress("glDeleteFramebuffers");
            oglCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)g_platform.getExtensionProcAddress("glCheckFramebufferStatus");
        }

        // generate FBO
        oglGenFramebuffers(1, &m_fbo);
        oglBindFramebuffer(GL_FRAMEBUFFER_EXT, m_fbo);

        // attach 2D texture to this FBO
        oglFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_texture->getId(), 0);

        GLenum status = oglCheckFramebufferStatus(GL_FRAMEBUFFER_EXT);
        switch(status) {
            case GL_FRAMEBUFFER_COMPLETE_EXT:
                //ok
                break;
            default: // fallback to old implementation
                m_fallbackOldImp = true;
                break;
        }

        // restore back buffer
        oglBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
        glDrawBuffer(GL_BACK);
        glReadBuffer(GL_BACK);
    } else {
        // otherwise fallback to copy texture from screen implementation
        m_fallbackOldImp = true;
    }
}

FrameBuffer::~FrameBuffer()
{
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
    glViewport(0, 0, m_texture->getWidth(), m_texture->getHeight());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0f, m_texture->getWidth(), 0, m_texture->getHeight());

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
        glBindTexture(GL_TEXTURE_2D, m_texture->getId());
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_texture->getWidth(), m_texture->getHeight());

        // restore graphics viewport
        g_graphics.restoreViewport();

        // clear screen
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}

void FrameBuffer::draw(const Rect& screenCoords, const Rect& framebufferCoords)
{
    g_graphics.drawTexturedRect(screenCoords, m_texture ,framebufferCoords);
}
