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

#include "fontmanager.h"

#include <framework/graphics/graphics.h>
#include <framework/graphics/texture.h>
#include <framework/platform/platformwindow.h>

Graphics g_graphics;

void Graphics::init()
{
    glEnable(GL_BLEND);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    logInfo("GPU ", glGetString(GL_RENDERER));
    logInfo("OpenGL ", glGetString(GL_VERSION));

    m_emptyTexture = TexturePtr(new Texture);

    g_painter.init();
}

void Graphics::terminate()
{
    g_fonts.releaseFonts();
    g_painter.terminate();
    m_emptyTexture.reset();
}

bool Graphics::isExtensionSupported(const char *extension)
{
    std::string extensionsString = (const char*)glGetString(GL_EXTENSIONS);
    auto extensions = Fw::split(extensionsString);
    return std::find(extensions.begin(), extensions.end(), extension) != extensions.end();
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
    glClear(GL_COLOR_BUFFER_BIT);
}

void Graphics::endRender()
{
}

void Graphics::setViewportSize(const Size& size)
{
    glViewport(0, 0, size.width(), size.height());
    m_viewportSize = size;
}

