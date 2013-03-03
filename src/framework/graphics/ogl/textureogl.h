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

#ifndef TEXTUREOGL_H
#define TEXTUREOGL_H

#include <framework/graphics/texture.h>

class TextureOGL : public Texture
{
public:
    TextureOGL();
    TextureOGL(const Size& size);
    TextureOGL(const ImagePtr& image, bool buildMipmaps = false);
    virtual ~TextureOGL();

    void uploadPixels(const ImagePtr& image, bool buildMipmaps = false);
    void bind();
    void copyFromScreen(const Rect& screenRect);
    virtual bool buildHardwareMipmaps();

    virtual void setSmooth(bool smooth);
    virtual void setRepeat(bool repeat);
    void setUpsideDown(bool upsideDown);

protected:
    void createTexture();
    bool setupSize(const Size& size, bool forcePowerOfTwo = false);
    void setupWrap();
    void setupFilters();
    void setupPixels(int level, const Size& size, uchar *pixels, int channels = 4);

};

#endif
