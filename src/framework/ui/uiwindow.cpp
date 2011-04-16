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


#include "uiwindow.h"
#include "uiwindowskin.h"

void UIWindow::onInputEvent(const InputEvent& event)
{
    UIContainer::onInputEvent(event);
    if(event.type == EV_MOUSE_LDOWN) {
        UIWindowSkin *skin = static_cast<UIWindowSkin*>(getSkin());
        Rect headRect = getRect();
        headRect.setHeight(skin->getHeadHeight());
        if(headRect.contains(event.mousePos)) {
            m_moving = true;
        }
    } else if(event.type == EV_MOUSE_LUP) {
        if(m_moving) {
            m_moving = false;
        }
    } else if(event.type == EV_MOUSE_MOVE) {
        if(m_moving) {
            moveTo(getRect().topLeft() + event.mouseMoved);
        }
    }
}
