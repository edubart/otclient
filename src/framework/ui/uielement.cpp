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


#include <prerequisites.h>
#include <core/dispatcher.h>
#include <graphics/graphics.h>
#include <ui/uielement.h>
#include <ui/uiskins.h>
#include <ui/uielementskin.h>
#include <ui/uicontainer.h>

UIElement::UIElement(UI::EElementType type) :
    UILayout(),
    m_type(type),
    m_visible(true),
    m_enabled(true),
    m_focused(false)
{

}

void UIElement::destroy()
{
    if(m_onDestroyCallback)
        g_dispatcher.addTask(boost::bind(m_onDestroyCallback, asUIElement()));
    if(getParent()) {
        // schedule removal from parent
        g_dispatcher.addTask(boost::bind(&UIContainer::removeChild, getParent(), asUIElement()));
    }
    // schedule internal destroy
    g_dispatcher.addTask(boost::bind(&UIElement::internalOnDestroy, asUIElement()));
}

void UIElement::internalOnDestroy()
{
    setVisible(false);
    setEnabled(false);

    // check for leaks, the number of references must be always 2 here
    assert(asUIElement().use_count() == 2);
}

void UIElement::setSkin(const UIElementSkinPtr& skin)
{
    m_skin = skin;
    if(skin)
        skin->apply(this);
}

void UIElement::onLoad()
{
    if(m_onLoadCallback)
        g_dispatcher.addTask(boost::bind(m_onLoadCallback, asUIElement()));
}

void UIElement::render()
{
    if(m_skin)
        m_skin->draw(this);
    //g_graphics.drawBoundingRect(getRect());
}

UIElementPtr UIElement::backwardsGetElementById(const std::string& id)
{
    if(getId() == id)
        return asUIElement();

    UIElementPtr element;

    if(asUIContainer()) {
        element = asUIContainer()->recursiveGetChildById(id);
        if(element)
            return element;
    }

    if(getParent())
        element = getParent()->backwardsGetElementById(id);
    return element;
}

void UIElement::moveTo(Point pos)
{
    Rect newRect = getRect();
    newRect.moveTo(pos);

    // bound newRect to parent rect
    UIContainerPtr parent = getParent();
    if(parent) {
        Rect parentRect = parent->getRect();
        if(newRect.left() < parentRect.left())
            newRect.moveLeft(parentRect.left());
        if(newRect.top() < parentRect.top())
            newRect.moveTop(parentRect.top());
        if(newRect.bottom() > parentRect.bottom())
            newRect.moveBottom(parentRect.bottom());
        if(newRect.right() > parentRect.right())
            newRect.moveRight(parentRect.right());
    }
    setRect(newRect);
}
