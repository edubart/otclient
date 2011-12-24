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

#include "texture.h"
#include "graphics.h"
#include "framebuffer.h"

Texture::Texture()
{
    m_textureId = 0;
}

Texture::Texture(int width, int height, int channels, uchar *pixels)
{
    // generate opengl texture
    m_textureId = internalLoadGLTexture(pixels, channels, width, height);
}

Texture::~Texture()
{
    // free texture from gl memory
    if(m_textureId > 0)
        glDeleteTextures(1, &m_textureId);
}

uint Texture::internalLoadGLTexture(uchar *pixels, int channels, int width, int height)
{
    m_size.resize(width, height);

    // gets max texture size supported by the driver
    static GLint maxTexSize = -1;
    if(maxTexSize == -1)
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);

    // checks texture max size
    if(width > maxTexSize || height > maxTexSize) {
        logError("loading texture with size ", width, "x", height, " failed, "
                 "the maximum size allowed by the graphics card is ", maxTexSize, "x", maxTexSize, ",",
                 "to prevent crashes the texture will be displayed as a blank texture");
        //TODO: make a workaround, could be bilinear scaling the texture
        return 0;
    }

    // generate gl texture
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    // detect pixels GL format
    GLenum format = 0;
    switch(channels) {
        case 4:
            format = GL_RGBA;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 2:
            format = GL_LUMINANCE_ALPHA;
            break;
        case 1:
            format = GL_LUMINANCE;
            break;
    }

    // load pixels into gl memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_size.width(), m_size.height(), 0, format, GL_UNSIGNED_BYTE, pixels);

    // disable texture border
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // nearest filtering (non smooth)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return id;
}

void Texture::enableBilinearFilter()
{
    // enable smooth texture
    glBindTexture(GL_TEXTURE_2D, m_textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

std::vector<uint8> Texture::getPixels()
{
    // hack to copy pixels from opengl memory
    FrameBufferPtr fb(new FrameBuffer(m_size));
    std::vector<uint8> pixels(m_size.area()*4, 0);
    fb->bind();
    g_painter.drawTexturedRect(Rect(0,0,m_size), shared_from_this());
    glReadPixels(0, 0, m_size.width(), m_size.height(), GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
    fb->release();
    return pixels;
}
