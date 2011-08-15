#ifndef UIDECLARATIONS_H
#define UIDECLARATIONS_H

#include <global.h>

class UIManager;
class UILayout;
class UIAnchorLayout;
class UIStyle;
class UIWidget;
class UILabel;
class UIButton;
class UILineEdit;
class UIWindow;

typedef std::shared_ptr<UIWidget> UIWidgetPtr;
typedef std::weak_ptr<UIWidget> UIWidgetWeakPtr;
typedef std::deque<UIWidgetPtr> UIWidgetList;

typedef std::shared_ptr<UILayout> UILayoutPtr;
typedef std::shared_ptr<UIAnchorLayout> UIAnchorLayoutPtr;
typedef std::shared_ptr<UIStyle> UIStylePtr;
typedef std::shared_ptr<UILabel> UILabelPtr;
typedef std::shared_ptr<UIButton> UIButtonPtr;
typedef std::shared_ptr<UILineEdit> UILineEditPtr;
typedef std::shared_ptr<UIWindow> UIWindowPtr;

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

#endif