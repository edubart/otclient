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

#include "borderimage.h"
#include "graphics.h"
#include "texture.h"
#include "texturemanager.h"

#include <framework/otml/otml.h>

BorderImage::BorderImage(TexturePtr texture,
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
    m_leftBorderTexCoords = left;
    m_rightBorderTexCoords = right;
    m_topBorderTexCoords = top;
    m_bottomBorderTexCoords = bottom;
    m_topLeftCornerTexCoords = topLeft;
    m_topRightCornerTexCoords = topRight;
    m_bottomLeftCornerTexCoords = bottomLeft;
    m_bottomRightCornerTexCoords = bottomRight;
    m_centerTexCoords = center;

    m_bordersSize = Size(left.width() + right.width(),
                         top.height() + bottom.height());

    m_defaultSize = Size(std::max(std::max(topLeft.width(), bottomLeft.width()), left.width()) +
                            std::max(std::max(topRight.width(), bottomRight.width()), right.width()) +
                            center.width(),
                         std::max(std::max(topLeft.height(), topRight.height()), top.height()) +
                            std::max(std::max(bottomLeft.height(), bottomRight.height()), bottom.height()) +
                            center.height());
}

BorderImagePtr BorderImage::loadFromOTML(const OTMLNodePtr& borderImageNode)
{
    Rect leftBorder;
    Rect rightBorder;
    Rect topBorder;
    Rect bottomBorder;
    Rect topLeftCorner;
    Rect topRightCorner;
    Rect bottomLeftCorner;
    Rect bottomRightCorner;
    Rect center;
    Rect subRect;
    int top, bottom, left, right, border;
    Size size;
    Point offset;

    // load texture
    std::string source = borderImageNode->at("source")->value();
    TexturePtr texture = g_textures.getTexture(source);
    if(!texture)
        throw OTMLException(borderImageNode, "could not load border-image texture");

    // load basic border confs
    size = texture->getSize();
    size = borderImageNode->valueAt("size", size);
    offset = borderImageNode->valueAt("offset", offset);
    border = borderImageNode->valueAt("border", 0);
    subRect = Rect(offset, size);

    // load border margins
    top = bottom = left = right = border;
    top = borderImageNode->valueAt("border.top", top);
    bottom = borderImageNode->valueAt("border.bottom", bottom);
    left = borderImageNode->valueAt("border.left", left);
    right = borderImageNode->valueAt("border.right", right);

    // calculates border coords
    leftBorder = Rect(subRect.left(), subRect.top() + top, left, subRect.height() - top - bottom);
    rightBorder = Rect(subRect.right() - right + 1, subRect.top() + top, right, subRect.height() - top - bottom);
    topBorder = Rect(subRect.left() + left, subRect.top(), subRect.width() - right - left, top);
    bottomBorder = Rect(subRect.left() + left, subRect.bottom() - bottom + 1, subRect.width() - right - left, bottom);
    topLeftCorner = Rect(subRect.left(), subRect.top(), left, top);
    topRightCorner = Rect(subRect.right() - right + 1, subRect.top(), right, top);
    bottomLeftCorner = Rect(subRect.left(), subRect.bottom() - bottom, left, bottom);
    bottomRightCorner = Rect(subRect.right() - right + 1, subRect.bottom() - bottom + 1, right, bottom);
    center = Rect(subRect.left() + left, subRect.top() + top, subRect.width() - right - left, subRect.height() - top - bottom);

    // load individual border conf if supplied
    /*
    leftBorder = borderImageNode->valueAt("left border", leftBorder);
    rightBorder = borderImageNode->valueAt("right border", rightBorder);
    topBorder = borderImageNode->valueAt("top border", topBorder);
    bottomBorder = borderImageNode->valueAt("bottom border", bottomBorder);
    topLeftCorner = borderImageNode->valueAt("top left corner", topLeftCorner);
    topRightCorner = borderImageNode->valueAt("top right corner", topRightCorner);
    bottomLeftCorner = borderImageNode->valueAt("bottom left corner", bottomLeftCorner);
    bottomRightCorner = borderImageNode->valueAt("bottom right corner", bottomRightCorner);
    center = borderImageNode->valueAt("center", center);
    */

    return BorderImagePtr(new BorderImage(texture,
                            leftBorder,
                            rightBorder,
                            topBorder,
                            bottomBorder,
                            topLeftCorner,
                            topRightCorner,
                            bottomLeftCorner,
                            bottomRightCorner,
                            center));
}

