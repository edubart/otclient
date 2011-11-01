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

#include "uilabel.h"
#include <framework/graphics/font.h>
#include <framework/otml/otmlnode.h>

void UILabel::setup()
{
    UIWidget::setup();
    setFocusable(false);
    setPhantom(true);
    setAlign(Fw::AlignLeft);
}

void UILabel::render()
{
    UIWidget::render();
    Rect textRect = m_rect;
    textRect.setTopLeft(textRect.topLeft() + m_offset);
    m_font->renderText(m_text, textRect, m_align, m_foregroundColor);
}

void UILabel::setText(const std::string& text)
{
    m_text = text;

    // auto resize
    if(!m_fixedSize && !m_rect.isValid()) {
        Size textSize = m_font->calculateTextRectSize(m_text);
        if(m_rect.width() <= 0)
            m_rect.setWidth(textSize.width());
        if(m_rect.height() <= 0)
            m_rect.setHeight(textSize.height());
    }
}

void UILabel::resizeToText()
{
    resize(m_font->calculateTextRectSize(m_text));
}

void UILabel::onStyleApply(const OTMLNodePtr& styleNode)
{
    UIWidget::onStyleApply(styleNode);

    for(const OTMLNodePtr& node : styleNode->children()) {
        if(node->tag() == "text")
            setText(node->value());
        else if(node->tag() == "align")
            setAlign(Fw::translateAlignment(node->value()));
        else if(node->tag() == "offset") {
            setOffset(node->value<Point>());
        }
    }
}
