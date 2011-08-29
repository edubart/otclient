/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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

#include "uimap.h"
#include <otclient/core/map.h>
#include <otclient/core/game.h>

void UIMap::setup()
{
    UIWidget::setup();
}

void UIMap::render()
{
    if(g_game.isOnline())
        g_map.draw(m_rect);

    UIWidget::render();
}

bool UIMap::onKeyPress(uchar keyCode, char keyChar, int keyboardModifiers)
{
    if(keyboardModifiers == Fw::KeyboardNoModifier) {
        if(keyCode == Fw::KeyUp || keyCode == Fw::KeyNumpad8) {
            g_game.walk(Otc::North);
            return true;
        } else if(keyCode == Fw::KeyRight || keyCode == Fw::KeyNumpad6) {
            g_game.walk(Otc::East);
            return true;
        } else if(keyCode == Fw::KeyDown || keyCode == Fw::KeyNumpad2) {
            g_game.walk(Otc::South);
            return true;
        } else if(keyCode == Fw::KeyLeft || keyCode == Fw::KeyNumpad4) {
            g_game.walk(Otc::West);
            return true;
        } else if(keyCode == Fw::KeyNumpad9) {
            g_game.walk(Otc::NorthEast);
            return true;
        } else if(keyCode == Fw::KeyNumpad3) {
            g_game.walk(Otc::SouthEast);
            return true;
        } else if(keyCode == Fw::KeyNumpad1) {
            g_game.walk(Otc::SouthWest);
            return true;
        } else if(keyCode == Fw::KeyNumpad7) {
            g_game.walk(Otc::NorthWest);
            return true;
        }
    } else if(keyboardModifiers == Fw::KeyboardCtrlModifier) {
        if(keyCode == Fw::KeyUp || keyCode == Fw::KeyNumpad8) {
            g_game.turn(Otc::North);
            return true;
        } else if(keyCode == Fw::KeyRight || keyCode == Fw::KeyNumpad6) {
            g_game.turn(Otc::East);
            return true;
        } else if(keyCode == Fw::KeyDown || keyCode == Fw::KeyNumpad2) {
            g_game.turn(Otc::South);
            return true;
        } else if(keyCode == Fw::KeyLeft || keyCode == Fw::KeyNumpad4) {
            g_game.turn(Otc::West);
            return true;
        }
    }
    return UIWidget::onKeyPress(keyCode, keyChar, keyboardModifiers);
}

bool UIMap::onMousePress(const Point& mousePos, Fw::MouseButton button)
{
    return UIWidget::onMousePress(mousePos, button);
}
