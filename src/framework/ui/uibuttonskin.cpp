#include <global.h>
#include <graphics/fonts.h>
#include <ui/uibuttonskin.h>
#include <ui/uibutton.h>

void UIButtonSkin::load(OTMLNode* node)
{
    UIElementSkin::load(node);

    UIButtonStateSkinPtr defaultStateSkin = loadStateSkin(node);
    m_statesSkin[UIButton::ButtonUp] = defaultStateSkin;
    m_statesSkin[UIButton::ButtonDown] = defaultStateSkin;
    m_statesSkin[UIButton::ButtonMouseOver] = defaultStateSkin;

    if(OTMLNode* cnode = node->at("down state"))
        m_statesSkin[UIButton::ButtonDown] = loadStateSkin(cnode);

    if(OTMLNode* cnode = node->at("hover state"))
        m_statesSkin[UIButton::ButtonMouseOver] = loadStateSkin(cnode);
}

UIButtonStateSkinPtr UIButtonSkin::loadStateSkin(OTMLNode* node)
{
    UIButtonStateSkinPtr stateSkin = UIButtonStateSkinPtr(new UIButtonStateSkin);
    stateSkin->image = loadImage(node);
    stateSkin->textTranslate = node->readAt("text translate", Point());
    stateSkin->textColor = node->readAt("font color", getFontColor());
    return stateSkin;
}

void UIButtonSkin::draw(UIElement *element)
{
    UIButton *button = static_cast<UIButton*>(element);
    UIButtonStateSkinPtr stateSkin = m_statesSkin[button->getState()];
    Rect textRect = button->getRect();

    stateSkin->image->draw(element->getRect());
    textRect.translate(stateSkin->textTranslate);

    getFont()->renderText(button->getText(), textRect, AlignCenter, stateSkin->textColor);
}
