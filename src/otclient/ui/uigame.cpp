/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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

#include "uigame.h"
#include <otclient/core/game.h>
#include <framework/ui/uilineedit.h>
#include <framework/platform/platformwindow.h>

bool UIGame::onKeyPress(uchar keyCode, int keyboardModifiers, bool wouldFilter)
{
    if(UIWidget::onKeyPress(keyCode, keyboardModifiers, wouldFilter))
        return true;

    UILineEditPtr chatLineEdit = std::dynamic_pointer_cast<UILineEdit>(getParent()->recursiveGetChildById("consoleLineEdit"));

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
        } else if(wouldFilter) {
            return false;
        } else if(keyCode == Fw::KeyReturn || keyCode == Fw::KeyEnter) {
            g_game.talk(chatLineEdit->getText());
            chatLineEdit->clearText();
            return true;
        } else if(keyCode == Fw::KeyDelete) {
            chatLineEdit->removeCharacter(true);
            return true;
        } else if(keyCode == Fw::KeyBackspace) {
            chatLineEdit->removeCharacter(false);
            return true;
        } else if(keyCode == Fw::KeyRight) {
            chatLineEdit->moveCursor(true);
            return true;
        } else if(keyCode == Fw::KeyLeft) {
            chatLineEdit->moveCursor(false);
            return true;
        } else if(keyCode == Fw::KeyHome) {
            chatLineEdit->setCursorPos(0);
            return true;
        } else if(keyCode == Fw::KeyEnd) {
            chatLineEdit->setCursorPos(chatLineEdit->getText().length());
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
        } else if(keyCode == Fw::KeyV) {
            chatLineEdit->appendText(g_window.getClipboardText());
        }
    } else if(keyboardModifiers == Fw::KeyboardShiftModifier) {
        if(keyCode == Fw::KeyRight || keyCode == Fw::KeyNumpad6) {
            chatLineEdit->moveCursor(true);
            return true;
        } else if(keyCode == Fw::KeyLeft || keyCode == Fw::KeyNumpad4) {
            chatLineEdit->moveCursor(false);
            return true;
        }
    }

    return false;
}

bool UIGame::onKeyText(const std::string& keyText)
{
    if(UIWidget::onKeyText(keyText))
        return true;

    UILineEditPtr chatLineEdit = std::dynamic_pointer_cast<UILineEdit>(getParent()->recursiveGetChildById("consoleLineEdit"));
    chatLineEdit->appendText(keyText);
    return true;
}
