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


#include "uitextedit.h"
#include "graphics/fonts.h"

UITextEdit::UITextEdit(Font* font) :
    UIElement(UI::TextEdit),
    m_cursorPos(0),
    m_startRenderPos(0),
    m_font(font)
{
    if(!font)
        m_font = g_fonts.get("tibia-10px-antialised");
}

void UITextEdit::render()
{
    UIElement::render();

    Rect textRect = getRect(); 
    textRect.setLeft(textRect.left() + 2);
    textRect.setRight(textRect.right() - 2);

    // render text
    m_font->renderText(m_text, textRect, ALIGN_LEFT, Color(0xFFBFBFBF), Point(m_startRenderPos, 0), m_cursorPos);
}

void UITextEdit::onInputEvent(const InputEvent& event)
{
    if(event.type == EV_TEXT_ENTER) {
        appendCharacter(event.keychar);
    } else if(event.type == EV_KEY_DOWN) {
        if(event.keycode == KC_DELETE)
            removeCharacter(true);
        else if(event.keycode == KC_BACK)
            removeCharacter(false);
        else if(event.keycode == KC_RIGHT) {
            if(m_cursorPos < m_text.length())
                m_cursorPos++;
        } else if(event.keycode == KC_LEFT) {
            if(m_cursorPos > 0)
                m_cursorPos--;
        }
    }
}

void UITextEdit::clearText()
{
    m_text = "";
    m_cursorPos = 0;
}

void UITextEdit::setText(const std::string& text)
{
    m_text = text;
    m_cursorPos = 0;
}

void UITextEdit::appendCharacter(char c)
{
    std::string tmp;
    tmp = c;
    m_text.insert(m_cursorPos, tmp);
    m_cursorPos++;
}

void UITextEdit::removeCharacter(bool right)
{
    if(right && m_cursorPos < m_text.length())
        m_text.erase(m_text.begin() + m_cursorPos);
    else if(m_text.length() >= m_cursorPos && m_cursorPos > 0)
        m_text.erase(m_text.begin() + (--m_cursorPos));
}

void UITextEdit::setCursorPos(uint pos)
{
    if(pos > m_text.length())
        m_cursorPos = m_text.length();
    else
        m_cursorPos = pos;
}

void UITextEdit::onLayoutRectChange(const Rect& rect)
{
    m_textRect = rect;
}


