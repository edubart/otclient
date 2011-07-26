#ifndef UILAYOUT_H
#define UILAYOUT_H

#include <global.h>

class UIElement;
typedef std::shared_ptr<UIElement> UIElementPtr;

class UILayout;
typedef std::shared_ptr<UILayout> UILayoutPtr;

class UILayout : public std::enable_shared_from_this<UILayout>
{
public:
    UILayout() { }

    virtual void recalculateElementLayout(const UIElementPtr& element) = 0;
    virtual void recalculateChildrenLayout(const UIElementPtr& parent) = 0;
};

#endif // UILAYOUT_H