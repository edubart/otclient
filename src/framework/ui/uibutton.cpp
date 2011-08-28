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

void UIButton::setup()
{
    UIWidget::setup();
    setFocusable(false);

    // by default, all callbacks call lua fields
    m_onClick = [this]() { this->callLuaField("onClick"); };
}

void UIButton::render()
{
    UIWidget::render();
    Rect textRect = m_rect;
    textRect.translate(m_textTranslate);
    m_font->renderText(m_text, textRect, Fw::AlignCenter, m_foregroundColor);
}

void UIButton::onStyleApply(const OTMLNodePtr& styleNode)
{
    UIWidget::onStyleApply(styleNode);

    for(OTMLNodePtr node : styleNode->children()) {
        if(node->tag() == "text-translate") {
            m_textTranslate = node->value<Point>();
        } else if(node->tag() == "text") {
            m_text = node->value();
        } else if(node->tag() == "onClick") {
            g_lua.loadFunction(node->value(), "@" + node->source() + "[" + node->tag() + "]");
            luaSetField(node->tag());
        }
    }
}

bool UIButton::onMouseRelease(const Point& mousePos, Fw::MouseButton button)
{
    if(isPressed()) {
        if(m_onClick && getRect().contains(mousePos))
            m_onClick();
        return true;
    }
    return false;
}
