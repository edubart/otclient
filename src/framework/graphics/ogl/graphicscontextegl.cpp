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

#include "graphicscontextegl.h"

GraphicsContextEGL::GraphicsContextEGL() :
    GraphicsContext("EGL")
{
    m_eglConfig = 0;
    m_eglContext = 0;
    m_eglDisplay = 0;
    m_eglSurface = 0;
}

void GraphicsContextEGL::create()
{
    m_eglDisplay = eglGetDisplay(m_deviceContext);
    if(m_eglDisplay == EGL_NO_DISPLAY)
        g_logger.fatal("EGL not supported");

    if(!eglInitialize(m_eglDisplay, NULL, NULL))
        g_logger.fatal("Unable to initialize EGL");

    static int configList[] = {
#if OPENGL_ES==2
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
#else
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
#endif
        EGL_RED_SIZE, 4,
        EGL_GREEN_SIZE, 4,
        EGL_BLUE_SIZE, 4,
        EGL_ALPHA_SIZE, 4,
        EGL_NONE
    };

    EGLint numConfig;

    if(!eglGetConfigs(m_eglDisplay, NULL, 0, &numConfig))
        g_logger.fatal("No valid GL configurations");

    if(!eglChooseConfig(m_eglDisplay, configList, &m_eglConfig, 1, &numConfig))
        g_logger.fatal("Failed to choose EGL config");

    if(numConfig != 1)
        g_logger.warning("Didn't got the exact EGL config");

    EGLint contextAtrrList[] = {
#if OPENGL_ES==2
        EGL_CONTEXT_CLIENT_VERSION, 2,
#else
        EGL_CONTEXT_CLIENT_VERSION, 1,
#endif
        EGL_NONE
    };

    m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, m_window, NULL);
    if(m_eglSurface == EGL_NO_SURFACE)
        g_logger.fatal(stdext::format("Unable to create EGL surface: %s", eglGetError()));

    m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, EGL_NO_CONTEXT, contextAtrrList);
    if(m_eglContext == EGL_NO_CONTEXT )
        g_logger.fatal(stdext::format("Unable to create EGL context: %s", eglGetError()));
}

void GraphicsContextEGL::destroy()
{
    if(m_eglDisplay) {
        if(m_eglContext) {
            eglDestroyContext(m_eglDisplay, m_eglContext);
            m_eglContext = 0;
        }
        if(m_eglSurface) {
            eglDestroySurface(m_eglDisplay, m_eglSurface);
            m_eglSurface = 0;
        }
        eglTerminate(m_eglDisplay);
        m_eglDisplay = 0;
    }
}

void GraphicsContextEGL::restore()
{
    if(!eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext))
        g_logger.fatal("Unable to make current EGL context");
}

bool GraphicsContextEGL::isExtensionSupported(const char *ext)
{
    //TODO
    return false;
}

void *GraphicsContextEGL::getExtensionProcAddress(const char *ext)
{
    //TODO
    return NULL;
}

void GraphicsContextEGL::swapBuffers()
{
    eglSwapBuffers(m_eglDisplay, m_eglSurface);
}

void GraphicsContextEGL::setVerticalSync(bool enable)
{
    eglSwapInterval(m_eglDisplay, enable ? 1 : 0);
}
