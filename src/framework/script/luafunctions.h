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


#ifndef LUAFUNCTIONS_H
#define LUAFUNCTIONS_H

#include <global.h>
#include <script/scriptable.h>

void registerLuaFunctions();

// App
int lua_App_exit();

// UI
int lua_UI_load();
int lua_UI_getRootContainer();

// UIElement
int lua_UIElement_getId();
int lua_UIElement_setId();
int lua_UIElement_isEnabled();
int lua_UIElement_setEnabled();
int lua_UIElement_isVisible();
int lua_UIElement_setVisible();
int lua_UIElement_isFocused();
int lua_UIElement_setFocused();
int lua_UIElement_getParent();
int lua_UIElement_setParent();
int lua_UIElement_setLocked();
int lua_UIElement_destroy();
void lua_UIElement_onLoad();
void lua_UIElement_onDestroy();

// UIContainer
int lua_UIContainer_getChild();
int lua_UIContainer_getChildren();

// UILabel
int lua_UILabel_setText();
int lua_UILabel_getText();

// UITextEdit
int lua_UITextEdit_setText();
int lua_UITextEdit_getText();

// UIButton
void lua_UIButton_onClick();

// UIWindow
int lua_UIWindow_setTitle();
int lua_UIWindow_getTitle();

#endif // LUAFUNCTIONS_H
