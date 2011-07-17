#ifndef UILABELSKIN_H
#define UILABELSKIN_H

#include <global.h>
#include <graphics/font.h>
#include <ui/uielementskin.h>

class UILabelSkin : public UIElementSkin
{
public:
    UILabelSkin(const std::string& name) :
        UIElementSkin(name, UI::Label) { }

    void load(OTMLNode* node);
    void apply(UIElement *element);
    void draw(UIElement *element);
};

#endif // UILABELSKIN_H
