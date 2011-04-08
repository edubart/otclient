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


#include "graphics.h"
#include "logger.h"
#include "texture.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

Graphics g_graphics;

void Graphics::init()
{
    // setup opengl
    glEnable(GL_ALPHA_TEST); // enable alpha by default
    glAlphaFunc(GL_GREATER, 0.0f); // default alpha mode
    glDisable(GL_DEPTH_TEST); // we are rendering 2D only, we don't need it
    glEnable(GL_TEXTURE_2D); // enable textures by default
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glShadeModel(GL_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    logInfo("GPU %s", (const char*)glGetString(GL_RENDERER));
    logInfo("OpenGL %s", (const char*)glGetString(GL_VERSION));
}

void Graphics::terminate()
{

}

bool Graphics::isExtensionSupported(const char *extension)
{
    const GLubyte *extensions = NULL;
    const GLubyte *start;
    GLubyte *where, *terminator;
    where = (GLubyte *)strchr(extension, ' ');

    if(where || *extension == '\0')
        return 0;

    extensions = glGetString(GL_EXTENSIONS);

    start = extensions;
    while(true) {
        where = (GLubyte *) strstr((const char *)start, extension);
        if(!where)
            break;

        terminator = where + strlen(extension);

        if(where == start || *(where - 1) == ' ')
            if(*terminator == ' ' || *terminator == '\0')
                return 1;

        start = terminator;
    }
    return 0;
}

void Graphics::resize(const Size& size)
{
    m_screenSize = size;
    restoreViewport();
}

void Graphics::restoreViewport()
{
    const int& width = m_screenSize.width();
    const int& height = m_screenSize.height();

    // resize gl viewport
    glViewport(0, 0, width, height);

    /*
     0,0---------0,w
     |           |
     |           |
     |           |
     h,0---------h,w
     */
    // setup view region like above
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0f, width, height, 0.0f);

    // back to model view
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Graphics::beginRender()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
}

void Graphics::endRender()
{

}


void Graphics::setColor(const Color& color)
{
    glColor4ubv(color.rgbaPtr());
}

void Graphics::resetColor()
{
    glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);
}

void Graphics::_beginTextureRender(const Texture *texture)
{
    glBindTexture(GL_TEXTURE_2D, texture->getTextureId());
    glBegin(GL_QUADS);
}

void Graphics::_endTextureRender()
{
    glEnd();
}

void Graphics::drawTexturedRect(const Rect& screenCoords, const Texture *texture, const Rect& textureCoords)
{
    glBindTexture(GL_TEXTURE_2D, texture->getTextureId());
    glBegin(GL_QUADS);
    _drawTexturedRect(screenCoords, textureCoords, texture->getSize());
    glEnd();
}

void Graphics::_drawTexturedRect(const Rect& screenCoords, const Rect& textureCoords, const Size& textureSize)
{
    // rect correction for opengl
    int right = screenCoords.right() + 1;
    int bottom = screenCoords.bottom() + 1;
    int top = screenCoords.top();
    int left = screenCoords.left();

    float textureRight = 0.0f;
    float textureBottom = 1.0f;
    float textureTop = 0.0f;
    float textureLeft = 1.0f;

    if(!textureCoords.isEmpty()) {
        textureRight = (float)(textureCoords.right() + 1) / textureSize.width();
        textureBottom = (float)(textureCoords.bottom() + 1) / textureSize.height();
        textureTop = (float)textureCoords.top() / textureSize.height();
        textureLeft = (float)textureCoords.left() / textureSize.width();
    }

    glTexCoord2f(textureLeft,  textureTop);    glVertex2i(left,  top);
    glTexCoord2f(textureLeft,  textureBottom); glVertex2i(left,  bottom);
    glTexCoord2f(textureRight, textureBottom); glVertex2i(right, bottom);
    glTexCoord2f(textureRight, textureTop);    glVertex2i(right, top);
}

void Graphics::drawColoredRect(const Rect& screenCoords, const Color& color)
{
    glDisable(GL_TEXTURE_2D);

    setColor(color);

    // rect correction for opengl
    int right = screenCoords.right() + 1;
    int bottom = screenCoords.bottom() + 1;
    int top = screenCoords.top();
    int left = screenCoords.left();

    glBegin(GL_QUADS);
    glVertex2i(left,  top);
    glVertex2i(left,  bottom);
    glVertex2i(right, bottom);
    glVertex2i(right, top);
    glEnd();

    glEnable(GL_TEXTURE_2D);

    resetColor();
}


void Graphics::drawBoundingRect(const Rect& screenCoords, const Color& color, int innerLineWidth)
{
    if(2 * innerLineWidth > screenCoords.height())
        return;

    glDisable(GL_TEXTURE_2D);

    setColor(color);

    // rect correction for opengl
    int right = screenCoords.right()+1;
    int bottom = screenCoords.bottom()+1;
    int top = screenCoords.top();
    int left = screenCoords.left();

    glBegin(GL_QUADS);
        // top line
        glVertex2i(left,  top);
        glVertex2i(left,  top + innerLineWidth);
        glVertex2i(right, top + innerLineWidth);
        glVertex2i(right, top);

        // left
        glVertex2i(left, screenCoords.top() + innerLineWidth);
        glVertex2i(left, bottom - innerLineWidth);
        glVertex2i(left + innerLineWidth, bottom - innerLineWidth);
        glVertex2i(left + innerLineWidth, screenCoords.top() + innerLineWidth);

        // bottom line
        glVertex2i(left,  bottom);
        glVertex2i(left,  bottom - innerLineWidth);
        glVertex2i(right, bottom - innerLineWidth);
        glVertex2i(right, bottom);

        // right line
        glVertex2i(right                 , top + innerLineWidth);
        glVertex2i(right                 , bottom - innerLineWidth);
        glVertex2i(right - innerLineWidth, bottom - innerLineWidth);
        glVertex2i(right - innerLineWidth, top + innerLineWidth);
    glEnd();

    glEnable(GL_TEXTURE_2D);

    resetColor();
}
