
#ifndef SCRIPTFUNCTIONS_H
#define SCRIPTFUNCTIONS_H

#include <global.h>
#include "scriptobject.h"

void registerScriptFunctions();

// App
int lua_App_exit();

// UI
int lua_UI_load();
int lua_UI_getRootContainer();

// UIElement
int lua_UIElement_getId();
int lua_UIElement_setId();
int lua_UIElement_isEnabled();
int lua_UIElement_setEnabled();
int lua_UIElement_isVisible();
int lua_UIElement_setVisible();
int lua_UIElement_isFocused();
int lua_UIElement_setFocused();
int lua_UIElement_getParent();
int lua_UIElement_setParent();
int lua_UIElement_setLocked();
int lua_UIElement_destroy();
void lua_UIElement_onLoad();
void lua_UIElement_onDestroy();

// UIContainer
int lua_UIContainer_getChild();
int lua_UIContainer_getChildren();

// UILabel
int lua_UILabel_setText();
int lua_UILabel_getText();

// UITextEdit
int lua_UITextEdit_setText();
int lua_UITextEdit_getText();

// UIButton
void lua_UIButton_onClick();

// UIWindow
int lua_UIWindow_setTitle();
int lua_UIWindow_getTitle();

#endif // SCRIPTFUNCTIONS_H
