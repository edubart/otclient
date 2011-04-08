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


#include "borderedimage.h"
#include "graphics.h"
#include "textures.h"

#include <GL/gl.h>

BorderedImage::BorderedImage(TexturePtr texture,
                             const Rect& left,
                             const Rect& right,
                             const Rect& top,
                             const Rect& bottom,
                             const Rect& topLeft,
                             const Rect& topRight,
                             const Rect& bottomLeft,
                             const Rect& bottomRight,
                             const Rect& center) : Image(texture)
{
    setTexCoords(left, right, top, bottom, topLeft, topRight, bottomLeft, bottomRight, center);
}

BorderedImage::BorderedImage(const std::string& texture,
                             const Rect& left,
                             const Rect& right,
                             const Rect& top,
                             const Rect& bottom,
                             const Rect& topLeft,
                             const Rect& topRight,
                             const Rect& bottomLeft,
                             const Rect& bottomRight,
                             const Rect& center) : Image(texture)
{
    setTexCoords(left, right, top, bottom, topLeft, topRight, bottomLeft, bottomRight, center);
}

void BorderedImage::setTexCoords(const Rect& left,
                                 const Rect& right,
                                 const Rect& top,
                                 const Rect& bottom,
                                 const Rect& topLeft,
                                 const Rect& topRight,
                                 const Rect& bottomLeft,
                                 const Rect& bottomRight,
                                 const Rect& center)
{
    m_leftBorderTexCoords = left;
    m_rightBorderTexCoords = right;
    m_topBorderTexCoords = top;
    m_bottomBorderTexCoords = bottom;
    m_topLeftCornerTexCoords = topLeft;
    m_topRightCornerTexCoords = topRight;
    m_bottomLeftCornerTexCoords = bottomLeft;
    m_bottomRightCornerTexCoords = bottomRight;
    m_centerTexCoords = center;

    m_cornersSize = Size(topLeft.width() + topRight.width(),
                         topLeft.height() + bottomLeft.height());
}

void BorderedImage::draw(const Rect& screenCoords)
{
    // check minumim size
    if(screenCoords.size() <= m_cornersSize)
        return;

    const Size& textureSize = m_texture->getSize();
    Rect rectCoords;
    Size centerSize = screenCoords.size() - m_cornersSize;

    g_graphics._beginTextureRender(m_texture.get());

    // first the center
    rectCoords = Rect(screenCoords.topLeft() + m_topLeftCornerTexCoords.size().toPoint(),
                      centerSize);
    g_graphics._drawRepeatedTexturedRect(rectCoords,  m_centerTexCoords, textureSize);

    // top left corner
    rectCoords = Rect(screenCoords.topLeft(),
                      m_topLeftCornerTexCoords.size());
    g_graphics._drawTexturedRect(rectCoords,  m_topLeftCornerTexCoords, textureSize);

    // top
    rectCoords = Rect(screenCoords.left() + m_topLeftCornerTexCoords.width(),
                      screenCoords.topLeft().y,
                      centerSize.width(),
                      m_topBorderTexCoords.height());
    g_graphics._drawRepeatedTexturedRect(rectCoords,  m_topBorderTexCoords, textureSize);


    // top right corner
    rectCoords = Rect(screenCoords.left() + m_topLeftCornerTexCoords.width() + centerSize.width(),
                      screenCoords.top(),
                      m_topRightCornerTexCoords.size());
    g_graphics._drawTexturedRect(rectCoords,  m_topRightCornerTexCoords, textureSize);

    // left
    rectCoords = Rect(screenCoords.left(),
                      screenCoords.top() + m_topLeftCornerTexCoords.height(),
                      m_leftBorderTexCoords.width(),
                      centerSize.height());
    g_graphics._drawRepeatedTexturedRect(rectCoords,  m_leftBorderTexCoords, textureSize);

    // right
    rectCoords = Rect(screenCoords.left() + m_topLeftCornerTexCoords.width() + centerSize.width(),
                      screenCoords.top() + m_topRightCornerTexCoords.height(),
                      m_rightBorderTexCoords.width(),
                      centerSize.height());
    g_graphics._drawRepeatedTexturedRect(rectCoords,  m_rightBorderTexCoords, textureSize);

    // bottom left corner
    rectCoords = Rect(screenCoords.left(),
                      screenCoords.top() + m_topLeftCornerTexCoords.height() + centerSize.height(),
                      m_bottomLeftCornerTexCoords.size());
    g_graphics._drawTexturedRect(rectCoords,  m_bottomLeftCornerTexCoords, textureSize);

    // bottom
    rectCoords = Rect(screenCoords.left() + m_bottomLeftCornerTexCoords.width(),
                      screenCoords.top() + m_topBorderTexCoords.height() + centerSize.height(),
                      centerSize.width(),
                      m_bottomBorderTexCoords.height());
    g_graphics._drawRepeatedTexturedRect(rectCoords,  m_bottomBorderTexCoords, textureSize);

    // bottom right corner
    rectCoords = Rect(screenCoords.left() + m_bottomLeftCornerTexCoords.width() + centerSize.width(),
                      screenCoords.top() + m_topRightCornerTexCoords.height() + centerSize.height(),
                      m_bottomRightCornerTexCoords.size());
    g_graphics._drawTexturedRect(rectCoords,  m_bottomRightCornerTexCoords, textureSize);

    g_graphics._endTextureRender();
}
