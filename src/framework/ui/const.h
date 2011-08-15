#ifndef FRAMEWORK_UI_CONST_H
#define FRAMEWORK_UI_CONST_H

// namespace ui {

enum UIWidgetType {
    UITypeWidget = 0,
    UITypeLabel,
    UITypeButton,
    UITypeLineEdit,
    UITypeWindow,
    UITypeList
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

enum ButtonState {
    ButtonUp = 0,
    ButtonDown,
    ButtonHover
};

// }

#endif
