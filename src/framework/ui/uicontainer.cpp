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
#include "core/resources.h"
#include "uibutton.h"
#include "uipanel.h"
#include "uilabel.h"
#include "uitextedit.h"
#include "uiwindow.h"

UIContainerPtr rootContainer(new UIContainer);

UIContainerPtr& UIContainer::getRootContainer()
{
    return rootContainer;
}

void UIContainer::addChild(UIElementPtr child)
{
    m_children.push_back(child);
    child->setParent(asUIContainer());
}

void UIContainer::removeChild(UIElementPtr child)
{
    if(m_focusedElement == child)
        setFocusedElement(UIElementPtr());
    m_children.remove(child);
    if(child->getParent() == shared_from_this())
        child->setParent(UIContainerPtr());
}

UIElementPtr UIContainer::getChildById(const std::string& id)
{
    if(getId() == id)
        return asUIElement();
    for(auto it = m_children.begin(); it != m_children.end(); ++it) {
        if((*it)->getId() == id) {
            return (*it);
        }
    }
    return UIElementPtr();
}

UIElementPtr UIContainer::recursiveGetChildById(const std::string& id)
{
    if(getId() == id)
        return asUIElement();

    UIElementPtr element;
    for(auto it = m_children.begin(); it != m_children.end(); ++it) {
        element = (*it);
        if(element->getId() == id) {
            return element;
        } else {
            UIContainerPtr container = element->asUIContainer();
            if(container) {
                element = container->recursiveGetChildById(id);
                if(element)
                    return element;
            }
        }
    }
    return UIElementPtr();
}

void UIContainer::render()
{
    UIElement::render();
    for(auto it = m_children.begin(); it != m_children.end(); ++it) {
        const UIElementPtr& child = (*it);
        if(child->isVisible())
            child->render();
    }
}

void UIContainer::onInputEvent(const InputEvent& event)
{
    for(auto it = m_children.begin(); it != m_children.end(); ++it) {
        const UIElementPtr& child = (*it);
        bool shouldFire = false;

        // events should pass only when element is visible and enabled
        if(child->isEnabled() && child->isVisible()) {
            if(event.type & EV_KEYBOARD) {
                // keyboard events only go to focused elements or containers
                if(child->asUIContainer() || child == getFocusedElement()) {
                    shouldFire = true;
                }
            // mouse events
            } else if(event.type & EV_MOUSE) {
                // mouse down and wheel events only go to elements that contains the mouse position and are not containers
                if((event.type & EV_DOWN || event.type & EV_MOUSE_WHEEL) && !child->asUIContainer()) {
                    if(child->getRect().contains(event.mousePos)) {
                        // focus it
                        if(event.type == EV_MOUSE_LDOWN && child->isFocusable())
                            setFocusedElement(child);
                        shouldFire = true;
                    }
                } else {
                    shouldFire = true;
                }
            }
        }

        if(shouldFire)
            child->onInputEvent(event);
    }
}

void UIContainer::setFocusedElement(UIElementPtr focusedElement)
{
    if(m_focusedElement) {
        m_focusedElement->setFocused(false);
        m_focusedElement->onFocusChange();
    }
    m_focusedElement = focusedElement;
    m_focusedElement->setFocused(true);
    m_focusedElement->onFocusChange();
}
