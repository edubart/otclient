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
#include "image.h"

Texture::Texture()
{
    m_textureId = 0;
}

Texture::Texture(const ImagePtr& image)
{
    internalLoadGLTexture(image->getPixelData(), image->getBpp(), image->getWidth(), image->getHeight());
}

Texture::Texture(int width, int height, int channels, uchar *pixels)
{
    // generate opengl texture
    internalLoadGLTexture(pixels, channels, width, height);
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

    // convert texture pixel data to power of two size, only required for OpenGL 1.5 or older
    std::vector<uint8> tmp;
    if(!g_graphics.canUseNonPowerOfTwoTextures()) {
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

        m_glSize.resize(glWidth, glHeight);
    } else
        m_glSize = m_size;

    setupTranformMatrix();

    // checks texture max size
    if(std::max(m_glSize.width(), m_glSize.height()) > g_graphics.getMaxTextureSize()) {
        g_logger.error(stdext::format("loading texture with size %dx%d failed, "
                 "the maximum size allowed by the graphics card is %dx%d,"
                 "to prevent crashes the texture will be displayed as a blank texture",
                 width, height, g_graphics.getMaxTextureSize(), g_graphics.getMaxTextureSize()));
        //TODO: make a workaround, could be bilinear scaling the texture
        return 0;
    }

    // generate gl texture
    GLuint id;
    glGenTextures(1, &id);
    assert(id != 0);
    m_textureId = id;
    bind();

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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_glSize.width(), m_glSize.height(), 0, format, GL_UNSIGNED_BYTE, pixels);

    GLint texParam = GL_REPEAT;
    if(g_graphics.canUseClampToEdge())
        texParam = GL_CLAMP_TO_EDGE; // disable texture borders by default

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texParam);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texParam);

    setupFilters();

    return id;
}

void Texture::copyFromScreen(const Rect& screenRect)
{
    bind();
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, screenRect.x(), screenRect.y(), screenRect.width(), screenRect.height());
}

void Texture::bind()
{
    // must reset painter texture state
    g_painter->setTexture(this);
    glBindTexture(GL_TEXTURE_2D, m_textureId);
}

void Texture::generateMipmaps()
{
    if(!generateHardwareMipmaps()) {
        // fallback to software mipmaps generation, this can be slow
        //FIXME: disabled because mipmaps size needs to be in base of 2,
        //       and the current algorithmn does not support that
        //generateSoftwareMipmaps(getPixels());
        g_logger.traceError("non power of 2.");
    }
}

bool Texture::generateHardwareMipmaps()
{
    if(!g_graphics.canUseHardwareMipmaps())
        return false;

    bind();

    if(!m_hasMipmaps) {
        m_hasMipmaps = true;
        setupFilters();
    }

    glGenerateMipmap(GL_TEXTURE_2D);
    return true;
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

void Texture::setUpsideDown(bool upsideDown)
{
    if(m_upsideDown == upsideDown)
        return;
    m_upsideDown = upsideDown;
    setupTranformMatrix();
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
    std::vector<uint8> outPixels;

    int mipmap = 1;
    while(true) {
        outPixels.resize(outSize.area()*4);

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

void Texture::setupTranformMatrix()
{
    if(m_upsideDown) {
        m_transformMatrix = { 1.0f/m_glSize.width(),  0.0f,                    0.0f,
                              0.0f,                  -1.0f/m_glSize.height(),  0.0f,
                              0.0f,                   1.0f,                    1.0f };
    } else {
        m_transformMatrix = { 1.0f/m_glSize.width(),  0.0f,                    0.0f,
                              0.0f,                   1.0f/m_glSize.height(),  0.0f,
                              0.0f,                   0.0f,                    1.0f };
    }
}
