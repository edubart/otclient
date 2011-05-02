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

#include <prerequisites.h>
#include <script/scriptable.h>

void registerLuaFunctions();

// global functions
int lua_App_exit();
int lua_App_setOnClose();

int lua_UI_load();
int lua_UI_getRootContainer();

// UIElement
int lua_UIElement_setOnLoad();
int lua_UIElement_setOnDestroy();
int lua_UIElement_getParent();
int lua_UIElement_destroy();

// UIContainer
int lua_UIContainer_getChildById();
int lua_UIContainer_setLocked();

// UILabel
int lua_UILabel_setText();
int lua_UILabel_getText();

// UIButton
int lua_UIButton_setOnClick();

// UIWindow
int lua_UIWindow_setTitle();
int lua_UIWindow_getTitle();

#endif // LUAFUNCTIONS_H
