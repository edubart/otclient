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


#include <prerequisites.h>
#include <core/dispatcher.h>
#include <ui/uibutton.h>
#include "uicontainer.h"

void UIButton::onInputEvent(const InputEvent& event)
{
    if(event.type == EV_MOUSE_LDOWN && getRect().contains(event.mousePos)) {
        m_state = ButtonDown;
    } else if(event.type == EV_MOUSE_LUP && m_state == ButtonDown) {
        m_state = ButtonUp;
        if(getRect().contains(event.mousePos)) {
            g_dispatcher.addTask(boost::bind(&Scriptable::callLuaTableField, shared_from_this(), "onClick", 0));
        }
    } else if(event.type == EV_MOUSE_MOVE && m_state != ButtonDown) {
        if(isMouseOver())
            m_state = ButtonMouseOver;
        else
            m_state = ButtonUp;
    }
}
