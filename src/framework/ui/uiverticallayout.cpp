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

#include "uiverticallayout.h"
#include "uiwidget.h"
#include <framework/otml/otml.h>

UIVerticalLayout::UIVerticalLayout(UIWidgetPtr parentWidget)
    : UILayout(parentWidget)
{
    m_alignBottom = false;
    m_padding = 0;
}

void UIVerticalLayout::applyStyle(const OTMLNodePtr& styleNode)
{
    UILayout::applyStyle(styleNode);

    for(const OTMLNodePtr& node : styleNode->children()) {
        if(node->tag() == "align bottom")
            m_alignBottom = node->value<bool>();
        else if(node->tag() == "padding")
            m_padding = node->value<int>();
    }
}

void UIVerticalLayout::update()
{
    UIWidgetPtr parentWidget = getParentWidget();
    UIWidgetList widgets = parentWidget->getChildren();

    if(m_alignBottom)
        std::reverse(widgets.begin(), widgets.end());

    Point pos = (m_alignBottom) ? parentWidget->getRect().bottomLeft() : parentWidget->getPosition();

    for(const UIWidgetPtr& widget : widgets) {
        if(!widget->isExplicitlyVisible())
            continue;

        Size size = widget->getSize();
        pos.y += (m_alignBottom) ? -(widget->getMarginBottom()+widget->getHeight()) : widget->getMarginTop();
        if(widget->isSizeFixed()) {
            pos.x = parentWidget->getX() + (parentWidget->getWidth() - (widget->getMarginLeft() + widget->getWidth() + widget->getMarginRight()))/2;
            pos.x = std::max(pos.x, parentWidget->getX());
        } else {
            size.setWidth(parentWidget->getWidth() - (widget->getMarginLeft() + widget->getMarginRight()));
            pos.x = std::max(pos.x, parentWidget->getX() + (parentWidget->getWidth() - size.width())/2);
        }
        widget->setRect(Rect(pos, size));
        pos.y += (m_alignBottom) ? -widget->getMarginTop() : (widget->getHeight() + widget->getMarginBottom());
        pos.y += m_padding;
    }
}

void UIVerticalLayout::addWidget(const UIWidgetPtr& widget)
{
    update();
}

void UIVerticalLayout::removeWidget(const UIWidgetPtr& widget)
{
    update();
}
