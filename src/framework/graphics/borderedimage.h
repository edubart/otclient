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


#ifndef BORDEREDIMAGE_H
#define BORDEREDIMAGE_H

#include "prerequisites.h"
#include "image.h"
#include "texture.h"

class BorderedImage : public Image
{
public:
    BorderedImage(TexturePtr texture,
                  const Rect& left,
                  const Rect& right,
                  const Rect& top,
                  const Rect& bottom,
                  const Rect& topLeft,
                  const Rect& topRight,
                  const Rect& bottomLeft,
                  const Rect& bottomRight,
                  const Rect& center);

    BorderedImage(const std::string& texture,
                  const Rect& left,
                  const Rect& right,
                  const Rect& top,
                  const Rect& bottom,
                  const Rect& topLeft,
                  const Rect& topRight,
                  const Rect& bottomLeft,
                  const Rect& bottomRight,
                  const Rect& center);

    void setTexCoords(const Rect& left,
                      const Rect& right,
                      const Rect& top,
                      const Rect& bottom,
                      const Rect& topLeft,
                      const Rect& topRight,
                      const Rect& bottomLeft,
                      const Rect& bottomRight,
                      const Rect& center);

    void draw(const Rect& screenCoords);

private:
    Rect m_leftBorderTexCoords;
    Rect m_rightBorderTexCoords;
    Rect m_topBorderTexCoords;
    Rect m_bottomBorderTexCoords;

    Rect m_topLeftCornerTexCoords;
    Rect m_topRightCornerTexCoords;
    Rect m_bottomLeftCornerTexCoords;
    Rect m_bottomRightCornerTexCoords;

    Rect m_centerTexCoords;

    Size m_cornersSize;
};

typedef std::shared_ptr<BorderedImage> BorderedImagePtr;

#endif // BORDEREDIMAGE_H
