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

void UIItem::draw()
{
    drawSelf();

    if(m_item) {
        Point topLeft = m_rect.bottomRight() - Point(32, 32) + Point(m_padding.left, m_padding.top);

        g_painter.setColor(Fw::white);
        m_item->draw(topLeft, m_rect);

        if(m_font && m_item->isStackable() && m_item->getData() > 1) {
            std::string count = Fw::tostring(m_item->getData());
            m_font->renderText(count, Rect(m_rect.topLeft(), m_rect.bottomRight() - Point(3, 0)), Fw::AlignBottomRight, Color(231, 231, 231));
        }

        //m_font->renderText(Fw::unsafeCast<std::string>(m_item->getId()), m_rect);
    }


    drawChildren();
}
