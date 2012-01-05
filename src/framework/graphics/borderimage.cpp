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
                             const Rect& center)
{
    m_texture = texture;
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
    Rect clipRect;
    int top, bottom, left, right, border;
    Size size;
    Point offset;

    // load texture
    std::string source = borderImageNode->at("source")->value();
    TexturePtr texture = g_textures.getTexture(source);

    // load basic border confs
    border = borderImageNode->valueAt("border", 0);
    clipRect = borderImageNode->valueAt("clip", Rect(0, 0, texture->getSize()));

    // load border margins
    top = bottom = left = right = border;
    top = borderImageNode->valueAt("border.top", top);
    bottom = borderImageNode->valueAt("border.bottom", bottom);
    left = borderImageNode->valueAt("border.left", left);
    right = borderImageNode->valueAt("border.right", right);

    // calculates border coords
    leftBorder = Rect(clipRect.left(), clipRect.top() + top, left, clipRect.height() - top - bottom);
    rightBorder = Rect(clipRect.right() - right + 1, clipRect.top() + top, right, clipRect.height() - top - bottom);
    topBorder = Rect(clipRect.left() + left, clipRect.top(), clipRect.width() - right - left, top);
    bottomBorder = Rect(clipRect.left() + left, clipRect.bottom() - bottom + 1, clipRect.width() - right - left, bottom);
    topLeftCorner = Rect(clipRect.left(), clipRect.top(), left, top);
    topRightCorner = Rect(clipRect.right() - right + 1, clipRect.top(), right, top);
    bottomLeftCorner = Rect(clipRect.left(), clipRect.bottom() - bottom + 1, left, bottom);
    bottomRightCorner = Rect(clipRect.right() - right + 1, clipRect.bottom() - bottom + 1, right, bottom);
    center = Rect(clipRect.left() + left, clipRect.top() + top, clipRect.width() - right - left, clipRect.height() - top - bottom);

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
    if(screenCoords != m_cachedScreenCoords) {
        m_cachedScreenCoords = screenCoords;
        m_coordsBuffer.clear();

        Rect rectCoords;
        Size centerSize = screenCoords.size() - m_bordersSize;

        // first the center
        if(centerSize.area() > 0) {
            rectCoords = Rect(screenCoords.left() + m_leftBorderTexCoords.width(),
                            screenCoords.top() + m_topBorderTexCoords.height(),
                            centerSize);
            m_coordsBuffer.addRepeatedRects(rectCoords, m_centerTexCoords);
        }

        // top left corner
        rectCoords = Rect(screenCoords.topLeft(),
                        m_topLeftCornerTexCoords.size());
        m_coordsBuffer.addRepeatedRects(rectCoords, m_topLeftCornerTexCoords);

        // top
        rectCoords = Rect(screenCoords.left() + m_topLeftCornerTexCoords.width(),
                        screenCoords.topLeft().y,
                        centerSize.width(),
                        m_topBorderTexCoords.height());
        m_coordsBuffer.addRepeatedRects(rectCoords, m_topBorderTexCoords);


        // top right corner
        rectCoords = Rect(screenCoords.left() + m_topLeftCornerTexCoords.width() + centerSize.width(),
                        screenCoords.top(),
                        m_topRightCornerTexCoords.size());
        m_coordsBuffer.addRepeatedRects(rectCoords, m_topRightCornerTexCoords);

        // left
        rectCoords = Rect(screenCoords.left(),
                        screenCoords.top() + m_topLeftCornerTexCoords.height(),
                        m_leftBorderTexCoords.width(),
                        centerSize.height());
        m_coordsBuffer.addRepeatedRects(rectCoords, m_leftBorderTexCoords);

        // right
        rectCoords = Rect(screenCoords.left() + m_leftBorderTexCoords.width() + centerSize.width(),
                        screenCoords.top() + m_topRightCornerTexCoords.height(),
                        m_rightBorderTexCoords.width(),
                        centerSize.height());
        m_coordsBuffer.addRepeatedRects(rectCoords, m_rightBorderTexCoords);

        // bottom left corner
        rectCoords = Rect(screenCoords.left(),
                        screenCoords.top() + m_topLeftCornerTexCoords.height() + centerSize.height(),
                        m_bottomLeftCornerTexCoords.size());
        m_coordsBuffer.addRepeatedRects(rectCoords, m_bottomLeftCornerTexCoords);

        // bottom
        rectCoords = Rect(screenCoords.left() + m_bottomLeftCornerTexCoords.width(),
                        screenCoords.top() + m_topBorderTexCoords.height() + centerSize.height(),
                        centerSize.width(),
                        m_bottomBorderTexCoords.height());
        m_coordsBuffer.addRepeatedRects(rectCoords, m_bottomBorderTexCoords);

        // bottom right corner
        rectCoords = Rect(screenCoords.left() + m_bottomLeftCornerTexCoords.width() + centerSize.width(),
                        screenCoords.top() + m_topRightCornerTexCoords.height() + centerSize.height(),
                        m_bottomRightCornerTexCoords.size());
        m_coordsBuffer.addRepeatedRects(rectCoords, m_bottomRightCornerTexCoords);
    }
    g_painter.drawTextureCoords(m_coordsBuffer, m_texture);
}