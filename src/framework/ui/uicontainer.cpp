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
#include <core/resources.h>
#include <ui/uicontainer.h>
#include <core/dispatcher.h>

void UIContainer::internalOnDestroy()
{
    //logTraceDebug(getId());

    // clear additional references
    m_lockedElements.clear();
    m_focusedElement.reset();

    // destroy children
    while(m_children.size() > 0) {
        UIElementPtr element = m_children.back(); //hold reference
        element->internalOnDestroy();
    }

    UIElement::internalOnDestroy();
}

UIContainerPtr& UIContainer::getRoot()
{
    static UIContainerPtr rootContainer;
    if(!rootContainer) {
        rootContainer = UIContainerPtr(new UIContainer);
        rootContainer->setId("root");
    }
    return rootContainer;
}

void UIContainer::addChild(UIElementPtr child)
{
    m_children.push_back(child);
    child->setParent(asUIContainer());
}

void UIContainer::removeChild(UIElementPtr child)
{
    // defocus if needed
    if(m_focusedElement == child)
        setFocusedElement(UIElementPtr());

    // try to unlock
    unlockElement(child);

    // remove from children list
    m_children.remove(child);

    // child must have this container as parent
    child->setParent(UIContainerPtr());
}

UIElementPtr UIContainer::getChildById(const std::string& id)
{
    if(getId() == id)
        return asUIElement();

    foreach(const UIElementPtr& child, m_children) {
        if(child->getId() == id)
            return child;
    }

    return UIElementPtr();
}

UIElementPtr UIContainer::getChildByPos(const Point& pos)
{
    for(auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        const UIElementPtr& element = (*it);
        if(element->getRect().contains(pos))
            return element;
    }

    return UIElementPtr();
}

UIElementPtr UIContainer::recursiveGetChildById(const std::string& id)
{
    if(getId() == id)
        return asUIElement();

    foreach(const UIElementPtr& element, m_children) {
        if(element->getId() == id)
            return element;
        else {
            UIContainerPtr container = element->asUIContainer();
            if(container) {
                UIElementPtr element2 = container->recursiveGetChildById(id);
                if(element2)
                    return element2;
            }
        }
    }

    return UIElementPtr();
}

void UIContainer::pushChildToTop(const UIElementPtr& child)
{
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if(it != m_children.end()) {
        m_children.erase(it);
        m_children.push_back(child);
    }
}

void UIContainer::onLoad()
{
    foreach(const UIElementPtr& child, m_children)
        child->onLoad();
    UIElement::onLoad();
}

void UIContainer::render()
{
    UIElement::render();
    foreach(const UIElementPtr& child, m_children) {
        if(child->isVisible())
            child->render();
    }
}

void UIContainer::onInputEvent(const InputEvent& event)
{
    UIElementPtr focusedElement = m_focusedElement;
    foreach(const UIElementPtr& child, m_children) {
        bool shouldFire = false;

        // events should pass only when element is visible and enabled
        if(child->isEnabled() && child->isVisible()) {
            if(event.type & EV_KEYBOARD) {
                // keyboard events only go to focused elements or containers
                if(child->asUIContainer() || child == focusedElement) {
                    shouldFire = true;
                }
            // mouse events
            } else if(event.type & EV_MOUSE) {
                // mouse down and wheel events only go to elements that contains the mouse position
                if(event.type & EV_DOWN || event.type & EV_MOUSE_WHEEL) {
                    // the child must contains the mouse position and be on top
                    if(child->getRect().contains(event.mousePos) && child == getChildByPos(event.mousePos)) {
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

void UIContainer::focusNextElement()
{
    UIElementPtr element;
    std::list<UIElementPtr> rotatedChildren(m_children);
    auto focusedIt = std::find(rotatedChildren.begin(), rotatedChildren.end(), m_focusedElement);
    if(focusedIt != rotatedChildren.end()) {
        std::rotate(rotatedChildren.begin(), focusedIt, rotatedChildren.end());
        rotatedChildren.pop_front();
        foreach(const UIElementPtr& child, rotatedChildren) {
            if(child->isFocusable()) {
                element = child;
                break;
            }
        }
    }

    if(element)
        setFocusedElement(element);
}

void UIContainer::setFocusedElement(UIElementPtr focusedElement)
{
    if(focusedElement != m_focusedElement) {
        if(m_focusedElement) {
            m_focusedElement->setFocused(false);
            m_focusedElement->onFocusChange();
        }

        m_focusedElement = focusedElement;
        if(m_focusedElement) {
            m_focusedElement->setFocused(true);
            m_focusedElement->onFocusChange();
        }
    }

    // when containers are focused they go to the top
    if(focusedElement && focusedElement->asUIContainer()) {
        g_dispatcher.addTask(boost::bind(&UIContainer::pushChildToTop, asUIContainer(), m_focusedElement));
    }
}

bool UIContainer::lockElement(UIElementPtr element)
{
    if(std::find(m_children.begin(), m_children.end(), element) != m_children.end()) {
        m_lockedElements.remove(element);
        m_lockedElements.push_front(element);
        foreach(const UIElementPtr& child, m_children) {
            if(child != element)
                child->setEnabled(false);
            else
                child->setEnabled(true);
        }
        return true;
    }
    return false;
}

bool UIContainer::unlockElement(UIElementPtr element)
{
    auto it = std::find(m_lockedElements.begin(), m_lockedElements.end(), element);
    if(it != m_lockedElements.end()) {
        m_lockedElements.erase(it);
        UIElementPtr newLockedElement;
        if(m_lockedElements.size() > 0)
            newLockedElement = m_lockedElements.front();
        foreach(const UIElementPtr& child, m_children) {
            if(newLockedElement) {
                if(child == newLockedElement)
                    child->setEnabled(true);
                else
                    child->setEnabled(false);
            } else
                child->setEnabled(true);
        }
        return true;
    }
    return false;
}
