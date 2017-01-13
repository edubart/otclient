/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
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

bool UIHorizontalLayout::internalUpdate()
{
    UIWidgetPtr parentWidget = getParentWidget();
    if(!parentWidget)
        return false;
    UIWidgetList widgets = parentWidget->getChildren();

    if(m_alignRight)
        std::reverse(widgets.begin(), widgets.end());

    bool changed = false;
    Rect paddingRect = parentWidget->getPaddingRect();
    Point pos = (m_alignRight) ? paddingRect.topRight() : paddingRect.topLeft();
    int preferredWidth = 0;
    int gap;

    for(const UIWidgetPtr& widget : widgets) {
        if(!widget->isExplicitlyVisible())
            continue;

        Size size = widget->getSize();

        gap = (m_alignRight) ? -(widget->getMarginRight()+widget->getWidth()) : widget->getMarginLeft();
        pos.x += gap;
        preferredWidth += gap;

        if(widget->isFixedSize()) {
            if(widget->getTextAlign() & Fw::AlignTop) {
                pos.y = paddingRect.top() + widget->getMarginTop();
            } else if(widget->getTextAlign() & Fw::AlignBottom) {
                pos.y = paddingRect.bottom() - widget->getHeight() - widget->getMarginBottom();
                pos.y = std::max<int>(pos.y, paddingRect.top());
            } else { // center it
                pos.y = paddingRect.top() + (paddingRect.height() - (widget->getMarginTop() + widget->getHeight() + widget->getMarginBottom()))/2;
                pos.y = std::max<int>(pos.y, paddingRect.top());
            }
        } else {
            // expand height
            size.setHeight(paddingRect.height() - (widget->getMarginTop() + widget->getMarginBottom()));
            pos.y = paddingRect.top() + (paddingRect.height() - size.height())/2;
        }

        if(widget->setRect(Rect(pos - parentWidget->getVirtualOffset(), size)))
            changed = true;

        gap = (m_alignRight) ? -widget->getMarginLeft() : (widget->getWidth() + widget->getMarginRight());
        gap += m_spacing;
        pos.x += gap;
        preferredWidth += gap;
    }

    preferredWidth -= m_spacing;
    preferredWidth += parentWidget->getPaddingLeft() + parentWidget->getPaddingRight();

    if(m_fitChildren && preferredWidth != parentWidget->getWidth()) {
        // must set the preferred width later
        g_dispatcher.addEvent([=] {
            parentWidget->setWidth(preferredWidth);
        });
    }

    return changed;
}
