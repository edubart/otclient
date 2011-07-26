#ifndef UITEXTEDIT_H
#define UITEXTEDIT_H

#include <global.h>
#include <graphics/textarea.h>
#include <ui/uielement.h>

class Font;

class UITextEdit;
typedef std::shared_ptr<UITextEdit> UITextEditPtr;

class UITextEdit : public UIElement
{
public:
    UITextEdit();

    static UITextEditPtr create() { return UITextEditPtr(new UITextEdit); }

    void onInputEvent(const InputEvent& event);
    void onRectUpdate();
    void onFocusChange();

    void setText(const std::string& text) { m_textArea.setText(text); }
    std::string getText() const { return m_textArea.getText(); }
    TextArea& getTextArea() { return m_textArea; }

    bool isFocusable() const { return true; }

    virtual const char *getLuaTypeName() const { return "UITextEdit"; }

private:
    TextArea m_textArea;
};

#endif // UITEXTEDIT_H
