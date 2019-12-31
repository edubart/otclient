/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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

#ifndef FRAMEWORK_UI_DECLARATIONS_H
#define FRAMEWORK_UI_DECLARATIONS_H

#include <framework/global.h>

class UIManager;
class UIWidget;
class UITextEdit;
class UILayout;
class UIBoxLayout;
class UIHorizontalLayout;
class UIVerticalLayout;
class UIGridLayout;
class UIAnchor;
class UIAnchorGroup;
class UIAnchorLayout;
class UIParticles;

typedef stdext::shared_object_ptr<UIWidget> UIWidgetPtr;
typedef stdext::shared_object_ptr<UIParticles> UIParticlesPtr;
typedef stdext::shared_object_ptr<UITextEdit> UITextEditPtr;
typedef stdext::shared_object_ptr<UILayout> UILayoutPtr;
typedef stdext::shared_object_ptr<UIBoxLayout> UIBoxLayoutPtr;
typedef stdext::shared_object_ptr<UIHorizontalLayout> UIHorizontalLayoutPtr;
typedef stdext::shared_object_ptr<UIVerticalLayout> UIVerticalLayoutPtr;
typedef stdext::shared_object_ptr<UIGridLayout> UIGridLayoutPtr;
typedef stdext::shared_object_ptr<UIAnchor> UIAnchorPtr;
typedef stdext::shared_object_ptr<UIAnchorGroup> UIAnchorGroupPtr;
typedef stdext::shared_object_ptr<UIAnchorLayout> UIAnchorLayoutPtr;

typedef std::deque<UIWidgetPtr> UIWidgetList;
typedef std::vector<UIAnchorPtr> UIAnchorList;

#endif
