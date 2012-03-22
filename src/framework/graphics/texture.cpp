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
    m_transformMatrix = { 1.0f/width,  0.0f,
                               0.0f,        1.0f/height };

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
    assert(id != 0);
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

    setupFilters();

    return id;
}

void Texture::generateMipmaps()
{
    if(g_graphics.canGenerateHardwareMipmaps())
        generateHardwareMipmaps();
    else {
        // fallback to software mipmaps generation, this can be slow
        //FIXME: disable because mipmaps size needs to be in base of 2,
        //       and the current algorithmn does not support that
        //generateSoftwareMipmaps(getPixels());
    }
}

void Texture::generateHardwareMipmaps()
{
    if(!g_graphics.canGenerateHardwareMipmaps())
        return;

    bind();

    if(!m_hasMipmaps) {
        m_hasMipmaps = true;
        setupFilters();
    }

    glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::setSmooth(bool smooth)
{
    if(smooth && !g_graphics.canUseBilinearFiltering())
        return;

    if(smooth == m_smooth)
        return;

    m_smooth = smooth;
    bind();
    setupFilters();
}

std::vector<uint8> Texture::getPixels()
{
    std::vector<uint8> pixels(m_size.area()*4, 0);
#ifdef OPENGL_ES2
    // hack to copy pixels from opengl memory in opengl es
    // NOTE: this can be slow, but its the only way to get pixels from a texture in OpenGL ES
    FrameBufferPtr fb(new FrameBuffer(m_size));
    fb->bind();
    g_painter.saveAndResetState();
    g_painter.drawTexturedRect(Rect(0,0,m_size), shared_from_this());
    glReadPixels(0, 0, m_size.width(), m_size.height(), GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
    g_painter.restoreSavedState();
    fb->release();
#else
    // copy pixels from opengl memory
    glBindTexture(GL_TEXTURE_2D, m_textureId);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
#endif
    return pixels;
}

void Texture::generateSoftwareMipmaps(std::vector<uint8> inPixels)
{
    bind();

    if(!m_hasMipmaps) {
        m_hasMipmaps = true;
        setupFilters();
    }

    Size inSize = getSize();
    Size outSize = inSize / 2;
    std::vector<uint8> outPixels(outSize.area()*4);

    dump << "yeah";
    int mipmap = 1;
    while(true) {
        // this is a simple bilinear filtering algorithm, it combines every 4 pixels in one pixel
        for(int x=0;x<outSize.width();++x) {
            for(int y=0;y<outSize.height();++y) {
                uint8 *inPixel[4];
                inPixel[0] = &inPixels[((y*2)*inSize.width() + (x*2))*4];
                inPixel[1] = &inPixels[((y*2)*inSize.width() + (x*2)+1)*4];
                inPixel[2] = &inPixels[((y*2+1)*inSize.width() + (x*2))*4];
                inPixel[3] = &inPixels[((y*2+1)*inSize.width() + (x*2)+1)*4];
                uint8 *outPixel = &outPixels[(y*outSize.width() + x)*4];

                int pixelsSum[4];
                for(int i=0;i<4;++i)
                    pixelsSum[i] = 0;

                int usedPixels = 0;
                for(int j=0;j<4;++j) {
                    // ignore colors of complete alpha pixels
                    if(inPixel[j][3] < 16)
                        continue;

                    for(int i=0;i<4;++i)
                        pixelsSum[i] += inPixel[j][i];

                    usedPixels++;
                }

                // try to guess the alpha pixel more accurately
                for(int i=0;i<4;++i) {
                    if(usedPixels > 0)
                        outPixel[i] = pixelsSum[i] / usedPixels;
                    else
                        outPixel[i] = 0;
                }
                outPixel[3] = pixelsSum[3]/4;
            }
        }

        glTexImage2D(GL_TEXTURE_2D, mipmap++, GL_RGBA, outSize.width(), outSize.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, &outPixels[0]);

        if(inSize.width() == 1 || inSize.height() == 1)
            break;

        inPixels = std::move(outPixels);
        inSize /= 2;
        outSize /= 2;
    }
}

void Texture::setupFilters()
{
    GLint minFilter;
    GLint magFilter;
    if(m_smooth) {
        minFilter = m_hasMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
        magFilter = GL_LINEAR;
    } else {
        minFilter = m_hasMipmaps ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
        magFilter = GL_NEAREST;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}
