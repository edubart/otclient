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

Graphics g_graphics;

Graphics::Graphics()
{

}

Graphics::~Graphics()
{

}

void Graphics::init()
{
    // setup opengl
    glEnable(GL_ALPHA_TEST); // enable alpha by default
    glAlphaFunc(GL_GREATER, 0.0f); // default alpha mode
    glDisable(GL_DEPTH_TEST); // we are rendering 2D only, we don't need it
    glEnable(GL_TEXTURE_2D); // enable textures by default

    notice("GPU %s", (const char*)glGetString(GL_RENDERER));
    notice("OpenGL %s", (const char*)glGetString(GL_VERSION));
}

void Graphics::terminate()
{

}

void Graphics::resize(int width, int height)
{
    m_screenSize.setWidth(width);
    m_screenSize.setHeight(height);
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

void Graphics::drawTexturedRect(const Rect& screenCoords, const Texture *texture, const Rect& texCoords)
{
    // rect correction for opengl
    int right = screenCoords.right()+1;
    int bottom = screenCoords.bottom()+1;
    int top = screenCoords.top();
    int left = screenCoords.left();

    float tright;
    float tbottom;
    float ttop;
    float tleft;

    if(!texCoords.isEmpty()) {
        const Size& textureSize = texture->getSize();
        tright = (float)(texCoords.right()+1)/textureSize.width();
        tbottom = (float)(texCoords.bottom()+1)/textureSize.height();
        ttop = (float)texCoords.top()/textureSize.height();
        tleft = (float)texCoords.left()/textureSize.width();
    } else {
        tright = 0.0f;
        tbottom = 1.0f;
        ttop = 0.0f;
        tleft = 1.0f;
    }

    glBindTexture(GL_TEXTURE_2D, texture->getTextureId());
    glBegin(GL_QUADS);
    glTexCoord2f(tleft,  ttop); glVertex2i(left,  top);
    glTexCoord2f(tleft,  tbottom); glVertex2i(left,  bottom);
    glTexCoord2f(tright, tbottom); glVertex2i(right, bottom);
    glTexCoord2f(tright, ttop); glVertex2i(right, top);
    glEnd();
}

void Graphics::drawColoredRect(const Rect& screenCoords, const Color& color)
{
    glDisable(GL_TEXTURE_2D);

    glColor4ubv(color.rgbaPtr());

    // rect correction for opengl
    int right = screenCoords.right()+1;
    int bottom = screenCoords.bottom()+1;
    int top = screenCoords.top();
    int left = screenCoords.left();

    glBegin(GL_QUADS);
    glVertex2i(left,  top);
    glVertex2i(left,  bottom);
    glVertex2i(right, bottom);
    glVertex2i(right, top);
    glEnd();

    glEnable(GL_TEXTURE_2D);
}


void Graphics::drawBoundingRect(const Rect& screenCoords, const Color& color, int lineWidth)
{
    if(2*lineWidth > screenCoords.height())
        return;

    glDisable(GL_TEXTURE_2D);

    glColor4ubv(color.rgbaPtr());

    // rect correction for opengl
    int right = screenCoords.right()+1;
    int bottom = screenCoords.bottom()+1;
    int top = screenCoords.top();
    int left = screenCoords.left();

    glBegin(GL_QUADS);

    // top line
    glVertex2i(left,  top);
    glVertex2i(left,  top+lineWidth);
    glVertex2i(right, top+lineWidth);
    glVertex2i(right, top);

    // left
    glVertex2i(left, screenCoords.top()+lineWidth);
    glVertex2i(left, bottom-lineWidth);
    glVertex2i(left+lineWidth, bottom-lineWidth);
    glVertex2i(left+lineWidth, screenCoords.top()+lineWidth);

    // bottom line
    glVertex2i(left,  bottom);
    glVertex2i(left,  bottom-lineWidth);
    glVertex2i(right, bottom-lineWidth);
    glVertex2i(right, bottom);

    // right line
    glVertex2i(right,           top+lineWidth);
    glVertex2i(right,           bottom-lineWidth);
    glVertex2i(right-lineWidth, bottom-lineWidth);
    glVertex2i(right-lineWidth, top+lineWidth);

    glEnd();

    glEnable(GL_TEXTURE_2D);
}
