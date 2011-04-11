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


#ifndef UITEXTEDIT_H
#define UITEXTEDIT_H

#include "prerequisites.h"
#include "uielement.h"

class Font;

class UITextEdit : public UIElement
{
public:
    UITextEdit(Font *font = NULL);

    void onInputEvent(const InputEvent& event);

    void render();

    void clearText();
    void setText(const std::string& text);
    const std::string& getText() const { return m_text; }

    void setCursorPos(uint pos);
    uint getCursorPos() { return m_cursorPos; }

    void onLayoutRectChange(const Rect& rect);

private:
    void appendCharacter(char c);
    void removeCharacter(bool right);
    void recalculate();

    Rect m_textRect;
    uint m_cursorPos;
    int m_startRenderPos;
    std::string m_text;
    Font *m_font;
};

typedef std::shared_ptr<UITextEdit> UITextEditPtr;

#endif // UITEXTEDIT_H
