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

#include <GL/gl.h>
#include <GL/glext.h>

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
    assert(!g_graphics.isDrawing());
    // free texture from gl memory
    if(m_textureId > 0)
        glDeleteTextures(1, &m_textureId);
}

uint Texture::internalLoadGLTexture(uchar *pixels, int channels, int width, int height)
{
    assert(!g_graphics.isDrawing());

    m_size.setSize(width, height);

    // gets max texture size supported by the driver
    static GLint maxTexSize = -1;
    if(maxTexSize == -1)
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);

    // checks texture max size
    if(width > maxTexSize || height > maxTexSize) {
        logError("loading texture with size ", width, "x", height, " failed, "
                 "the maximum size allowed by the graphics card is ", maxTexSize, "x", maxTexSize, ",",
                 "to prevent crashes the texture will be displayed as a blank texture");
        //TODO: a workground, like bilinear scaling the texture
        return 0;
    }

    // generate gl texture
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    std::vector<uint8> tmp;

    // old opengl drivers only accept power of two dimensions
    if(!g_graphics.isExtensionSupported("GL_ARB_texture_non_power_of_two")) {
        int glWidth = 1;
        while(glWidth < width)
            glWidth = glWidth << 1;

        int glHeight = 1;
        while(glHeight < height)
            glHeight = glHeight << 1;

        if(m_size != m_glSize && pixels) {
            tmp.resize(glHeight*glWidth*channels, 0);
            for(int y=0; y<height; ++y)
                for(int x=0; x<width; ++x)
                    for(int i=0; i<channels; ++i)
                        tmp[y*glWidth*channels+x*channels+i] = pixels[y*width*channels+x*channels+i];
            pixels = &tmp[0];
        }

        m_glSize.setSize(glWidth, glHeight);
    } else
        m_glSize = m_size;

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
    glTexImage2D(GL_TEXTURE_2D, 0, channels, m_glSize.width(), m_glSize.height(), 0, format, GL_UNSIGNED_BYTE, pixels);

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
    // copy pixels from opengl memory
    std::vector<uint8> pixels(m_glSize.area()*4, 0);
    glBindTexture(GL_TEXTURE_2D, m_textureId);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);

    // convert pixels to the real texture size
    if(m_size != m_glSize)
        for(int y=0; y<m_size.height(); ++y)
            for(int x=0; x<m_size.width(); ++x)
                for(int i=0; i<4; ++i)
                    pixels[y*m_size.width()*4+x*4+i] = pixels[y*m_glSize.width()*4+x*4+i];
    return pixels;
}
