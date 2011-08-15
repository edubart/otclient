#ifndef FRAMEWORK_UI_DECLARATIONS_H
#define FRAMEWORK_UI_DECLARATIONS_H

#include <framework/global.h>
#include "const.h"

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

#endif
