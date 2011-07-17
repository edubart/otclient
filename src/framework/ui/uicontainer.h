#ifndef UICONTAINER_H
#define UICONTAINER_H

#include <global.h>
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
    /// Find an element in this container and in its children by id
    UIElementPtr recursiveGetChildById(const std::string& id);
    /// Find an element by position
    UIElementPtr getChildByPos(const Point& pos);
    /// Find an element in this container and in its children by position
    UIElementPtr recursiveGetChildByPos(const Point& pos);
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

    virtual const char *getScriptObjectType() const { return "UIContainer"; }

    /// Get root container (the container that contains everything)
    static UIContainerPtr& getRoot();

private:
    std::list<UIElementPtr> m_children;
    std::list<UIElementPtr> m_lockedElements;
    UIElementPtr m_focusedElement;
};

#endif // UICONTAINER_H
