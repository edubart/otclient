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

#include "uihorizontallayout.h"
#include "uiwidget.h"
#include <framework/core/eventdispatcher.h>


void UIHorizontalLayout::applyStyle(const OTMLNodePtr& styleNode)
{
    UIBoxLayout::applyStyle(styleNode);

    for(const OTMLNodePtr& node : styleNode->children()) {
        if(node->tag() == "align-right")
            setAlignRight(node->value<bool>());
    }
}

void UIHorizontalLayout::internalUpdate()
{
    UIWidgetPtr parentWidget = getParentWidget();
    if(!parentWidget)
        return;
    UIWidgetList widgets = parentWidget->getChildren();

    if(m_alignRight)
        std::reverse(widgets.begin(), widgets.end());

    Rect childrenRect = parentWidget->getChildrenRect();
    Point pos = (m_alignRight) ? childrenRect.topRight() : childrenRect.topLeft();
    int prefferedWidth = 0;
    int gap;

    for(const UIWidgetPtr& widget : widgets) {
        if(!widget->isExplicitlyVisible())
            continue;

        Size size = widget->getSize();

        gap = (m_alignRight) ? -(widget->getMarginRight()+widget->getWidth()) : widget->getMarginLeft();
        pos.x += gap;
        prefferedWidth += gap;

        if(widget->isFixedSize()) {
            // center it
            pos.y = childrenRect.top() + (childrenRect.height() - (widget->getMarginTop() + widget->getHeight() + widget->getMarginBottom()))/2;
            pos.y = std::max(pos.y, parentWidget->getY());
        } else {
            // expand height
            size.setHeight(childrenRect.height() - (widget->getMarginTop() + widget->getMarginBottom()));
            pos.y = childrenRect.top() + (childrenRect.height() - size.height())/2;
        }

        widget->setRect(Rect(pos, size));

        gap = (m_alignRight) ? -widget->getMarginLeft() : (widget->getWidth() + widget->getMarginRight());
        gap += m_spacing;
        pos.x += gap;
        prefferedWidth += gap;
    }

    prefferedWidth -= m_spacing;
    prefferedWidth += parentWidget->getPaddingLeft() + parentWidget->getPaddingRight();

    if(m_fitChildren && prefferedWidth != parentWidget->getWidth()) {
        // must set the preffered width later
        g_dispatcher.addEvent([=] {
            parentWidget->setWidth(prefferedWidth);
        });
    }
}
