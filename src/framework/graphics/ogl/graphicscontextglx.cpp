/*
 * Copyright (c) 2010-2013 OTClient <https://github.com/edubart/otclient>
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

#include "graphicscontextglx.h"

GraphicsContextGLX::GraphicsContextGLX() :
    GraphicsContext("GLX")
{
    m_fbConfig = 0;
    m_glxContext = 0;
}

void GraphicsContextGLX::create()
{

}

void GraphicsContextGLX::destroy()
{
    if(m_glxContext) {
        glXMakeCurrent(m_display, None, NULL);
        glXDestroyContext(m_display, m_glxContext);
        m_glxContext = 0;
    }
}

void GraphicsContextGLX::restore()
{

}

bool GraphicsContextGLX::isExtensionSupported(const char *ext)
{
    const char *exts = glXQueryExtensionsString(m_display, m_screen);
    if(strstr(exts, ext))
        return true;
}

void *GraphicsContextGLX::getExtensionProcAddress(const char *ext)
{
    return (void *)glXGetProcAddressARB((const GLubyte*)ext);
}

void GraphicsContextGLX::swapBuffers()
{
    glXSwapBuffers(m_display, m_window);
}

void GraphicsContextGLX::setVerticalSync(bool enable)
{
    typedef GLint (*glSwapIntervalProc)(GLint);
    glSwapIntervalProc glSwapInterval = NULL;

    if(isExtensionSupported("GLX_MESA_swap_control"))
        glSwapInterval = (glSwapIntervalProc)getExtensionProcAddress("glXSwapIntervalMESA");
    else if(isExtensionSupported("GLX_SGI_swap_control"))
        glSwapInterval = (glSwapIntervalProc)getExtensionProcAddress("glXSwapIntervalSGI");

    if(glSwapInterval)
        glSwapInterval(enable ? 1 : 0);
}
