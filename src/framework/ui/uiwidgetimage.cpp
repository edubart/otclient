/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
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

#include "uiwidget.h"
#include <framework/graphics/painter.h>
#include <framework/graphics/texture.h>
#include <framework/graphics/texturemanager.h>
#include <framework/graphics/graphics.h>

void UIWidget::initImage()
{
    m_imageCoordsBuffer.enableHardwareCaching();
}

void UIWidget::parseImageStyle(const OTMLNodePtr& styleNode)
{
    for(const OTMLNodePtr& node : styleNode->children()) {
        if(node->tag() == "image-source")
            setImageSource(stdext::resolve_path(node->value(), node->source()));
        else if(node->tag() == "image-offset-x")
            setImageOffsetX(node->value<int>());
        else if(node->tag() == "image-offset-y")
            setImageOffsetY(node->value<int>());
        else if(node->tag() == "image-offset")
            setImageOffset(node->value<Point>());
        else if(node->tag() == "image-width")
            setImageWidth(node->value<int>());
        else if(node->tag() == "image-height")
            setImageHeight(node->value<int>());
        else if(node->tag() == "image-size")
            setImageSize(node->value<Size>());
        else if(node->tag() == "image-rect")
            setImageRect(node->value<Rect>());
        else if(node->tag() == "image-clip")
            setImageClip(node->value<Rect>());
        else if(node->tag() == "image-fixed-ratio")
            setImageFixedRatio(node->value<bool>());
        else if(node->tag() == "image-repeated")
            setImageRepeated(node->value<bool>());
        else if(node->tag() == "image-smooth")
            setImageSmooth(node->value<bool>());
        else if(node->tag() == "image-color")
            setImageColor(node->value<Color>());
        else if(node->tag() == "image-border-top")
            setImageBorderTop(node->value<int>());
        else if(node->tag() == "image-border-right")
            setImageBorderRight(node->value<int>());
        else if(node->tag() == "image-border-bottom")
            setImageBorderBottom(node->value<int>());
        else if(node->tag() == "image-border-left")
            setImageBorderLeft(node->value<int>());
        else if(node->tag() == "image-border")
            setImageBorder(node->value<int>());
        else if(node->tag() == "image-auto-resize")
            setImageAutoResize(node->value<bool>());
    }
}

