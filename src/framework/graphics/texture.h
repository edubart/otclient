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

#ifndef TEXTURE_H
#define TEXTURE_H

#include "declarations.h"

class Texture : public std::enable_shared_from_this<Texture>
{
public:
    Texture();
    Texture(const Size& size);
    Texture(const ImagePtr& image, bool buildMipmaps = false);
    virtual ~Texture();

    void bind();
    void copyFromScreen(const Rect& screenRect);
    bool buildHardwareMipmaps();

    void setSmooth(bool smooth);
    void setUpsideDown(bool upsideDown);

    GLuint getId()  { return m_id; }
    int getWidth() { return m_size.width(); }
    int getHeight() { return m_size.height(); }
    const Size& getSize() { return m_size; }
    const Size& getGlSize() { return m_glSize; }
    const Matrix3& getTransformMatrix() { return m_transformMatrix; }
    bool isEmpty() { return m_id == 0; }
    bool hasMipmaps() { return m_hasMipmaps; }

protected:
    void createTexture();
    bool setupSize(const Size& size, bool forcePowerOfTwo = false);
    void setupWrap();
    void setupFilters();
    void setupTranformMatrix();
    void setupPixels(int level, const Size& size, uchar *pixels, int channels = 4);

    GLuint m_id;
    Size m_size;
    Size m_glSize;
    Matrix3 m_transformMatrix;
    Boolean<false> m_hasMipmaps;
    Boolean<false> m_smooth;
    Boolean<false> m_upsideDown;
};

#endif
