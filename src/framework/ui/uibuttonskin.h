#ifndef UIBUTTONSKIN_H
#define UIBUTTONSKIN_H

#include <global.h>
#include <ui/uiconstants.h>
#include <ui/uielementskin.h>

class Font;

struct UIButtonStateSkin {
    ImagePtr image;
    Point textTranslate;
    Color textColor;
};
typedef boost::shared_ptr<UIButtonStateSkin> UIButtonStateSkinPtr;

class UIButtonSkin : public UIElementSkin
{
public:
    UIButtonSkin(const std::string& name) :
        UIElementSkin(name, UI::Button) { }

    void load(OTMLNode* node);
    UIButtonStateSkinPtr loadStateSkin(OTMLNode* node);

    void draw(UIElement *element);

private:
    UIButtonStateSkinPtr m_statesSkin[3];
};

#endif // UIBUTTONSKIN_H
