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

#include "uigridlayout.h"
#include "uiwidget.h"

#include <framework/core/eventdispatcher.h>

UIGridLayout::UIGridLayout(UIWidgetPtr parentWidget): UILayout(parentWidget)
{
    m_cellSize = Size(16,16);
    m_cellSpacing = 0;
    m_numColumns = 1;
    m_numLines = 0;
}

void UIGridLayout::applyStyle(const OTMLNodePtr& styleNode)
{
    UILayout::applyStyle(styleNode);

    for(const OTMLNodePtr& node : styleNode->children()) {
        if(node->tag() == "cell-size")
            setCellSize(node->value<Size>());
        else if(node->tag() == "cell-width")
            setCellWidth(node->value<int>());
        else if(node->tag() == "cell-height")
            setCellHeight(node->value<int>());
        else if(node->tag() == "cell-spacing")
            setCellSpacing(node->value<int>());
        else if(node->tag() == "num-columns")
            setNumColumns(node->value<int>());
        else if(node->tag() == "num-lines")
            setNumLines(node->value<int>());
        else if(node->tag() == "fit-children")
            setFitChildren(node->value<bool>());
        else if(node->tag() == "auto-spacing")
            setAutoSpacing(node->value<bool>());
        else if(node->tag() == "flow")
            setFlow(node->value<bool>());
    }
}

void UIGridLayout::removeWidget(const UIWidgetPtr& widget)
{
    update();
}

void UIGridLayout::addWidget(const UIWidgetPtr& widget)
{
    update();
}

bool UIGridLayout::internalUpdate()
{
    bool changed = false;
    UIWidgetPtr parentWidget = getParentWidget();
    if(!parentWidget)
        return false;

    UIWidgetList widgets = parentWidget->getChildren();

    Rect clippingRect = parentWidget->getPaddingRect();
    Point topLeft = clippingRect.topLeft();

    int numColumns = m_numColumns;
    if(m_flow && m_cellSize.width() > 0) {
        numColumns = (clippingRect.width() + m_cellSpacing) / (m_cellSize.width() + m_cellSpacing);
        if(numColumns > 0) {
            m_numColumns = numColumns;
            m_numLines = std::ceil(widgets.size() / (float)numColumns);
        }
    }

    if(numColumns <= 0)
        numColumns = 1;

    int cellSpacing = m_cellSpacing;
    if(m_autoSpacing && numColumns > 1)
        cellSpacing = (clippingRect.width() - numColumns * m_cellSize.width()) / (numColumns - 1);

    int index = 0;
    int preferredHeight = 0;
    for(const UIWidgetPtr& widget : widgets) {
        if(!widget->isExplicitlyVisible())
            continue;

        int line = index / numColumns;
        int column = index % numColumns;
        Point virtualPos = Point(column * (m_cellSize.width() + cellSpacing), line * (m_cellSize.height() + cellSpacing));
        preferredHeight = virtualPos.y + m_cellSize.height();
        Point pos = topLeft + virtualPos - parentWidget->getVirtualOffset();
        Rect dest = Rect(pos, m_cellSize);
        dest.expand(-widget->getMarginTop(), -widget->getMarginRight(), -widget->getMarginBottom(), -widget->getMarginLeft());

        if(widget->setRect(dest))
            changed = true;

        index++;

        if(m_numLines > 0 && index >= m_numColumns * m_numLines)
            break;
    }
    preferredHeight += parentWidget->getPaddingTop() + parentWidget->getPaddingBottom();

    if(m_fitChildren && preferredHeight != parentWidget->getHeight()) {
        // must set the preferred height later
        g_dispatcher.addEvent([=] {
            parentWidget->setHeight(preferredHeight);
        });
    }

    return changed;
}
