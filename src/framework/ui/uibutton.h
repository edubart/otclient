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


#ifndef UIBUTTON_H
#define UIBUTTON_H

#include "prerequisites.h"
#include "uielement.h"
#include "graphics/borderedimage.h"

typedef std::function<void()> Callback;

class UIButton : public UIElement
{
public:
    UIButton(const std::string& text = std::string()) : UIElement(UI::Button),
        m_text(text),
        m_state(UI::ButtonUp) {
            UIElement();
        }

    void onInputEvent(const InputEvent& event);

    void setText(const std::string& text) { m_text = text; }
    const std::string& getText() const { return m_text; }

    UI::EButtonState getState() { return m_state; }

    void onClick(const Callback& callback) { m_buttonClickCallback = callback; }

private:
    std::string m_text;
    UI::EButtonState m_state;
    Callback m_buttonClickCallback;
};

typedef std::shared_ptr<UIButton> UIButtonPtr;

#endif // UIBUTTON_H
