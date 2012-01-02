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

#include "uiprogressbar.h"
#include <framework/graphics/graphics.h>
#include <framework/otml/otmlnode.h>

UIProgressBar::UIProgressBar()
{
    m_phantom = false;
    m_focusable = false;
    m_percent = 0;
}

void UIProgressBar::render()
{
    UIWidget::render();

    g_painter.setColor(m_foregroundColor);
    g_painter.drawBoundingRect(m_rect, 1);

    Rect fillRect = m_rect.expanded(-1);
    fillRect.setWidth(fillRect.width() * m_percent / 100.0);

    g_painter.setColor(m_backgroundColor);
    g_painter.drawFilledRect(fillRect);
}

void UIProgressBar::setPercent(double percent)
{
    if(percent == NAN)
        percent = 0;
    m_percent = std::min(std::max(percent, 0.0), 100.0);
}