void BorderImage::draw(const Rect& screenCoords)
{
    //TODO: borderimage drawing could be optimized by caching the render into a texture

    Rect rectCoords;
    Size centerSize = screenCoords.size() - m_bordersSize;

    g_graphics.bindTexture(m_texture);
    g_graphics.startDrawing();

    // first the center
    if(centerSize.area() > 0) {
        rectCoords = Rect(screenCoords.left() + m_leftBorderTexCoords.width(),
                        screenCoords.top() + m_topBorderTexCoords.height(),
                        centerSize);
        g_graphics.drawRepeatedTexturedRect(rectCoords, m_texture, m_centerTexCoords);
    }

    // top left corner
    rectCoords = Rect(screenCoords.topLeft(),
                      m_topLeftCornerTexCoords.size());
    g_graphics.drawTexturedRect(rectCoords, m_texture, m_topLeftCornerTexCoords);

    // top
    rectCoords = Rect(screenCoords.left() + m_topLeftCornerTexCoords.width(),
                      screenCoords.topLeft().y,
                      centerSize.width(),
                      m_topBorderTexCoords.height());
    g_graphics.drawRepeatedTexturedRect(rectCoords, m_texture, m_topBorderTexCoords);


    // top right corner
    rectCoords = Rect(screenCoords.left() + m_topLeftCornerTexCoords.width() + centerSize.width(),
                      screenCoords.top(),
                      m_topRightCornerTexCoords.size());
    g_graphics.drawTexturedRect(rectCoords, m_texture, m_topRightCornerTexCoords);

    // left
    rectCoords = Rect(screenCoords.left(),
                      screenCoords.top() + m_topLeftCornerTexCoords.height(),
                      m_leftBorderTexCoords.width(),
                      centerSize.height());
    g_graphics.drawRepeatedTexturedRect(rectCoords, m_texture, m_leftBorderTexCoords);

    // right
    rectCoords = Rect(screenCoords.left() + m_leftBorderTexCoords.width() + centerSize.width(),
                      screenCoords.top() + m_topRightCornerTexCoords.height(),
                      m_rightBorderTexCoords.width(),
                      centerSize.height());
    g_graphics.drawRepeatedTexturedRect(rectCoords, m_texture, m_rightBorderTexCoords);

    // bottom left corner
    rectCoords = Rect(screenCoords.left(),
                      screenCoords.top() + m_topBorderTexCoords.height() + centerSize.height(),
                      m_bottomLeftCornerTexCoords.size());
    g_graphics.drawTexturedRect(rectCoords, m_texture, m_bottomLeftCornerTexCoords);

    // bottom
    rectCoords = Rect(screenCoords.left() + m_bottomLeftCornerTexCoords.width(),
                      screenCoords.top() + m_topBorderTexCoords.height() + centerSize.height(),
                      centerSize.width(),
                      m_bottomBorderTexCoords.height());
    g_graphics.drawRepeatedTexturedRect(rectCoords, m_texture, m_bottomBorderTexCoords);

    // bottom right corner
    rectCoords = Rect(screenCoords.left() + m_bottomLeftCornerTexCoords.width() + centerSize.width(),
                      screenCoords.top() + m_topRightCornerTexCoords.height() + centerSize.height(),
                      m_bottomRightCornerTexCoords.size());
    g_graphics.drawTexturedRect(rectCoords, m_texture, m_bottomRightCornerTexCoords);

    g_graphics.stopDrawing();
}