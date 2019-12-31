/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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

class Texture : public stdext::shared_object
{
public:
    Texture();
    Texture(const Size& size);
    Texture(const ImagePtr& image, bool buildMipmaps = false, bool compress = false);
    virtual ~Texture();

    void uploadPixels(const ImagePtr& image, bool buildMipmaps = false, bool compress = false);
    void bind();
    void copyFromScreen(const Rect& screenRect);
    virtual bool buildHardwareMipmaps();

    virtual void setSmooth(bool smooth);
    virtual void setRepeat(bool repeat);
    void setUpsideDown(bool upsideDown);
    void setTime(ticks_t time) { m_time = time; }

    uint getId() { return m_id; }
    ticks_t getTime() { return m_time; }
    int getWidth() { return m_size.width(); }
    int getHeight() { return m_size.height(); }
    const Size& getSize() { return m_size; }
    const Size& getGlSize() { return m_glSize; }
    const Matrix3& getTransformMatrix() { return m_transformMatrix; }
    bool isEmpty() { return m_id == 0; }
    bool hasRepeat() { return m_repeat; }
    bool hasMipmaps() { return m_hasMipmaps; }
    virtual bool isAnimatedTexture() { return false; }

protected:
    void createTexture();
    bool setupSize(const Size& size, bool forcePowerOfTwo = false);
    void setupWrap();
    void setupFilters();
    void setupTranformMatrix();
    void setupPixels(int level, const Size& size, uchar *pixels, int channels = 4, bool compress = false);

    uint m_id;
    ticks_t m_time;
    Size m_size;
    Size m_glSize;
    Matrix3 m_transformMatrix;
    stdext::boolean<false> m_hasMipmaps;
    stdext::boolean<false> m_smooth;
    stdext::boolean<false> m_upsideDown;
    stdext::boolean<false> m_repeat;
};

#endif
