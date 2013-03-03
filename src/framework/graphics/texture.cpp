/*
 * Copyright (c) 2010-2013 OTClient <https://github.com/edubart/otclient>
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

Texture::~Texture()
{
#ifndef NDEBUG
    assert(!g_app.isTerminated());
#endif
}

bool Texture::setupSize(const Size& size, bool)
{
    m_size = size;
    setupTranformMatrix(size, size);
    return true;
}

void Texture::setupTranformMatrix(const Size& textureSize, const Size& realSize)
{
    if(m_upsideDown) {
        m_transformMatrix = { 1.0f/textureSize.width(), 0.0f,                                          0.0f,
                              0.0f,                    -1.0f/textureSize.height(),                     0.0f,
                              0.0f,                     realSize.height()/(float)textureSize.height(), 1.0f };
    } else {
        m_transformMatrix = { 1.0f/textureSize.width(), 0.0f,                      0.0f,
                              0.0f,                     1.0f/textureSize.height(), 0.0f,
                              0.0f,                     0.0f,                      1.0f };
    }
}
