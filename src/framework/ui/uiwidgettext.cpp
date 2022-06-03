/*
 * Copyright (c) 2010-2022 OTClient <https://github.com/edubart/otclient>
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

#include "uitranslator.h"
#include "uiwidget.h"
#include <framework/graphics/drawpool.h>
#include <framework/graphics/fontmanager.h>

void UIWidget::initText()
{
    m_font = g_fonts.getDefaultFont();
    m_textAlign = Fw::AlignCenter;
}

void UIWidget::updateText()
{
    if (m_textWrap && m_rect.isValid())
        m_drawText = m_font->wrapText(m_text, getWidth() - m_textOffset.x);
    else
        m_drawText = m_text;

    // update rect size
    if (!m_rect.isValid() || m_textHorizontalAutoResize || m_textVerticalAutoResize) {
        Size textBoxSize = getTextSize();
        textBoxSize += Size(m_padding.left + m_padding.right, m_padding.top + m_padding.bottom) + m_textOffset.toSize();
        Size size = getSize();
        if (size.width() <= 0 || (m_textHorizontalAutoResize && !m_textWrap))
            size.setWidth(textBoxSize.width());
        if (size.height() <= 0 || m_textVerticalAutoResize)
            size.setHeight(textBoxSize.height());
        setSize(size);
    }

    m_textMustRecache = true;
}

void UIWidget::resizeToText()
{
    auto textSize = getTextSize();
    textSize += Size(m_padding.left + m_padding.right, m_padding.top + m_padding.bottom);
    textSize += m_textOffset.toSize();

    setSize(textSize);
}

void UIWidget::parseTextStyle(const OTMLNodePtr& styleNode)
{
    for (const OTMLNodePtr& node : styleNode->children()) {
        if (node->tag() == "text")
            setText(node->value());
        else if (node->tag() == "text-align")
            setTextAlign(Fw::translateAlignment(node->value()));
        else if (node->tag() == "text-offset")
            setTextOffset(node->value<Point>());
        else if (node->tag() == "text-wrap")
            setTextWrap(node->value<bool>());
        else if (node->tag() == "text-auto-resize")
            setTextAutoResize(node->value<bool>());
        else if (node->tag() == "text-horizontal-auto-resize")
            setTextHorizontalAutoResize(node->value<bool>());
        else if (node->tag() == "text-vertical-auto-resize")
            setTextVerticalAutoResize(node->value<bool>());
        else if (node->tag() == "text-only-upper-case")
            setTextOnlyUpperCase(node->value<bool>());
        else if (node->tag() == "font")
            setFont(node->value());
    }
}

void UIWidget::drawText(const Rect& screenCoords)
{
    if (m_drawText.length() == 0 || m_color.aF() == 0.0f)
        return;

    if (screenCoords != m_textCachedScreenCoords || m_textMustRecache) {
        auto coords = Rect(screenCoords.topLeft(), screenCoords.bottomRight());
        coords.translate(m_textOffset);

        m_textMustRecache = false;
        m_textCachedScreenCoords = coords;

        m_textCoordsCache.clear();
        m_textCoordsCache = m_font->getDrawTextCoords(m_drawText, coords, m_textAlign);
    }

    for (const auto& fontRect : m_textCoordsCache)
        g_drawPool.addTexturedRect(fontRect.first, m_font->getTexture(), fontRect.second, m_color);
}

void UIWidget::onTextChange(const std::string_view text, const std::string_view oldText)
{
    g_app.repaint();
    callLuaField("onTextChange", text, oldText);
}

void UIWidget::onFontChange(const std::string_view font)
{
    callLuaField("onFontChange", font);
}

void UIWidget::setText(const std::string_view text, bool dontFireLuaCall)
{
    std::string _text{ text.data() };
    if (m_textOnlyUpperCase)
        stdext::toupper(_text);

    if (m_text == _text)
        return;

    const std::string oldText = m_text;
    m_text = _text;
    updateText();

    if (!dontFireLuaCall) {
        onTextChange(_text, oldText);
    }
}

void UIWidget::setFont(const std::string_view fontName)
{
    m_font = g_fonts.getFont(fontName);
    updateText();
    onFontChange(fontName);
}
