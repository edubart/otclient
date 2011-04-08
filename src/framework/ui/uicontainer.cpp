/* The MIT License
 *
 * Copyright (c) 2010 OTClient, https://github.com/edubart/otclient
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


#include "uicontainer.h"

UIContainer g_gui;

void UIContainer::addChild(UIElementPtr child)
{
    m_children.push_back(child);
    child->setParent(asUIContainer());

    // adjust child rect
    Rect childRect = child->getRect();
    childRect.translate(m_rect.topLeft());
    child->setRect(childRect);
}

void UIContainer::removeChild(UIElementPtr child)
{
    if(m_activeElement == child)
        setActiveElement(UIElementPtr());
    m_children.remove(child);
}

UIElementPtr UIContainer::getChildByName(const std::string& name) const
{
    for(auto it = m_children.begin(); it != m_children.end(); ++it) {
        if((*it)->getName() == name) {
            return (*it);
            break;
        }
    }
    return UIElementPtr();
}


void UIContainer::setRect(const Rect& rect)
{
    // update children rect
    for(auto it = m_children.begin(); it != m_children.end(); ++it) {
        UIElementPtr child = (*it)->asUIElement();

        // transforme child rect
        Rect childRect = child->getRect();
        childRect.translate(-m_rect.topLeft());
        childRect.translate(rect.topLeft());
        child->setRect(childRect);
    }

    m_rect = rect;
}

void UIContainer::resize(const Size& size)
{
    Rect newRect = m_rect;
    newRect.setSize(size);
    setRect(newRect);
}

void UIContainer::move(const Point& trans)
{
    Rect newRect = m_rect;
    newRect.translate(trans);
    setRect(newRect);
}

void UIContainer::moveTo(const Point& pos)
{
    Rect newRect = m_rect;
    newRect.moveTo(pos);
    setRect(newRect);

}

void UIContainer::setActiveElement(UIElementPtr activeElement)
{

}
