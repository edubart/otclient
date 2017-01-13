/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
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

#include <framework/core/application.h>

Texture::Texture()
{
    m_id = 0;
    m_time = 0;
}

Texture::Texture(const Size& size)
{
    m_id = 0;
    m_time = 0;

    if(!setupSize(size))
        return;

    createTexture();
    bind();
    setupPixels(0, m_glSize, nullptr, 4);
    setupWrap();
    setupFilters();
}

Texture::Texture(const ImagePtr& image, bool buildMipmaps, bool compress)
{
    m_id = 0;
    m_time = 0;

    createTexture();

    uploadPixels(image, buildMipmaps, compress);
}

Texture::~Texture()
{
#ifndef NDEBUG
    assert(!g_app.isTerminated());
#endif
    // free texture from gl memory
    if(g_graphics.ok() && m_id != 0)
        glDeleteTextures(1, &m_id);
}

void Texture::uploadPixels(const ImagePtr& image, bool buildMipmaps, bool compress)
{
    if(!setupSize(image->getSize(), buildMipmaps))
        return;

    ImagePtr glImage = image;
    if(m_size != m_glSize) {
        glImage = ImagePtr(new Image(m_glSize, image->getBpp()));
        glImage->paste(image);
    } else
        glImage = image;

    bind();

    if(buildMipmaps) {
        int level = 0;
        do {
            setupPixels(level++, glImage->getSize(), glImage->getPixelData(), glImage->getBpp(), compress);
        } while(glImage->nextMipmap());
        m_hasMipmaps = true;
    } else
        setupPixels(0, glImage->getSize(), glImage->getPixelData(), glImage->getBpp(), compress);

    setupWrap();
    setupFilters();
}

void Texture::bind()
{
    // must reset painter texture state
    g_painter->setTexture(this);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::copyFromScreen(const Rect& screenRect)
{
    bind();
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, screenRect.x(), screenRect.y(), screenRect.width(), screenRect.height());
}

bool Texture::buildHardwareMipmaps()
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

void Texture::setRepeat(bool repeat)
{
    if(m_repeat == repeat)
        return;

    m_repeat = repeat;
    bind();
    setupWrap();
}

void Texture::setUpsideDown(bool upsideDown)
{
    if(m_upsideDown == upsideDown)
        return;
    m_upsideDown = upsideDown;
    setupTranformMatrix();
}

void Texture::createTexture()
{
    glGenTextures(1, &m_id);
    assert(m_id != 0);
}

bool Texture::setupSize(const Size& size, bool forcePowerOfTwo)
{
    Size glSize;
    if(!g_graphics.canUseNonPowerOfTwoTextures() || forcePowerOfTwo)
        glSize.resize(stdext::to_power_of_two(size.width()), stdext::to_power_of_two(size.height()));
    else
        glSize = size;

    // checks texture max size
    if(std::max<int>(glSize.width(), glSize.height()) > g_graphics.getMaxTextureSize()) {
        g_logger.error(stdext::format("loading texture with size %dx%d failed, "
                 "the maximum size allowed by the graphics card is %dx%d,"
                 "to prevent crashes the texture will be displayed as a blank texture",
                 size.width(), size.height(), g_graphics.getMaxTextureSize(), g_graphics.getMaxTextureSize()));
        return false;
    }

    m_size = size;
    m_glSize = glSize;
    setupTranformMatrix();
    return true;
}

void Texture::setupWrap()
{
    int texParam;
    if(!m_repeat && g_graphics.canUseClampToEdge())
        texParam = GL_CLAMP_TO_EDGE;
    else
        texParam = GL_REPEAT;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texParam);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texParam);
}

void Texture::setupFilters()
{
    int minFilter;
    int magFilter;
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
        m_transformMatrix = { 1.0f/m_glSize.width(),  0.0f,                                     0.0f,
                              0.0f,                  -1.0f/m_glSize.height(),                   0.0f,
                              0.0f,                   m_size.height()/(float)m_glSize.height(), 1.0f };
    } else {
        m_transformMatrix = { 1.0f/m_glSize.width(),  0.0f,                    0.0f,
                              0.0f,                   1.0f/m_glSize.height(),  0.0f,
                              0.0f,                   0.0f,                    1.0f };
    }
}

void Texture::setupPixels(int level, const Size& size, uchar* pixels, int channels, bool compress)
{
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

    GLenum internalFormat = GL_RGBA;

#ifdef OPENGL_ES
    //TODO
#else
    if(compress)
        internalFormat = GL_COMPRESSED_RGBA;
#endif

    glTexImage2D(GL_TEXTURE_2D, level, internalFormat, size.width(), size.height(), 0, format, GL_UNSIGNED_BYTE, pixels);
}
