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

#include "graphicscontextwgl.h"

GraphicsContextWGL::GraphicsContextWGL() :
    GraphicsContext("WGL")
{
    m_deviceContext = 0;
    m_wglContext = 0;
}

void GraphicsContextWGL::create(WindowType window)
{
    m_deviceContext = GetDC(window);
    if(!m_deviceContext)
        g_logger.fatal("GetDC failed");

    uint pixelFormat;
    static PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR),
                                         1,
                                         PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
                                         PFD_TYPE_RGBA,
                                         32,                         // Select Our Color Depth
                                         8, 0, 8, 0, 8, 0,           // Color Bits Ignored
                                         8,                          // Alpha Buffer Bits
                                         0,                          // Shift Bit Ignored
                                         0,                          // No Accumulation Buffer
                                         0, 0, 0, 0,                 // Accumulation Bits Ignored
                                         0,                          // Z-Buffer (Depth Buffer)
                                         0,                          // No Stencil Buffer
                                         0,                          // No Auxiliary Buffer
                                         PFD_MAIN_PLANE,             // Main Drawing Layer
                                         0,                          // Reserved
                                         0, 0, 0 };                  // Layer Masks Ignored

    pixelFormat = ChoosePixelFormat(m_deviceContext, &pfd);
    if(!pixelFormat)
        g_logger.fatal("Could not find a suitable pixel format");

    if(!SetPixelFormat(m_deviceContext, pixelFormat, &pfd))
        g_logger.fatal("Could not set the pixel format");

    if(!(m_wglContext = wglCreateContext(m_deviceContext)))
        g_logger.fatal("Unable to create GL context");
}

void GraphicsContextWGL::destroy(WindowType window)
{
    if(m_wglContext) {
        if(!wglMakeCurrent(NULL, NULL))
            g_logger.error("Release of dc and rc failed.");
        if(!wglDeleteContext(m_wglContext))
            g_logger.error("Release rendering context failed.");
        m_wglContext = NULL;
    }

    if(m_deviceContext) {
        if(!ReleaseDC(window, m_deviceContext))
            g_logger.error("Release device context failed.");
        m_deviceContext = NULL;
    }
}

void GraphicsContextWGL::restore()
{
    if(!wglMakeCurrent(m_deviceContext, m_wglContext))
        g_logger.fatal("Unable to make current WGL context");
}

bool GraphicsContextWGL::isExtensionSupported(const char *ext)
{
    typedef const char* (WINAPI * wglGetExtensionsStringProc)();
    wglGetExtensionsStringProc wglGetExtensionsString = (wglGetExtensionsStringProc)getExtensionProcAddress("wglGetExtensionsStringEXT");
    if(!wglGetExtensionsString)
        return false;

    const char *exts = wglGetExtensionsString();
    if(exts && strstr(exts, ext))
        return true;

    return false;
}

void *GraphicsContextWGL::getExtensionProcAddress(const char *ext)
{
    return (void*)wglGetProcAddress(ext);
}

void GraphicsContextWGL::swapBuffers()
{
    SwapBuffers(m_deviceContext);
}

void GraphicsContextWGL::setVerticalSync(bool enable)
{
    if(!isExtensionSupported("WGL_EXT_swap_control"))
        return;

    typedef BOOL (WINAPI * wglSwapIntervalProc)(int);
    wglSwapIntervalProc wglSwapInterval = (wglSwapIntervalProc)getExtensionProcAddress("wglSwapIntervalEXT");
    if(!wglSwapInterval)
        return;

    wglSwapInterval(enable ? 1 : 0);
}
