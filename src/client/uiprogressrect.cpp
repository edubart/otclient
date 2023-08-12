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

#include "uiprogressrect.h"
#include <framework/otml/otml.h>
#include <framework/graphics/graphics.h>
#include <framework/graphics/fontmanager.h>

UIProgressRect::UIProgressRect()
{
    m_percent = 0;
}

void UIProgressRect::drawSelf(Fw::DrawPane drawPane)
{
    if((drawPane & Fw::ForegroundPane) == 0)
        return;

    g_painter->setColor(m_backgroundColor);

    // todo: check +1 to right/bottom
    // todo: add smooth
    Rect drawRect = getPaddingRect();

    // 0% - 12.5% (12.5)
    // triangle from top center, to top right (var x)
    if(m_percent < 12.5f) {
        Point var = Point(std::max<int>(m_percent - 0.f, 0.f) * (drawRect.right() - drawRect.horizontalCenter()) / 12.5f, 0);
        g_painter->drawFilledTriangle(drawRect.center(), drawRect.topRight() + Point(1,0), drawRect.topCenter() + var);
    }

    // 12.5% - 37.5% (25)
    // triangle from top right to bottom right (var y)
    if(m_percent < 37.5f) {
        Point var = Point(0, std::max<int>(m_percent - 12.5f, 0.f) * (drawRect.bottom() - drawRect.top()) / 25.f);
        g_painter->drawFilledTriangle(drawRect.center(), drawRect.bottomRight() + Point(1,1), drawRect.topRight() + var + Point(1,0));
    }

    // 37.5% - 62.5% (25)
    // triangle from bottom right to bottom left (var x)
    if(m_percent < 62.5f) {
        Point var = Point(std::max<int>(m_percent - 37.5f, 0.f) * (drawRect.right() - drawRect.left()) / 25.f, 0);
        g_painter->drawFilledTriangle(drawRect.center(), drawRect.bottomLeft() + Point(0,1), drawRect.bottomRight() - var + Point(1,1));
    }

    // 62.5% - 87.5% (25)
    // triangle from bottom left to top left
    if(m_percent < 87.5f) {
        Point var = Point(0, std::max<int>(m_percent - 62.5f, 0.f) * (drawRect.bottom() - drawRect.top()) / 25.f);
        g_painter->drawFilledTriangle(drawRect.center(), drawRect.topLeft(), drawRect.bottomLeft() - var + Point(0,1));
    }

    // 87.5% - 100% (12.5)
    // triangle from top left to top center
    if(m_percent < 100) {
        Point var = Point(std::max<int>(m_percent - 87.5f, 0.f) * (drawRect.horizontalCenter() - drawRect.left()) / 12.5f, 0);
        g_painter->drawFilledTriangle(drawRect.center(), drawRect.topCenter(), drawRect.topLeft() + var);
    }

    drawImage(m_rect);
    drawBorder(m_rect);
    drawIcon(m_rect);
    drawText(m_rect);
}

void UIProgressRect::setPercent(float percent)
{
    m_percent = stdext::clamp<float>(percent, 0.f, 100.f);
}

void UIProgressRect::onStyleApply(const std::string& styleName, const OTMLNodePtr& styleNode)
{
    UIWidget::onStyleApply(styleName, styleNode);

    for(const OTMLNodePtr& node : styleNode->children()) {
        if(node->tag() == "percent")
            setPercent(node->value<float>());
    }
}
