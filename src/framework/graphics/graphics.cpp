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

#if OPENGL_ES==2
#include "painterogl2.h"
#elif OPENGL_ES==1
#include "painterogl1.h"
#else
#include "painterogl1.h"
#include "painterogl2.h"
#endif

#include <framework/graphics/graphics.h>
#include <framework/graphics/texture.h>
#include <framework/platform/platformwindow.h>

Graphics g_graphics;

Graphics::Graphics()
{
    m_maxTextureSize = -1;
    m_selectedPainterEngine = Painter_Any;
}

void Graphics::init()
{
    g_logger.info(stdext::format("GPU %s", glGetString(GL_RENDERER)));
    g_logger.info(stdext::format("OpenGL %s", glGetString(GL_VERSION)));

#if OPENGL_ES==2
    g_painterOGL2 = new PainterOGL2;
#elif OPENGL_ES==1
    g_painterOGL1 = new PainterOGL1;
#else
    // init GL extensions
    GLenum err = glewInit();
    if(err != GLEW_OK)
        g_logger.fatal(stdext::format("Unable to init GLEW: %s", glewGetErrorString(err)));

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
#endif

    // blending is always enabled
    glEnable(GL_BLEND);

    // determine max texture size
    GLint maxTextureSize = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    if(m_maxTextureSize == -1 || m_maxTextureSize > maxTextureSize)
        m_maxTextureSize = maxTextureSize;

    selectPainterEngine(m_prefferedPainterEngine);
    m_emptyTexture = TexturePtr(new Texture);
}

void Graphics::terminate()
{
    g_fonts.releaseFonts();

#ifdef PAINTER_OGL2
    if(g_painterOGL2) {
        delete g_painterOGL2;
        g_painterOGL2 = nullptr;
    }
#endif

#ifdef PAINTER_OGL1
    if(g_painterOGL1) {
        delete g_painterOGL1;
        g_painterOGL1 = nullptr;
    }
#endif

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
    else if(option == "-hardware-buffers")
        m_useHardwareBuffers = true;
    else if(option == "-no-non-power-of-two-textures")
        m_useNonPowerOfTwoTextures = false;
    else if(option == "-no-clamp-to-edge")
        m_useClampToEdge = false;
    else if(option == "-no-backbuffer-cache")
        m_cacheBackbuffer = false;
    else if(option == "-opengl1")
        m_prefferedPainterEngine = Painter_OpenGL1;
    else if(option == "-opengl2")
        m_prefferedPainterEngine = Painter_OpenGL2;
    else
        return false;
    return true;
}

bool Graphics::isPainterEngineAvailable(Graphics::PainterEngine painterEngine)
{
#ifdef PAINTER_OGL2
    if(g_painterOGL2 && painterEngine == Painter_OpenGL2)
        return true;
#endif

#ifdef PAINTER_OGL1
    if(g_painterOGL1 && painterEngine == Painter_OpenGL1)
        return true;
#endif
    return false;
}

bool Graphics::selectPainterEngine(PainterEngine painterEngine)
{
    Painter *painter = nullptr;
#ifdef PAINTER_OGL2
    // always prefer OpenGL 2 over OpenGL 1
    if(!painter && g_painterOGL2 && (painterEngine == Painter_OpenGL2 || painterEngine == Painter_Any)) {
        m_selectedPainterEngine = Painter_OpenGL2;
        painter = g_painterOGL2;
    }
#endif

#ifdef PAINTER_OGL1
    // fallback to OpenGL 1 in older hardwares
    if(!painter && g_painterOGL1 && (painterEngine == Painter_OpenGL1 || painterEngine == Painter_Any)) {
        m_selectedPainterEngine = Painter_OpenGL1;
        painter = g_painterOGL1;
    }
#endif

    // switch painters GL state
    if(painter && painter != g_painter) {
        if(g_painter)
            g_painter->unbind();
        painter->bind();
        g_painter = painter;

        if(painterEngine == Painter_Any)
            return true;
    } else
        g_logger.fatal("Neither OpenGL 1.0 nor OpenGL 2.0 painter engine is supported by your platform, "
                       "try updating your graphics drivers or your hardware and then run again.");

    return m_selectedPainterEngine == painterEngine;
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

#ifdef PAINTER_OGL1
    if(g_painterOGL1)
        g_painterOGL1->setProjectionMatrix(projectionMatrix);
#endif

#ifdef PAINTER_OGL2
    if(g_painterOGL2)
        g_painterOGL2->setProjectionMatrix(projectionMatrix);
#endif
}

