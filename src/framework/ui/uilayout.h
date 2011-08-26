#ifndef UILAYOUT_H
#define UILAYOUT_H

#include "declarations.h"
#include <framework/luascript/luaobject.h>

class UILayout : public LuaObject
{
public:
    UILayout(UIWidgetPtr parentWidget) : m_parentWidget(parentWidget) { }

    virtual void update() = 0;
    virtual void addWidget(const UIWidgetPtr& widget) = 0;
    virtual void removeWidget(const UIWidgetPtr& widget) = 0;

    UIWidgetPtr getParentWidget() { return m_parentWidget.lock(); }

    virtual UIAnchorLayoutPtr asUIAnchorLayout() { return nullptr; }

protected:
    UIWidgetWeakPtr m_parentWidget;
};

#endif
