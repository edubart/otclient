/*
 * Copyright (c) 2010-2013 OTClient <https://github.com/edubart/otclient>
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

#include "mouse.h"
#include <framework/ui/uiwidget.h>
#include <framework/platform/platformwindow.h>

Mouse g_mouse;

void Mouse::setTargetCursor()
{
    //TODO: configure this in lua
    g_window.setMouseCursor("/images/cursors/targetcursor", Point(9, 9));
    m_cursorChanged = true;
}

void Mouse::setHorizontalCursor()
{
    g_window.setMouseCursor("/images/cursors/horizontal", Point(9, 4));
    m_cursorChanged = true;
}

void Mouse::setVerticalCursor()
{
    g_window.setMouseCursor("/images/cursors/vertical", Point(4, 9));
    m_cursorChanged = true;
}

void Mouse::setTextCursor()
{
    g_window.setMouseCursor("/images/cursors/text", Point(4, 9));
    m_cursorChanged = true;
}

void Mouse::restoreCursor()
{
    g_window.restoreMouseCursor();
    m_cursorChanged = false;
}

bool Mouse::isCursorChanged()
{
    return m_cursorChanged;
}

bool Mouse::isPressed(Fw::MouseButton mouseButton)
{
    return g_window.isMouseButtonPressed(mouseButton);
}