void Graphics::beginRender()
{
    //g_painter->clear(Color::black);
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
#ifdef OPENGL_ES
    return true;
#else
    // glDrawArrays is supported by OpenGL 1.1
    if(!GLEW_VERSION_1_1)
        return false;
    return m_useDrawArrays;
#endif
}

bool Graphics::canUseShaders()
{
#if OPENGL_ES==2
    return true;
#elif OPENGL_ES==1
    return false;
#else
    // fragment and vertex programs are supported by OpenGL 2.0
    if(GLEW_ARB_vertex_program && GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
        return true;
    return false;
#endif
}

bool Graphics::canUseFBO()
{
#if OPENGL_ES==2
    return m_useFBO;
#elif OPENGL_ES==1
    return false;
#else
    // FBOs are supported by OpenGL 3.0
    // or by OpenGL 2.0 with EXT_framebuffer_object (most of the OpenGL 2.0 implementations have this extension)
    if(!GLEW_ARB_framebuffer_object || !GLEW_EXT_framebuffer_object)
        return false;
    return m_useFBO;
#endif
}

bool Graphics::canUseBilinearFiltering()
{
    // bilinear filtering is supported by any OpenGL implementation
    return m_useBilinearFiltering;
}

bool Graphics::canUseHardwareBuffers()
{
#if OPENGL_ES==2
    return m_useHardwareBuffers;
#elif OPENGL_ES==1
    // OpenGL ES 1.1 supports it but OpenGL ES 1.0 not
    return false;
#else
    // vertex buffer objects is supported by OpenGL 1.5
    if(!GLEW_ARB_vertex_buffer_object)
        return false;
#endif
    return m_useHardwareBuffers;
}

bool Graphics::canUseNonPowerOfTwoTextures()
{
#if OPENGL_ES==2
    return m_useNonPowerOfTwoTextures;
#elif OPENGL_ES==1
    return false;
#else
    // power of two textures is supported by OpenGL 2.0
    if(!GLEW_ARB_texture_non_power_of_two)
        return false;
    return m_useNonPowerOfTwoTextures;
#endif
}

bool Graphics::canUseMipmaps()
{
    // mipmaps is supported by any OpenGL implementation
    return m_useMipmaps;
}

bool Graphics::canUseHardwareMipmaps()
{
#if OPENGL_ES==2
    return m_useHardwareMipmaps;
#elif OPENGL_ES==1
    return false;
#else
    // glGenerateMipmap is supported when FBOs are
    if(!GLEW_ARB_framebuffer_object || !GLEW_EXT_framebuffer_object)
        return false;
    return m_useHardwareMipmaps;
#endif
}

bool Graphics::canUseClampToEdge()
{
#ifdef OPENGL_ES
    return m_useClampToEdge;
#else
    // GL_CLAMP_TO_EDGE is present in OpenGL 1.2
    if(!GLEW_VERSION_1_2)
        return false;
    return m_useClampToEdge;
#endif
}

bool Graphics::canUseBlendFuncSeparate()
{
#if OPENGL_ES==2
    return true;
#elif OPENGL_ES==1
    return false;
#else
    if(!GLEW_VERSION_1_4)
        return false;
    return true;
#endif
}

bool Graphics::canCacheBackbuffer()
{
#if OPENGL_ES==2
    return m_cacheBackbuffer;
#elif OPENGL_ES==1
    return false;
#else
    if(!GLEW_VERSION_1_4)
        return false;
    return m_cacheBackbuffer;
#endif
}
