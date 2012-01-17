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

#include "uiwidget.h"
#include "uitranslator.h"
#include <framework/graphics/fontmanager.h>
#include <framework/graphics/painter.h>
#include <framework/graphics/framebuffer.h>

void UIWidget::initText()
{
    m_font = g_fonts.getDefaultFont();
    m_textAlign = Fw::AlignCenter;
}

void UIWidget::parseTextStyle(const OTMLNodePtr& styleNode)
{
    for(const OTMLNodePtr& node : styleNode->children()) {
        if(node->tag() == "text")
            setText(node->value());
        else if(node->tag() == "text-align")
            setTextAlign(Fw::translateAlignment(node->value()));
        else if(node->tag() == "text-offset")
            setTextOffset(node->value<Point>());
        else if(node->tag() == "font")
            setFont(node->value());
    }
}

void UIWidget::drawText(const Rect& screenCoords)
{
    if(m_text.length() == 0 || m_color.a() == 0)
        return;

    Size boxSize = screenCoords.size();
    if(boxSize != m_textCachedBoxSize || m_textMustRecache) {
        if(!m_textFramebuffer)
            m_textFramebuffer = FrameBufferPtr(new FrameBuffer(boxSize));
        else
            m_textFramebuffer->resize(boxSize);

        m_textFramebuffer->bind();
        Rect virtualTextRect(0, 0, boxSize);
        virtualTextRect.translate(m_textOffset);
        g_painter.setCompositionMode(Painter::CompositionMode_Add);
        m_font->renderText(m_text, virtualTextRect, m_textAlign, Fw::white);
        g_painter.resetCompositionMode();
        m_textFramebuffer->release();

        m_textMustRecache = false;
        m_textCachedBoxSize = boxSize;
    }

    g_painter.setColor(m_color);
    m_textFramebuffer->draw(screenCoords);
}

void UIWidget::onTextChange(const std::string& text)
{
    callLuaField("onTextChange", text);
}

void UIWidget::onFontChange(const std::string& font)
{
    callLuaField("onFontChange", font);
}

void UIWidget::setText(const std::string& text)
{
    if(m_text == text)
        return;

    m_text = text;

    // update rect size
    if(!m_rect.isValid()) {
        Size textSize = m_font->calculateTextRectSize(m_text);
        Size newSize = getSize();
        if(newSize.width() <= 0)
            newSize.setWidth(textSize.width());
        if(newSize.height() <= 0)
            newSize.setHeight(textSize.height());
        setSize(newSize);
    }

    onTextChange(text);
    m_textMustRecache = true;
}

void UIWidget::setFont(const std::string& fontName)
{
    m_font = g_fonts.getFont(fontName);
    onFontChange(fontName);
    m_textMustRecache = true;
}
