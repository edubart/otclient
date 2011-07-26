#ifndef UIBUTTON_H
#define UIBUTTON_H

#include <global.h>
#include <ui/uielement.h>
#include <graphics/borderedimage.h>

class UIButton;
typedef std::shared_ptr<UIButton> UIButtonPtr;

class UIButton : public UIElement
{
public:
    enum ButtonState
    {
        ButtonUp = 0,
        ButtonDown,
        ButtonMouseOver
    };

    UIButton() :
        UIElement(UI::Button),
        m_state(ButtonUp) {  }

    static UIButtonPtr create() { return UIButtonPtr(new UIButton); }

    void onInputEvent(const InputEvent& event);

    void setText(const std::string& text) { m_text = text; }
    std::string getText() const { return m_text; }

    ButtonState getState() { return m_state; }

    virtual const char *getLuaTypeName() const { return "UIButton"; }

private:
    std::string m_text;
    ButtonState m_state;
};

#endif // UIBUTTON_H
