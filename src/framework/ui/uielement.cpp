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

UIElement::UIElement(UI::ElementType type) :
    Scriptable(),
    m_type(type),
    m_visible(true),
    m_enabled(true),
    m_marginLeft(0),
    m_marginRight(0),
    m_marginTop(0),
    m_marginBottom(0)
{

}

UIElement::~UIElement()
{
    //logTraceDebug(getId());
}

void UIElement::destroyLater()
{
    //logTraceDebug(getId());
    g_dispatcher.addTask(boost::bind(&UIElement::destroy, asUIElement()));
}

void UIElement::destroy()
{
    //logTraceDebug(getId());

    UIElementPtr me = asUIElement();
    callLuaTableField("onDestroy");

    // remove from parent
    if(getParent())
        getParent()->removeChild(me);

    // free script stuff
    releaseLuaTableRef();

    g_dispatcher.addTask(boost::bind(&UIElement::destroyCheck, me));
}

void UIElement::destroyCheck()
{
    //logTraceDebug(getId());

    UIElementPtr me = asUIElement();
    // check for leaks, the number of references must be always 2 here
    if(me.use_count() != 2 && me != UIContainer::getRoot()) {
        flogWarning("destroyed element with id '%s', but it still have %d references left", getId() % (me.use_count()-2));
    }
}

void UIElement::setSize(const Size& size)
{
    Rect rect = getRect();
    if(rect.isValid())
        rect.setSize(size);
    else
        rect = Rect(0, 0, size);
    setRect(rect);
    getLayout()->recalculateElementLayout(asUIElement());
}

void UIElement::setRect(const Rect& rect)
{
    if(rect != m_rect) {
        m_rect = rect;

        // rect updated, recalculate children layout
        getLayout()->recalculateChildrenLayout(asUIElement());

        onRectUpdate();
    }
}

void UIElement::setSkin(const UIElementSkinPtr& skin)
{
    m_skin = skin;
    if(skin)
        skin->apply(this);
}

void UIElement::onLoad()
{
    g_dispatcher.addTask(boost::bind(&Scriptable::callLuaTableField, shared_from_this(), "onLoad"));
}

void UIElement::render()
{
    if(m_skin)
        m_skin->draw(this);
    //g_graphics.drawBoundingRect(getRect());
}

UIElementPtr UIElement::backwardsGetElementById(const std::string& id)
{
    if(getId() == id || id == "self")
        return asUIElement();

    if(id == "parent")
        return getParent();

    if(id == "root")
        return UIContainer::getRoot();

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

void UIElement::setParent(UIContainerPtr parent)
{
    UIElementPtr me = asUIElement();
    UIContainerPtr oldParent = m_parent.lock();
    m_parent.reset();
    if(oldParent && oldParent->hasChild(me)) {
        oldParent->removeChild(me);
    }
    if(parent) {
        m_parent = UIContainerWeakPtr(parent);
        if(!parent->hasChild(me))
            parent->addChild(me);
    }
}

bool UIElement::isFocused() const
{
    if(UIContainerPtr parent = m_parent.lock())
        return (parent->getFocusedElement() == shared_from_this());
    return false;
}

void UIElement::setFocused(bool focused)
{
    if(UIContainerPtr parent = m_parent.lock()) {
        if(focused) {
            parent->setFocusedElement(asUIElement());
        } else if(parent->getFocusedElement() == asUIElement()) {
            parent->setFocusedElement(UIElementPtr());
        }
    }
}

UILayoutPtr UIElement::getLayout() const
{
    if(m_layout)
        return m_layout;
    else if(getParent())
        return getParent()->getLayout();
    return UILayoutPtr();
}
