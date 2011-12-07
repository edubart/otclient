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

#include "uibutton.h"
#include <framework/graphics/borderimage.h>
#include <framework/graphics/font.h>
#include <framework/otml/otmlnode.h>
#include <framework/luascript/luainterface.h>
#include <framework/graphics/graphics.h>
#include <framework/graphics/texture.h>
#include <framework/graphics/texturemanager.h>

UIButton::UIButton()
{
    m_focusable = false;
}

void UIButton::render()
{
    UIWidget::render();

    if(m_icon) {
        Rect iconRect;
        iconRect.resize(m_icon->getSize());
        iconRect.moveCenter(m_rect.center());
        g_painter.drawTexturedRect(iconRect, m_icon);
    }

    Rect textRect = m_rect;
    textRect.translate(m_textOffset);
    m_font->renderText(m_text, textRect, Fw::AlignCenter, m_foregroundColor);
}

void UIButton::onStyleApply(const OTMLNodePtr& styleNode)
{
    UIWidget::onStyleApply(styleNode);

    for(OTMLNodePtr node : styleNode->children()) {
        if(node->tag() == "text-offset")
            m_textOffset = node->value<Point>();
        else if(node->tag() == "text")
            m_text = node->value();
        else if(node->tag() == "icon")
            m_icon = g_textures.getTexture(node->value());
    }
}

void UIButton::onMouseRelease(const Point& mousePos, Fw::MouseButton button)
{
    if(isPressed() && getRect().contains(mousePos)) {
        callLuaField("onClick");
    }
}
