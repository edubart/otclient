#ifndef FRAMEWORK_CONST_H
#define FRAMEWORK_CONST_H

//namespace fw {

enum LogLevel {
    LogDebug = 0,
    LogInfo,
    LogWarning,
    LogError,
    LogFatal
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

//}

#endif
