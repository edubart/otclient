#include <global.h>
#include <graphics/fonts.h>
#include <ui/uilabelskin.h>
#include <ui/uilabel.h>

void UILabelSkin::load(OTMLNode* node)
{
    UIElementSkin::load(node);
}

void UILabelSkin::apply(UIElement* element)
{
    UIElementSkin::apply(element);
    UILabel *label = static_cast<UILabel*>(element);
    label->setSize(getFont()->calculateTextRectSize(label->getText()));
}

void UILabelSkin::draw(UIElement *element)
{
    UIElementSkin::draw(element);
    UILabel *label = static_cast<UILabel*>(element);
    getFont()->renderText(label->getText(), label->getRect(), label->getAlign(), getFontColor());
}
