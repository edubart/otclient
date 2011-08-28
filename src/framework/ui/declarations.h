#ifndef FRAMEWORK_UI_DECLARATIONS_H
#define FRAMEWORK_UI_DECLARATIONS_H

#include <framework/global.h>
#include <framework/platform/platformevent.h>

class UIManager;
class UIWidget;
class UILabel;
class UIButton;
class UILineEdit;
class UIWindow;
class UILayout;
class UIVerticalLayout;
class UIAnchorLayout;

typedef std::shared_ptr<UIWidget> UIWidgetPtr;
typedef std::weak_ptr<UIWidget> UIWidgetWeakPtr;

typedef std::shared_ptr<UILabel> UILabelPtr;
typedef std::shared_ptr<UIButton> UIButtonPtr;
typedef std::shared_ptr<UILineEdit> UILineEditPtr;
typedef std::shared_ptr<UIWindow> UIWindowPtr;
typedef std::shared_ptr<UILayout> UILayoutPtr;
typedef std::shared_ptr<UIVerticalLayout> UIVerticalLayoutPtr;
typedef std::shared_ptr<UIAnchorLayout> UIAnchorLayoutPtr;

typedef std::deque<UIWidgetPtr> UIWidgetList;

#endif
