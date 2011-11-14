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

#include "uiprogressbar.h"
#include <framework/graphics/graphics.h>
#include <framework/otml/otmlnode.h>

void UIProgressBar::setup()
{
    UIWidget::setup();
    setPhantom(true);
    setFocusable(false);
    m_percent = 0;
}

void UIProgressBar::render()
{
    UIWidget::render();

    g_graphics.bindColor(m_foregroundColor);
    g_graphics.drawBoundingRect(m_rect, 1);

    Rect fillRect = m_rect.expanded(-1);
    fillRect.setWidth(fillRect.width() * m_percent / 100.0);

    g_graphics.bindColor(m_backgroundColor);
    g_graphics.drawFilledRect(fillRect);
}

void UIProgressBar::setPercent(double percent)
{
    assert(percent >= 0 && percent <= 100);
    m_percent = percent;
}
