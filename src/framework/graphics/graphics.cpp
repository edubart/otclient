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
#include "painterogl1.h"
#include "painterogl2.h"

#include <framework/graphics/graphics.h>
#include <framework/graphics/texture.h>
#include <framework/platform/platformwindow.h>

Graphics g_graphics;

void oglDebugCallback(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char* message, void* userParam)
{
    logWarning("OGL: ", message);
}

Graphics::Graphics()
{
    m_maxTextureSize = -1;
}

void Graphics::init()
{
    logInfo("GPU ", glGetString(GL_RENDERER));
    logInfo("OpenGL ", glGetString(GL_VERSION));


#ifndef OPENGL_ES2
    // init GL extensions
    GLenum err = glewInit();
    if(err != GLEW_OK)
        logFatal("Unable to init GLEW: ", glewGetErrorString(err));

#ifdef DEBUG_OPENGL
    if(GLEW_ARB_debug_output)
        glDebugMessageCallbackARB(oglDebugCallback, NULL);
#endif

    // overwrite framebuffer API if needed
    if(GLEW_EXT_framebuffer_object && !GLEW_ARB_framebuffer_object) {
        glGenFramebuffers = glGenFramebuffersEXT;
        glDeleteFramebuffers = glDeleteFramebuffersEXT;
        glBindFramebuffer = glBindFramebufferEXT;
        glFramebufferTexture2D = glFramebufferTexture2DEXT;
        glCheckFramebufferStatus = glCheckFramebufferStatusEXT;
        glGenerateMipmap = glGenerateMipmapEXT;
    }

    // opengl 1 is always supported
    g_painterOGL1 = new PainterOGL1;

    // opengl 2 is only supported in newer hardware
    if(GLEW_VERSION_2_0)
        g_painterOGL2 = new PainterOGL2;
#else
    g_painterOGL2 = new PainterOGL2;
#endif

    // determine max texture size
    static GLint maxTextureSize = -1;
    if(maxTextureSize == -1)
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    if(m_maxTextureSize == -1 || m_maxTextureSize > maxTextureSize)
        m_maxTextureSize = maxTextureSize;

    //glClear(GL_COLOR_BUFFER_BIT);
    //m_prefferedPainterEngine = Painter_OpenGL1;

    selectPainterEngine(m_prefferedPainterEngine);
    m_emptyTexture = TexturePtr(new Texture);

}

void Graphics::terminate()
{
    g_fonts.releaseFonts();

    if(g_painterOGL1) {
        delete g_painterOGL1;
        g_painterOGL1 = nullptr;
    }

    if(g_painterOGL2) {
        delete g_painterOGL2;
        g_painterOGL2 = nullptr;
    }

    g_painter = nullptr;

    m_emptyTexture.reset();
}

bool Graphics::parseOption(const std::string& option)
{
    if(option == "-no-draw-arrays")
        m_useDrawArrays = false;
    else if(option == "-no-fbos")
        m_useFBO = false;
    else if(option == "-no-mipmaps")
        m_useMipmaps = false;
    else if(option == "-no-hardware-mipmaps")
        m_useHardwareMipmaps = false;
    else if(option == "-no-smooth")
        m_useBilinearFiltering = false;
    else if(option == "-no-hardware-buffers")
        m_useHardwareBuffers = false;
    else if(option == "-no-non-power-of-two-textures")
        m_useNonPowerOfTwoTextures = false;
    else if(option == "-opengl1")
        m_prefferedPainterEngine = Painter_OpenGL1;
    else if(option == "-opengl2")
        m_prefferedPainterEngine = Painter_OpenGL2;
    else
        return false;
    return true;
}

