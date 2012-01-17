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

#ifndef FRAMEWORK_CONST_H
#define FRAMEWORK_CONST_H

#include "util/types.h"

#define DEG_TO_RAD (acos(-1)/180.0)
#define RAD_TO_DEC (180.0/acos(-1))

namespace Fw
{
    static const double pi = 3.14159265;

    // NOTE: AABBGGRR order
    enum GlobalColor : uint32 {
        alpha      = 0x00000000,
        white      = 0xffffffff,
        black      = 0xff000000,
        red        = 0xff0000ff,
        darkRed    = 0xff000080,
        green      = 0xff00ff00,
        darkGreen  = 0xff008000,
        blue       = 0xffff0000,
        darkBlue   = 0xff800000,
        pink       = 0xffff00ff,
        darkPink   = 0xff800080,
        yellow     = 0xff00ffff,
        darkYellow = 0xff008080,
        teal       = 0xffffff00,
        darkTeal   = 0xff808000,
        gray       = 0xffa0a0a0,
        darkGray   = 0xff808080,
        lightGray  = 0xffc0c0c0,
        orange     = 0xffff8c00
    };

    enum Key : uint8 {
        KeyUnknown = 0,
        KeyEscape = 1,
        KeyTab = 2,
        KeyBackspace = 3,
        KeyReturn = 4,
        KeyEnter = 5,
        KeyInsert = 6,
        KeyDelete = 7,
        KeyPause = 8,
        KeyPrintScreen = 9,
        KeyHome = 10,
        KeyEnd = 11,
        KeyPageUp = 12,
        KeyPageDown = 13,
        KeyUp = 14,
        KeyDown = 15,
        KeyLeft = 16,
        KeyRight = 17,
        KeyNumLock = 18,
        KeyScrollLock = 19,
        KeyCapsLock = 20,
        KeyCtrl = 21,
        KeyShift = 22,
        KeyAlt = 23,
        KeyAltGr = 24,
        KeyMeta = 25,
        KeyMenu = 26,
        KeySpace = 32,        // ' '
        KeyExclamation = 33,  // !
        KeyQuote = 34,        // "
        KeyNumberSign = 35,   // #
        KeyDollar = 36,       // $
        KeyPercent = 37,      // %
        KeyAmpersand = 38,    // &
        KeyApostrophe = 39,   // '
        KeyLeftParen = 40,    // (
        KeyRightParen = 41,   // )
        KeyAsterisk = 42,     // *
        KeyPlus = 43,         // +
        KeyComma = 44,        // ,
        KeyMinus = 45,        // -
        KeyPeriod = 46,       // .
        KeySlash = 47,        // /
        Key0 = 48,            // 0
        Key1 = 49,            // 1
        Key2 = 50,            // 2
        Key3 = 51,            // 3
        Key4 = 52,            // 4
        Key5 = 53,            // 5
        Key6 = 54,            // 6
        Key7 = 55,            // 7
        Key8 = 56,            // 8
        Key9 = 57,            // 9
        KeyColon = 58,        // :
        KeySemicolon = 59,    // ;
        KeyLess = 60,         // <
        KeyEqual = 61,        // =
        KeyGreater = 62,      // >
        KeyQuestion = 63,     // ?
        KeyAtSign = 64,       // @
        KeyA = 65,            // a
        KeyB = 66,            // b
        KeyC = 67,            // c
        KeyD = 68,            // d
        KeyE = 69,            // e
        KeyF = 70,            // f
        KeyG = 71,            // g
        KeyH = 72,            // h
        KeyI = 73,            // i
        KeyJ = 74,            // j
        KeyK = 75,            // k
        KeyL = 76,            // l
        KeyM = 77,            // m
        KeyN = 78,            // n
        KeyO = 79,            // o
        KeyP = 80,            // p
        KeyQ = 81,            // q
        KeyR = 82,            // r
        KeyS = 83,            // s
        KeyT = 84,            // t
        KeyU = 85,            // u
        KeyV = 86,            // v
        KeyW = 87,            // w
        KeyX = 88,            // x
        KeyY = 89,            // y
        KeyZ = 90,            // z
        KeyLeftBracket = 91,  // [
        KeyBackslash = 92,    // '\'
        KeyRightBracket = 93, // ]
        KeyCaret = 94,        // ^
        KeyUnderscore = 95,   // _
        KeyGrave = 96,        // `
        KeyLeftCurly = 123,   // {
        KeyBar = 124,         // |
        KeyRightCurly = 125,  // }
        KeyTilde = 126,       // ~
        KeyF1 = 128,
        KeyF2 = 129,
        KeyF3 = 130,
        KeyF4 = 131,
        KeyF5 = 132,
        KeyF6 = 134,
        KeyF7 = 135,
        KeyF8 = 136,
        KeyF9 = 137,
        KeyF10 = 138,
        KeyF11 = 139,
        KeyF12 = 140,
        KeyNumpad0 = 141,
        KeyNumpad1 = 142,
        KeyNumpad2 = 143,
        KeyNumpad3 = 144,
        KeyNumpad4 = 145,
        KeyNumpad5 = 146,
        KeyNumpad6 = 147,
        KeyNumpad7 = 148,
        KeyNumpad8 = 149,
        KeyNumpad9 = 150
    };

    enum LogLevel {
        LogDebug = 0,
        LogInfo,
        LogWarning,
        LogError,
        LogFatal
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
        AlignTopLeft = AlignTop | AlignLeft, // 5
        AlignTopRight = AlignTop | AlignRight, // 6
        AlignBottomLeft = AlignBottom | AlignLeft, // 9
        AlignBottomRight = AlignBottom | AlignRight, // 10
        AlignLeftCenter = AlignLeft | AlignVerticalCenter, // 33
        AlignRightCenter = AlignRight | AlignVerticalCenter, // 34
        AlignTopCenter = AlignTop | AlignHorizontalCenter, // 20
        AlignBottomCenter = AlignBottom | AlignHorizontalCenter, // 24
        AlignCenter = AlignVerticalCenter | AlignHorizontalCenter // 48
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

    enum InputEventType {
        NoInputEvent = 0,
        KeyTextInputEvent,
        KeyDownInputEvent,
        KeyPressInputEvent,
        KeyUpInputEvent,
        MousePressInputEvent,
        MouseReleaseInputEvent,
        MouseMoveInputEvent,
        MouseWheelInputEvent
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
        InvalidState = -1,
        DefaultState = 0,
        ActiveState = 1,
        FocusState = 2,
        HoverState = 4,
        PressedState = 8,
        DisabledState = 16,
        CheckedState = 32,
        OnState = 64,
        FirstState = 128,
        MiddleState = 256,
        LastState = 512,
        AlternateState = 1024,
        LastWidgetState = 2048,
    };

    enum AppicationFlags {
        AppEnableModules = 1,
        AppEnableGraphics = 2,
        AppEnableConfigs = 4,
        AppEnableAll = AppEnableModules | AppEnableGraphics | AppEnableConfigs
    };
}

#endif
