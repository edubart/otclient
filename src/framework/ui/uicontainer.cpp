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
#include <ui/uianchorlayout.h>
#include <core/dispatcher.h>

void UIContainer::destroy()
{
    //logTraceDebug(getId());

    // clear additional references
    m_lockedElements.clear();
    m_focusedElement.reset();

    // destroy children
    while(m_children.size() > 0) {
        UIElementPtr element = m_children.front(); //hold reference
        element->destroy();
    }

    UIElement::destroy();
}

UIContainerPtr& UIContainer::getRoot()
{
    static UIContainerPtr rootContainer;
    if(!rootContainer) {
        rootContainer = UIContainerPtr(new UIContainer);
        rootContainer->setId("root");
        rootContainer->setLayout(UILayoutPtr(new UIAnchorLayout));
    }
    return rootContainer;
}

void UIContainer::addChild(const UIElementPtr& child)
{
    m_children.push_back(child);
    if(child->getParent() != asUIContainer())
        child->setParent(asUIContainer());
}

void UIContainer::removeChild(const UIElementPtr& child)
{
    // defocus if needed
    if(m_focusedElement == child)
        setFocusedElement(UIElementPtr());

    // try to unlock
    unlockElement(child);

    // remove from children list
    m_children.remove(child);

    if(child->getParent() == asUIContainer())
        child->setParent(UIContainerPtr());
}

bool UIContainer::hasChild(const UIElementPtr& child)
{
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if(it != m_children.end())
        return true;
    return false;
}

UIElementPtr UIContainer::getChildById(const std::string& id)
{
    if(getId() == id || id == "self")
        return asUIElement();

    if(id == "parent")
        return getParent();

    if(id == "root")
        return getRoot();

    foreach(const UIElementPtr& child, m_children) {
        if(child->getId() == id)
            return child;
    }

    return UIElementPtr();
}

UIElementPtr UIContainer::recursiveGetChildById(const std::string& id)
{
    if(getId() == id || id == "self")
        return asUIElement();

    if(id == "parent")
        return getParent();

    if(id == "root")
        return getRoot();

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

UIElementPtr UIContainer::getChildByPos(const Point& pos)
{
    for(auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        const UIElementPtr& element = (*it);
        if(element->getRect().contains(pos))
            return element;
    }

    return UIElementPtr();
}

UIElementPtr UIContainer::recursiveGetChildByPos(const Point& pos)
{
    for(auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        const UIElementPtr& element = (*it);
        if(element->getRect().contains(pos)) {
            if(UIContainerPtr container = element->asUIContainer()) {
                if(UIElementPtr containerChild = container->recursiveGetChildByPos(pos))
                    return containerChild;
                else
                    return container;
            }
            return element;
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

                    if(event.type == EV_MOUSE_MOVE) {
                        if(child->getRect().contains(event.mousePos) && UIContainer::getRoot()->recursiveGetChildByPos(event.mousePos) == child)
                            child->setMouseOver(true);
                        else
                            child->setMouseOver(false);
                    }
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

void UIContainer::setFocusedElement(const UIElementPtr& focusedElement)
{
    if(focusedElement != m_focusedElement) {
        UIElementPtr oldFocused = m_focusedElement;
        m_focusedElement = focusedElement;

        if(oldFocused)
            oldFocused->onFocusChange();
        if(focusedElement)
            focusedElement->onFocusChange();
    }

    // when containers are focused they go to the top
    if(focusedElement && focusedElement->asUIContainer()) {
        g_dispatcher.addTask(boost::bind(&UIContainer::pushChildToTop, asUIContainer(), m_focusedElement));
    }
}

bool UIContainer::lockElement(const UIElementPtr& element)
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

bool UIContainer::unlockElement(const UIElementPtr& element)
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
