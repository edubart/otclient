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

#include "uicheckbox.h"
#include <framework/otml/otmlnode.h>
#include <framework/graphics/image.h>
#include <framework/graphics/font.h>
#include <framework/graphics/graphics.h>
#include <framework/core/eventdispatcher.h>

void UICheckBox::render()
{
    if(m_image) {
        Rect boxRect;
        boxRect.setSize(m_boxSize);
        boxRect.moveLeft(m_rect.left());
        boxRect.moveVerticalCenter(m_rect.verticalCenter());
        g_graphics.bindColor(m_backgroundColor);
        m_image->draw(boxRect);
    }

    if(m_text.length()) {
        Rect textRect(m_rect);
        textRect.setTopLeft(textRect.topLeft() + m_textOffset);
        m_font->renderText(m_text, textRect, Fw::AlignLeft, m_foregroundColor);
    }
}

void UICheckBox::onMouseRelease(const Point& mousePos, Fw::MouseButton button)
{
    if(isPressed() && getRect().contains(mousePos))
        setChecked(!isChecked());
}

void UICheckBox::onStyleApply(const OTMLNodePtr& styleNode)
{
    UIWidget::onStyleApply(styleNode);

    for(OTMLNodePtr node : styleNode->children()) {
        if(node->tag() == "text-offset")
            m_textOffset = node->value<Point>();
        else if(node->tag() == "text")
            m_text = node->value();
        else if(node->tag() == "box-size")
            m_boxSize = node->value<Size>();
        else if(node->tag() == "checked") {
            // must be scheduled because setChecked can change the style again
            g_dispatcher.addEvent(std::bind(&UICheckBox::setChecked, asUICheckBox(), node->value<bool>()));
        }
    }
}

bool UICheckBox::isChecked()
{
    return hasState(Fw::CheckedState);
}

void UICheckBox::setChecked(bool checked)
{
    if(setState(Fw::CheckedState, checked))
        callLuaField("onCheckChange", checked);
}
