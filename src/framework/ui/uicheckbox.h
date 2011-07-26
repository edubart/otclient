#ifndef UICHECKBOX_H
#define UICHECKBOX_H

#include <global.h>
#include <ui/uielement.h>

class UICheckBox : public UIElement
{
public:
    UICheckBox(UI::ElementType type = UI::Element);

    virtual const char *getLuaTypeName() const { return "UICheckBox"; }
};

#endif // UICHECKBOX_H
