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

#include "fontmanager.h"

#include <framework/graphics/graphics.h>
#include <framework/graphics/texture.h>
#include <framework/platform/platformwindow.h>

Graphics g_graphics;

void Graphics::init()
{
    logInfo("GPU ", glGetString(GL_RENDERER));
    logInfo("OpenGL ", glGetString(GL_VERSION));

#ifndef OPENGL_ES2
    // init GL extensions
    GLenum err = glewInit();
    if(err != GLEW_OK)
        logFatal("Unable to init GLEW: ", glewGetErrorString(err));

    const char *requiredExtensions[] = {
        "GL_ARB_vertex_program",
        "GL_ARB_vertex_shader",
        "GL_ARB_fragment_shader",
        "GL_ARB_texture_non_power_of_two",
        "GL_ARB_multitexture"
    };

    std::stringstream ss;
    bool unsupported = false;
    for(auto ext : requiredExtensions) {
        if(!glewIsSupported(ext)) {
            ss << ext << std::endl;
            unsupported = true;
        }
    }

    if(unsupported)
        logFatal("The following OpenGL 2.0 extensions are not supported by your system graphics, please try updating your video drivers or buy a new hardware:\n",
                 ss.str(),
                 "Graphics card: ", glGetString(GL_RENDERER),
                 "\nOpenGL driver: ", glGetString(GL_VERSION));

    if(GLEW_EXT_framebuffer_object && !GLEW_ARB_framebuffer_object) {
        glGenFramebuffers = glGenFramebuffersEXT;
        glDeleteFramebuffers = glDeleteFramebuffersEXT;
        glBindFramebuffer = glBindFramebufferEXT;
        glFramebufferTexture2D = glFramebufferTexture2DEXT;
        glCheckFramebufferStatus = glCheckFramebufferStatusEXT;
    }

    m_useFBO = m_useFBO && (GLEW_ARB_framebuffer_object || GLEW_EXT_framebuffer_object);
    m_generateHardwareMipmaps = m_generateHardwareMipmaps && m_useFBO; // glGenerateMipmap is supported when FBO is
#endif

    glEnable(GL_BLEND);
    //glClear(GL_ACCUM_BUFFER_BIT);

    m_emptyTexture = TexturePtr(new Texture);

    g_painter.init();
}

void Graphics::terminate()
{
    g_fonts.releaseFonts();
    g_painter.terminate();
    m_emptyTexture.reset();
}

bool Graphics::parseOption(const std::string& option)
{
    if(option == "-no-fbos")
        m_useFBO = false;
    else if(option == "-no-mipmapping")
        m_generateMipmaps = false;
    else if(option == "-no-smoothing")
        m_useBilinearFiltering = false;
    else if(option == "-realtime-mipmapping")
        m_generateRealtimeMipmaps = true;
    else if(option == "-no-hardware-buffering")
        m_useHardwareBuffers = false;
    else
        return false;
    return true;
}

void Graphics::resize(const Size& size)
{
    setViewportSize(size);
    //glClear(GL_ACCUM_BUFFER_BIT);

    // The projection matrix converts from Painter's coordinate system to GL's coordinate system
    //    * GL's viewport is 2x2, Painter's is width x height
    //    * GL has +y -> -y going from bottom -> top, Painter is the other way round
    //    * GL has [0,0] in the center, Painter has it in the top-left
    //
    // This results in the Projection matrix below.
    //
    //                Projection Matrix                   Painter Coord   GL Coord
    // ------------------------------------------------     ---------     ---------
    // | 2.0 / width  |      0.0      |     -1.0      |     |   x   |     |   x'  |
    // |     0.0      | -2.0 / height |      1.0      |  *  |   y   |  =  |   y'  |
    // |     0.0      |      0.0      |      0.0      |     |   1   |     |   0   |
    // ------------------------------------------------     ---------     ---------
    Matrix3 projectionMatrix = { 2.0f/size.width(),  0.0f,                -1.0f,
                                 0.0f,              -2.0f/size.height(),   1.0f,
                                 0.0f,               0.0f,                 0.0f };
    projectionMatrix.transpose();
    g_painter.setProjectionMatrix(projectionMatrix);
}

void Graphics::beginRender()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Graphics::endRender()
{
    // this is a simple blur effect
    /*
    static Timer timer;
    if(timer.ticksElapsed() >= 10) {
        glAccum(GL_MULT, 0.9);
        glAccum(GL_ACCUM, 0.1);
        timer.restart();
    }
    glAccum(GL_RETURN, 1);
    */
}

void Graphics::beginClipping(const Rect& clipRect)
{
    // setup stencil buffer for writing
    glClear(GL_STENCIL_BUFFER_BIT);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 1);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

    // draw the clipping area into the stencil buffer
    glColorMask(0, 0, 0, 0);
    g_painter.drawFilledRect(clipRect);

    // set stencil buffer for clipping
    glColorMask(1, 1, 1, 1);
    glStencilFunc(GL_EQUAL, 1, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}

void Graphics::endClipping()
{
    glDisable(GL_STENCIL_TEST);
}

void Graphics::setViewportSize(const Size& size)
{
    glViewport(0, 0, size.width(), size.height());
    m_viewportSize = size;
}

int Graphics::getMaxTextureSize()
{
    static GLint maxTexSize = -1;
    if(maxTexSize == -1)
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);
    return maxTexSize;
}
