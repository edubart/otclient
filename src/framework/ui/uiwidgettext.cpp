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
    m_textCoordsBuffer.enableHardwareCaching();
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
    if(m_text.length() == 0 || m_color.aF() == 0.0f)
        return;

    if(screenCoords != m_textCachedScreenCoords || m_textMustRecache) {
        m_textMustRecache = false;
        m_textCachedScreenCoords = screenCoords;

        m_textCoordsBuffer.clear();

        m_font->calculateDrawTextCoords(m_textCoordsBuffer, m_text, screenCoords.translated(m_textOffset), m_textAlign);
    }

    g_painter.setColor(m_color);
    g_painter.drawTextureCoords(m_textCoordsBuffer, m_font->getTexture());
}

void UIWidget::onTextChange(const std::string& text, const std::string& oldText)
{
    callLuaField("onTextChange", text, oldText);
}

void UIWidget::onFontChange(const std::string& font)
{
    callLuaField("onFontChange", font);
}

void UIWidget::wrapText()
{
    setText(m_font->wrapText(m_text, getWidth() - m_textOffset.x));
}

void UIWidget::setText(const std::string& text)
{
    if(m_text == text)
        return;

    std::string oldText = m_text;
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

    onTextChange(text, oldText);
    m_textMustRecache = true;
}

void UIWidget::setFont(const std::string& fontName)
{
    m_font = g_fonts.getFont(fontName);
    onFontChange(fontName);
    m_textMustRecache = true;
}