bool Graphics::selectPainterEngine(PainterEngine painterEngine)
{
    // always prefer OpenGL 2 over OpenGL 1
    if(g_painterOGL2 && (painterEngine == Painter_OpenGL2 || painterEngine == Painter_Any))
        g_painter = g_painterOGL2;
    // fallback to OpenGL 1 in older hardwares
    else if(g_painterOGL1 && (painterEngine == Painter_OpenGL1 || painterEngine == Painter_Any))
        g_painter = g_painterOGL1;
    else
        logFatal("Neither OpenGL 1.0 nor OpenGL 2.0 painter engine is supported by your platform, "
                 "try updating your graphics drivers or your hardware and then run again.");

    // switch painters GL state
    if(g_painter)
        g_painter->unbind();
    g_painter->bind();

    if(painterEngine == Painter_Any)
        return true;
    return getPainterEngine() == painterEngine;
}

Graphics::PainterEngine Graphics::getPainterEngine()
{
    if(g_painter == g_painterOGL2)
        return Painter_OpenGL2;
    else
        return Painter_OpenGL1;
}

void Graphics::resize(const Size& size)
{
    setViewportSize(size);

    // The projection matrix converts from Painter's coordinate system to GL's coordinate system
    //    * GL's viewport is 2x2, Painter's is width x height
    //    * GL has +y -> -y going from bottom -> top, Painter is the other way round
    //    * GL has [0,0] in the center, Painter has it in the top-left
    //
    // This results in the Projection matrix below.
    //
    //                                    Projection Matrix
    //   Painter Coord     ------------------------------------------------        GL Coord
    //   -------------     | 2.0 / width  |      0.0      |      0.0      |     ---------------
    //   |  x  y  1  |  *  |     0.0      | -2.0 / height |      0.0      |  =  |  x'  y'  1  |
    //   -------------     |    -1.0      |      1.0      |      1.0      |     ---------------
    //                     ------------------------------------------------
    Matrix3 projectionMatrix = { 2.0f/size.width(),  0.0f,                 0.0f,
                                 0.0f,              -2.0f/size.height(),   0.0f,
                                -1.0f,               1.0f,                 1.0f };

    if(g_painterOGL1)
        g_painterOGL1->setProjectionMatrix(projectionMatrix);

    if(g_painterOGL2)
        g_painterOGL2->setProjectionMatrix(projectionMatrix);
}

void Graphics::beginRender()
{
    //glClearColor(0, 0, 0, 1);
    //glClear(GL_COLOR_BUFFER_BIT);
}

void Graphics::endRender()
{
}

void Graphics::setViewportSize(const Size& size)
{
    glViewport(0, 0, size.width(), size.height());
    m_viewportSize = size;
}

bool Graphics::canUseDrawArrays()
{
#ifndef OPENGL_ES2
    if(!GLEW_VERSION_1_1)
        return false;
#else
    return false;
#endif
    return m_useDrawArrays;
}

bool Graphics::canUseShaders()
{
#ifndef OPENGL_ES2
    if(GLEW_ARB_vertex_program && GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
        return true;
#else
    return true;
#endif
    return false;
}

bool Graphics::canUseFBO()
{
#ifndef OPENGL_ES2
    if(!GLEW_ARB_framebuffer_object || !GLEW_EXT_framebuffer_object)
        return false;
#endif
    return m_useFBO;
}

bool Graphics::canUseBilinearFiltering()
{
    return m_useBilinearFiltering;
}

bool Graphics::canUseHardwareBuffers()
{
#ifndef OPENGL_ES2
    if(!GLEW_ARB_vertex_buffer_object)
        return false;
#endif
    return m_useHardwareBuffers;
}

bool Graphics::canUseNonPowerOfTwoTextures()
{
#ifndef OPENGL_ES2
    if(!GLEW_ARB_texture_non_power_of_two)
        return false;
#endif
    return m_useNonPowerOfTwoTextures;
}

bool Graphics::canUseMipmaps()
{
    return m_useMipmaps;
}

bool Graphics::canUseHardwareMipmaps()
{
#ifndef OPENGL_ES2
    // glGenerateMipmap is supported when framebuffers are too
    if(!GLEW_ARB_framebuffer_object || !GLEW_EXT_framebuffer_object)
        return false;
#endif
    return m_useHardwareMipmaps;
}
