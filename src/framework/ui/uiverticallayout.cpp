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

#include "uiverticallayout.h"
#include "uiwidget.h"
#include <framework/otml/otml.h>

UIVerticalLayout::UIVerticalLayout(UIWidgetPtr parentWidget)
    : UILayout(parentWidget)
{
    m_alignBottom = false;
    m_fitParent = false;
    m_spacing = 0;
}

void UIVerticalLayout::applyStyle(const OTMLNodePtr& styleNode)
{
    UILayout::applyStyle(styleNode);

    for(const OTMLNodePtr& node : styleNode->children()) {
        if(node->tag() == "align-bottom")
            setAlignBottom(node->value<bool>());
        else if(node->tag() == "spacing")
            setSpacing(node->value<int>());
        else if(node->tag() == "fit-parent")
            setFitParent(node->value<bool>());
    }
}

void UIVerticalLayout::update()
{
    UIWidgetPtr parentWidget = getParentWidget();
    UIWidgetList widgets = parentWidget->getChildren();

    if(m_alignBottom)
        std::reverse(widgets.begin(), widgets.end());

    Point pos = (m_alignBottom) ? parentWidget->getRect().bottomLeft() : parentWidget->getPos();
    int prefferedHeight = 0;
    int gap;

    for(const UIWidgetPtr& widget : widgets) {
        if(!widget->isExplicitlyVisible())
            continue;

        Size size = widget->getSize();

        gap = (m_alignBottom) ? -(widget->getMarginBottom()+widget->getHeight()) : widget->getMarginTop();
        pos.y += gap;
        prefferedHeight += gap;

        if(widget->isSizeFixed()) {
            // center it
            pos.x = parentWidget->getX() + (parentWidget->getWidth() - (widget->getMarginLeft() + widget->getWidth() + widget->getMarginRight()))/2;
            pos.x = std::max(pos.x, parentWidget->getX());
        } else {
            // expand width
            size.setWidth(parentWidget->getWidth() - (widget->getMarginLeft() + widget->getMarginRight()));
            pos.x = parentWidget->getX() + (parentWidget->getWidth() - size.width())/2;
        }

        widget->setRect(Rect(pos, size));

        gap = (m_alignBottom) ? -widget->getMarginTop() : (widget->getHeight() + widget->getMarginBottom());
        gap += m_spacing;
        pos.y += gap;
        prefferedHeight += gap;
    }

    prefferedHeight -= m_spacing;

    if(m_fitParent && prefferedHeight != parentWidget->getHeight())
        parentWidget->setHeight(prefferedHeight);
}

void UIVerticalLayout::addWidget(const UIWidgetPtr& widget)
{
    update();
}

void UIVerticalLayout::removeWidget(const UIWidgetPtr& widget)
{
    update();
}

void UIVerticalLayout::setAlignBottom(bool aliginBottom)
{
    m_alignBottom = aliginBottom;
    update();
}

void UIVerticalLayout::setSpacing(int spacing)
{
    m_spacing = spacing;
    update();
}

void UIVerticalLayout::setFitParent(bool fitParent)
{
    m_fitParent = fitParent;
    update();
}
