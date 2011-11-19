/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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

#ifndef UIIMAGE_H
#define UIIMAGE_H

#include <framework/graphics/declarations.h>
#include <framework/otml/declarations.h>
#include "declarations.h"

class UIImage
{
public:
    void draw(const Rect& screenCoords);
    void applyStyle(const OTMLNodePtr& styleNode);

    void setImageSource(const std::string& source);
    void setImageClip(const Rect& clipRect);
    void setImageRect(const Rect& rect);
    void setImageFixedRatio(bool fixedRatio);
    void setImageRepeated(bool repeated);
    void setImageSmooth(bool smooth);
    void setImageColor(const Color& color);
    void setImageBorderTop(int border);
    void setImageBorderRight(int border);
    void setImageBorderBottom(int border);
    void setImageBorderLeft(int border);

protected:
    TexturePtr m_imageTexture;
    Rect m_imageClipRect;
    Rect m_imageRect;
    Boolean<false> m_imageFixedRatio;
    Boolean<false> m_imageRepeated;
    Color m_imageColor;

    // border image coords
    Rect m_leftBorderTexCoords;
    Rect m_rightBorderTexCoords;
    Rect m_topBorderTexCoords;
    Rect m_bottomBorderTexCoords;
    Rect m_topLeftCornerTexCoords;
    Rect m_topRightCornerTexCoords;
    Rect m_bottomLeftCornerTexCoords;
    Rect m_bottomRightCornerTexCoords;
    Rect m_centerTexCoords;
    Size m_bordersSize;
};

#endif
