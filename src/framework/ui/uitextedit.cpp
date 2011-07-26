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


#include <global.h>
#include <ui/uitextedit.h>
#include <ui/uitexteditskin.h>
#include <ui/uicontainer.h>

UITextEdit::UITextEdit() :
    UIElement(UI::TextEdit)
{
    m_textArea.enableCursor();
}

void UITextEdit::onInputEvent(const InputEvent& event)
{
    if(event.type == EV_TEXT_ENTER) {
        m_textArea.appendCharacter(event.keychar);
    } else if(event.type == EV_KEY_DOWN) {
        if(event.keycode == KC_DELETE) // erase right character
            m_textArea.removeCharacter(true);
        else if(event.keycode == KC_BACK) // erase left character
            m_textArea.removeCharacter(false);
        else if(event.keycode == KC_RIGHT) // move cursor right
            m_textArea.moveCursor(true);
        else if(event.keycode == KC_LEFT) // move cursor left
            m_textArea.moveCursor(false);
        else if(event.keycode == KC_HOME) // move cursor to first character
            m_textArea.setCursorPos(0);
        else if(event.keycode == KC_END) // move cursor to last character
            m_textArea.setCursorPos(m_textArea.getText().length());
        else if(event.keycode == KC_TAB) // focus next parent element
            getParent()->focusNextElement();
    } else if(event.type == EV_MOUSE_LDOWN) {
        int pos = m_textArea.getTextPos(event.mousePos);
        if(pos >= 0)
            m_textArea.setCursorPos(m_textArea.getTextPos(event.mousePos));
    } else if(event.type == EV_MOUSE_LUP && getRect().contains(event.mousePos)) {
    }
}

void UITextEdit::onRectUpdate()
{
    UITextEditSkin *skin = static_cast<UITextEditSkin*>(getSkin().get());
    Rect textRect = getRect();
    int margin = skin->getTextMargin();
    textRect.setLeft(textRect.left()+margin);
    textRect.setRight(textRect.right()-margin);
    m_textArea.setScreenCoords(textRect);
}

void UITextEdit::onFocusChange()
{
    m_textArea.setCursorVisible(isFocused());
}

