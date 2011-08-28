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

#ifndef FRAMEWORK_CONST_H
#define FRAMEWORK_CONST_H

#include "util/color.h"

namespace Fw
{
    const Color white (0xFF, 0xFF, 0xFF, 0xFF);
    const Color black (0x00, 0x00, 0x00, 0xFF);
    const Color alpha (0x00, 0x00, 0x00, 0x00);
    const Color red   (0xFF, 0x00, 0x00, 0xFF);
    const Color green (0x00, 0xFF, 0x00, 0xFF);
    const Color blue  (0x00, 0x00, 0xFF, 0xFF);
    const Color pink  (0xFF, 0x00, 0xFF, 0xFF);
    const Color yellow(0xFF, 0xFF, 0x00, 0xFF);

    enum LogLevel {
        LogDebug = 0,
        LogInfo,
        LogWarning,
        LogError,
        LogFatal
    };

    enum BlendFunc {
        BlendNormal,
        BlendColorzing
    };

    enum AspectRatioMode {
        IgnoreAspectRatio,
        KeepAspectRatio,
        KeepAspectRatioByExpanding
    };

    enum AlignmentFlag {
        AlignNone = 0,
        AlignLeft = 1,
        AlignRight = 2,
        AlignTop = 4,
        AlignBottom = 8,
        AlignHorizontalCenter = 16,
        AlignVerticalCenter = 32,
        AlignTopLeft = AlignTop | AlignLeft,
        AlignTopRight = AlignTop | AlignRight,
        AlignBottomLeft = AlignBottom | AlignLeft,
        AlignBottomRight = AlignBottom | AlignRight,
        AlignLeftCenter = AlignLeft | AlignVerticalCenter,
        AlignRightCenter = AlignRight | AlignVerticalCenter,
        AlignTopCenter = AlignTop | AlignHorizontalCenter,
        AlignBottomCenter = AlignBottom | AlignHorizontalCenter,
        AlignCenter = AlignVerticalCenter | AlignHorizontalCenter
    };

    enum AnchorEdge {
        AnchorNone = 0,
        AnchorTop,
        AnchorBottom,
        AnchorLeft,
        AnchorRight,
        AnchorVerticalCenter,
        AnchorHorizontalCenter,
    };

    enum FocusReason {
        MouseFocusReason = 0,
        TabFocusReason,
        ActiveFocusReason,
        OtherFocusReason
    };

    enum MouseButton {
        MouseNoButton = 0,
        MouseLeftButton,
        MouseRightButton,
        MouseMidButton
    };

    enum MouseWheelDirection {
        MouseNoWheel = 0,
        MouseWheelUp,
        MouseWheelDown
    };

    enum KeyboardModifier {
        KeyboardNoModifier = 0,
        KeyboardCtrlModifier = 1,
        KeyboardAltModifier = 2,
        KeyboardShiftModifier = 4
    };

    enum WidgetState {
        DefaultState = 0,
        ActiveState = 1,
        FocusState = 2,
        HoverState = 4,
        PressedState = 8,
        DisabledState = 16
        //FirstState,
        //MiddleState,
        //LastState,
        //AlternateState
    };
}

#endif
