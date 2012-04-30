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
#include <framework/graphics/fontmanager.h>

UIItem::UIItem()
{
    m_dragable = true;
}

void UIItem::drawSelf()
{
    UIWidget::drawSelf();

    if(m_item) {
        Rect drawRect = getClippingRect();
        Point dest = drawRect.topLeft();
        float scaleFactor = std::min(drawRect.width() / (float)m_item->getExactSize(), drawRect.height() / (float)m_item->getExactSize());
        dest += m_item->getDisplacement() * scaleFactor;

        g_painter->setColor(Color::white);
        m_item->draw(dest, scaleFactor, true);

        if(m_font && m_item->isStackable() && m_item->getCount() > 1) {
            std::string count = Fw::tostring(m_item->getCount());
            g_painter->setColor(Color(231, 231, 231));
            m_font->drawText(count, Rect(m_rect.topLeft(), m_rect.bottomRight() - Point(3, 0)), Fw::AlignBottomRight);
        }
        //m_font->drawText(Fw::tostring(m_item->getId()), m_rect, Fw::AlignBottomRight);
    }
}

void UIItem::setItemId(int id)
{
    if(!m_item && id != 0)
        m_item = Item::create(id);
    else {
        // remove item
        if(id == 0)
            m_item = nullptr;
        else
            m_item->setId(id);
    }
}

void UIItem::onStyleApply(const std::string& styleName, const OTMLNodePtr& styleNode)
{
    UIWidget::onStyleApply(styleName, styleNode);

    for(const OTMLNodePtr& node : styleNode->children()) {
        if(node->tag() == "item-id")
            setItemId(node->value<int>());
        else if(node->tag() == "item-count")
            setItemCount(node->value<int>());
        else if(node->tag() == "virtual")
            setVirtual(node->value<bool>());
    }
}