void UIWidget::drawImage(const Rect& screenCoords)
{
    if(!m_imageTexture || !screenCoords.isValid())
        return;

    // cache vertex buffers
    if(m_imageCachedScreenCoords != screenCoords || m_imageMustRecache) {
        m_imageCoordsBuffer.clear();
        m_imageCachedScreenCoords = screenCoords;
        m_imageMustRecache = false;

        Rect drawRect = screenCoords;
        drawRect.translate(m_imageRect.topLeft());
        if(m_imageRect.isValid())
            drawRect.resize(m_imageRect.size());

        Rect clipRect = m_imageClipRect.isValid() ? m_imageClipRect : Rect(0, 0, m_imageTexture->getSize());

        if(!m_imageBordered) {
            if(m_imageFixedRatio) {
                Size textureSize = m_imageTexture->getSize();

                Size textureClipSize = drawRect.size();
                textureClipSize.scale(textureSize, Fw::KeepAspectRatio);

                Point texCoordsOffset;
                if(textureSize.height() > textureClipSize.height())
                    texCoordsOffset.y = (textureSize.height() - textureClipSize.height())/2;
                else if(textureSize.width() > textureClipSize.width())
                    texCoordsOffset.x = (textureSize.width() - textureClipSize.width())/2;

                Rect textureClipRect(texCoordsOffset, textureClipSize);

                m_imageCoordsBuffer.addRect(drawRect, textureClipRect);
            } else {
                if(m_imageRepeated)
                    m_imageCoordsBuffer.addRepeatedRects(drawRect, clipRect);
                else
                    m_imageCoordsBuffer.addRect(drawRect, clipRect);
            }
        } else {
            int top = m_imageBorder.top;
            int bottom = m_imageBorder.bottom;
            int left =  m_imageBorder.left;
            int right  = m_imageBorder.right;

            // calculates border coords
            const Rect clip = clipRect;
            Rect leftBorder(clip.left(), clip.top() + top, left, clip.height() - top - bottom);
            Rect rightBorder(clip.right() - right + 1, clip.top() + top, right, clip.height() - top - bottom);
            Rect topBorder(clip.left() + left, clip.top(), clip.width() - right - left, top);
            Rect bottomBorder(clip.left() + left, clip.bottom() - bottom + 1, clip.width() - right - left, bottom);
            Rect topLeftCorner(clip.left(), clip.top(), left, top);
            Rect topRightCorner(clip.right() - right + 1, clip.top(), right, top);
            Rect bottomLeftCorner(clip.left(), clip.bottom() - bottom + 1, left, bottom);
            Rect bottomRightCorner(clip.right() - right + 1, clip.bottom() - bottom + 1, right, bottom);
            Rect center(clip.left() + left, clip.top() + top, clip.width() - right - left, clip.height() - top - bottom);
            Size bordersSize(leftBorder.width() + rightBorder.width(), topBorder.height() + bottomBorder.height());
            Size centerSize = drawRect.size() - bordersSize;
            Rect rectCoords;

            // first the center
            if(centerSize.area() > 0) {
                rectCoords = Rect(drawRect.left() + leftBorder.width(), drawRect.top() + topBorder.height(), centerSize);
                m_imageCoordsBuffer.addRepeatedRects(rectCoords, center);
            }
            // top left corner
            rectCoords = Rect(drawRect.topLeft(), topLeftCorner.size());
            m_imageCoordsBuffer.addRepeatedRects(rectCoords, topLeftCorner);
            // top
            rectCoords = Rect(drawRect.left() + topLeftCorner.width(), drawRect.topLeft().y, centerSize.width(), topBorder.height());
            m_imageCoordsBuffer.addRepeatedRects(rectCoords, topBorder);
            // top right corner
            rectCoords = Rect(drawRect.left() + topLeftCorner.width() + centerSize.width(), drawRect.top(), topRightCorner.size());
            m_imageCoordsBuffer.addRepeatedRects(rectCoords, topRightCorner);
            // left
            rectCoords = Rect(drawRect.left(), drawRect.top() + topLeftCorner.height(), leftBorder.width(), centerSize.height());
            m_imageCoordsBuffer.addRepeatedRects(rectCoords, leftBorder);
            // right
            rectCoords = Rect(drawRect.left() + leftBorder.width() + centerSize.width(), drawRect.top() + topRightCorner.height(), rightBorder.width(), centerSize.height());
            m_imageCoordsBuffer.addRepeatedRects(rectCoords, rightBorder);
            // bottom left corner
            rectCoords = Rect(drawRect.left(), drawRect.top() + topLeftCorner.height() + centerSize.height(), bottomLeftCorner.size());
            m_imageCoordsBuffer.addRepeatedRects(rectCoords, bottomLeftCorner);
            // bottom
            rectCoords = Rect(drawRect.left() + bottomLeftCorner.width(), drawRect.top() + topBorder.height() + centerSize.height(), centerSize.width(), bottomBorder.height());
            m_imageCoordsBuffer.addRepeatedRects(rectCoords, bottomBorder);
            // bottom right corner
            rectCoords = Rect(drawRect.left() + bottomLeftCorner.width() + centerSize.width(), drawRect.top() + topRightCorner.height() + centerSize.height(), bottomRightCorner.size());
            m_imageCoordsBuffer.addRepeatedRects(rectCoords, bottomRightCorner);
        }
    }

    // smooth is now enabled by default for all textures
    //m_imageTexture->setSmooth(m_imageSmooth);

    g_painter->setColor(m_imageColor);
    g_painter->drawTextureCoords(m_imageCoordsBuffer, m_imageTexture);
}

void UIWidget::setImageSource(const std::string& source)
{
    if(source.empty())
        m_imageTexture = nullptr;
    else
        m_imageTexture = g_textures.getTexture(source);

    if(m_imageTexture && (!m_rect.isValid() || m_imageAutoResize)) {
        Size size = getSize();
        Size imageSize = m_imageTexture->getSize();
        if(size.width() <= 0 || m_imageAutoResize)
            size.setWidth(imageSize.width());
        if(size.height() <= 0 || m_imageAutoResize)
            size.setHeight(imageSize.height());
        setSize(size);
    }

    m_imageMustRecache = true;
}
