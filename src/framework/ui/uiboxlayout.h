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

#ifndef UIBOXLAYOUT_H
#define UIBOXLAYOUT_H

#include "uilayout.h"

// @bindclass
class UIBoxLayout : public UILayout
{
public:
    UIBoxLayout(UIWidgetPtr parentWidget);

    void applyStyle(const OTMLNodePtr& styleNode);
    void addWidget(const UIWidgetPtr& widget) { update(); }
    void removeWidget(const UIWidgetPtr& widget) { update(); }

    void setSpacing(int spacing) { m_spacing = spacing; update(); }
    void setFitChildren(bool fitParent) { m_fitChildren = fitParent; update(); }

    bool isUIBoxLayout() { return true; }

protected:
    stdext::boolean<false> m_fitChildren;
    int m_spacing;
};

#endif
