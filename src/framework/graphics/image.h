/* The MIT License
 *
 * Copyright (c) 2010 OTClient, https://github.com/edubart/otclient
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


#ifndef IMAGE_H
#define IMAGE_H

#include <prerequisites.h>
#include <graphics/texture.h>

class Image
{
public:
    Image(TexturePtr texture) : m_texture(texture) { }
    Image(TexturePtr texture, Rect textureCoords) : m_texture(texture), m_textureCoords(textureCoords) { }
    Image(const std::string& texture);
    Image(const std::string& texture, Rect textureCoords);

    /// Draw image on screen
    virtual void draw(const Rect& screenCoords);

protected:
    TexturePtr m_texture;
    Rect m_textureCoords;
};

typedef boost::shared_ptr<Image> ImagePtr;

#endif // IMAGE_H
