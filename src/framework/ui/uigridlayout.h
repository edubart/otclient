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

#ifndef UIGRIDLAYOUT_H
#define UIGRIDLAYOUT_H

#include <framework/ui/uilayout.h>

class UIGridLayout : public UILayout
{
public:
    UIGridLayout(UIWidgetPtr parentWidget);

    void applyStyle(const OTMLNodePtr& styleNode);
    void removeWidget(const UIWidgetPtr& widget);
    void addWidget(const UIWidgetPtr& widget);

    void setCellSize(const Size& size) { m_cellSize = size; update(); }
    void setCellWidth(int width) { m_cellSize.setWidth(width); update(); }
    void setCellHeight(int height) { m_cellSize.setHeight(height); update(); }
    void setCellSpacing(int spacing) { m_cellSpacing = spacing; update(); }
    void setNumColumns(int columns) { m_numColumns = columns; update(); }
    void setNumLines(int lines) { m_numLines = lines; update(); }

    virtual UIGridLayoutPtr asUIGridLayout() { return nullptr; }

protected:
    bool internalUpdate();

private:
    Size m_cellSize;
    int m_cellSpacing;
    int m_numColumns;
    int m_numLines;
};

#endif

