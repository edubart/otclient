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

#define GL_GLEXT_PROTOTYPES

#include "framebuffer.h"

#include <GL/glext.h>
#include <GL/glu.h>
#include "platform.h"
#include "graphics.h"

FrameBuffer::FrameBuffer(int width, int height) :
    m_fbo(0)
{
    m_width = width;
    m_height = height;

    // texture where the framebuffer will be store
    m_frameTexture = TexturePtr(new Texture(width, height, 4));

    // we want a smooth framebuffer
    m_frameTexture->enableBilinearFilter();

    // use FBO only if supported
    if(Platform::isExtensionSupported("EXT_framebuffer_object")) {
        m_fallbackOldImp = false;

        // generate FBO
        glGenFramebuffersEXT(1, &m_fbo);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);

        // attach 2D texture to this FBO
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_frameTexture->getTextureId(), 0);

        // must be called before checking
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
        switch(status) {
            case GL_FRAMEBUFFER_COMPLETE_EXT:
                //ok
                break;
            default:
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
    if(m_fbo)
        glDeleteFramebuffersEXT(1, &m_fbo);
}

void FrameBuffer::bind()
{
    if(!m_fallbackOldImp) {
        // bind framebuffer
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);

        // must be called before rendering to framebuffer
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    // clear framebuffer
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // setup framebuffer viewport
    g_graphics.setViewport(m_width, m_height);
}

void FrameBuffer::unbind()
{
    if(!m_fallbackOldImp) {
        // bind back buffer again
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

        // must be called to render to back buffer again
        glDrawBuffer(GL_BACK);
        glReadBuffer(GL_BACK);
    } else {
        // copy screen to texture
        m_frameTexture->copyFromScreen(0, 0, 0, 0, m_width, m_height);

        // clear screen
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    // restore graphics viewport
    g_graphics.restoreViewport();
}
