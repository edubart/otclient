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

#include "uiitem.h"
#include <framework/otml/otml.h>
#include <framework/graphics/graphics.h>

UIItem::UIItem()
{
    m_itemMargin = 0;
}

void UIItem::render()
{
    renderSelf();

    if(m_item) {
        g_painter.setColor(Fw::white);
        m_item->draw(m_rect.bottomRight() - Point(32, 32) + m_itemMargin, m_rect);
    }

    renderChildren();
}

void UIItem::onStyleApply(const std::string& styleName, const OTMLNodePtr& styleNode)
{
    for(OTMLNodePtr node : styleNode->children()) {
        if(node->tag() == "item margin")
            m_itemMargin = node->value<int>();
    }

    UIWidget::onStyleApply(styleName, styleNode);
}
