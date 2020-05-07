/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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

#ifndef UILAYOUT_H
#define UILAYOUT_H

#include "declarations.h"
#include <framework/luaengine/luaobject.h>
#include <framework/otml/otml.h>

// @bindclass
class UILayout : public LuaObject
{
public:
    UILayout(UIWidgetPtr parentWidget) : m_parentWidget(parentWidget) { m_updateDisabled = 0; }

    void update();
    void updateLater();

    virtual void applyStyle(const OTMLNodePtr& /*styleNode*/) { }
    virtual void addWidget(const UIWidgetPtr& /*widget*/) { }
    virtual void removeWidget(const UIWidgetPtr& /*widget*/) { }
    void disableUpdates() { m_updateDisabled++; }
    void enableUpdates() { m_updateDisabled = std::max<int>(m_updateDisabled-1,0); }

    void setParent(UIWidgetPtr parentWidget) { m_parentWidget = parentWidget; }
    UIWidgetPtr getParentWidget() { return m_parentWidget; }

    bool isUpdateDisabled() { return m_updateDisabled > 0; }
    bool isUpdating() { return m_updating; }

    virtual bool isUIAnchorLayout() { return false; }
    virtual bool isUIBoxLayout() { return false; }
    virtual bool isUIHorizontalLayout() { return false; }
    virtual bool isUIVerticalLayout() { return false; }
    virtual bool isUIGridLayout() { return false; }

protected:
    virtual bool internalUpdate() { return false; }

    int m_updateDisabled;
    stdext::boolean<false> m_updating;
    stdext::boolean<false> m_updateScheduled;
    UIWidgetPtr m_parentWidget;
};

#endif
