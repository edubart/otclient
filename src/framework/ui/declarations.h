/*
 * Copyright (c) 2010-2022 OTClient <https://github.com/edubart/otclient>
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

#pragma once

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

using UIWidgetPtr = stdext::shared_object_ptr<UIWidget>;
using UIParticlesPtr = stdext::shared_object_ptr<UIParticles>;
using UITextEditPtr = stdext::shared_object_ptr<UITextEdit>;
using UILayoutPtr = stdext::shared_object_ptr<UILayout>;
using UIBoxLayoutPtr = stdext::shared_object_ptr<UIBoxLayout>;
using UIHorizontalLayoutPtr = stdext::shared_object_ptr<UIHorizontalLayout>;
using UIVerticalLayoutPtr = stdext::shared_object_ptr<UIVerticalLayout>;
using UIGridLayoutPtr = stdext::shared_object_ptr<UIGridLayout>;
using UIAnchorPtr = stdext::shared_object_ptr<UIAnchor>;
using UIAnchorGroupPtr = stdext::shared_object_ptr<UIAnchorGroup>;
using UIAnchorLayoutPtr = stdext::shared_object_ptr<UIAnchorLayout>;

using UIWidgetList = std::deque<UIWidgetPtr>;
using UIAnchorList = std::vector<UIAnchorPtr>;
