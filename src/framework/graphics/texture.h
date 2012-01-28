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
    Texture(int width, int height, int channels, uchar* pixels = NULL);
    virtual ~Texture();

    void bind() { glBindTexture(GL_TEXTURE_2D, m_textureId); }

    void generateMipmaps();
    void setSmooth(bool smooth);
    GLuint getId()  { return m_textureId; }

    std::vector<uint8> getPixels();

    int getWidth() { return m_size.width(); }
    int getHeight() { return m_size.height(); }
    const Size& getSize() { return m_size; }

    bool isEmpty() const { return m_textureId == 0; }

protected:
    void setupFilters();
    GLuint internalLoadGLTexture(uchar* pixels, int channels, int w, int h);

    GLuint m_textureId;
    Size m_size;
    Boolean<false> m_useMipmaps;
    Boolean<false> m_smooth;
};

#endif
