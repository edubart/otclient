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


#ifndef UICONTAINER_H
#define UICONTAINER_H

#include <prerequisites.h>
#include <ui/uielement.h>

class UIContainer : public UIElement
{
public:
    UIContainer(UI::ElementType type = UI::Container) :
        UIElement(type) { }
    virtual ~UIContainer() { }

    virtual void destroy();
    virtual void onLoad();
    virtual void render();
    virtual void onInputEvent(const InputEvent& event);

    /// Add an element, this must never be called from events loops
    void addChild(const UIElementPtr& child);
    /// Remove an element, this must never be called from events loops
    void removeChild(const UIElementPtr& child);
    /// Check if has child
    bool hasChild(const UIElementPtr& child);
    /// Find an element in this container by id
    UIElementPtr getChildById(const std::string& id);
    /// Find an element by position
    UIElementPtr getChildByPos(const Point& pos);
    /// Find an element in this container and in its children by id
    UIElementPtr recursiveGetChildById(const std::string& id);
    /// Get children
    const std::list<UIElementPtr>& getChildren() const { return m_children; }
    /// Pushs a child to the top
    void pushChildToTop(const UIElementPtr& child);

    int getChildCount() const { return m_children.size(); }

    /// Disable all children except the specified element
    bool lockElement(const UIElementPtr& element);
    /// Renable all children
    bool unlockElement(const UIElementPtr& element);

    /// Focus next element
    void focusNextElement();
    /// Focus element
    void setFocusedElement(const UIElementPtr& focusedElement);
    /// Get focused element
    UIElementPtr getFocusedElement() const { return m_focusedElement; }

    virtual UI::ElementType getElementType() const { return UI::Container; }
    UIContainerPtr asUIContainer() { return boost::static_pointer_cast<UIContainer>(shared_from_this()); }

    virtual const char *getScriptableName() const { return "UIContainer"; }

    /// Get root container (the container that contains everything)
    static UIContainerPtr& getRoot();

private:
    std::list<UIElementPtr> m_children;
    std::list<UIElementPtr> m_lockedElements;
    UIElementPtr m_focusedElement;
};

#endif // UICONTAINER_H
