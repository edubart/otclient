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

#ifndef UIANCHORLAYOUT_H
#define UIANCHORLAYOUT_H

#include "uilayout.h"

class UIAnchor
{
public:
    UIAnchor(Fw::AnchorEdge anchoredEdge, const std::string& hookedWidgetId, Fw::AnchorEdge hookedEdge) :
        m_anchoredEdge(anchoredEdge), m_hookedEdge(hookedEdge), m_hookedWidgetId(hookedWidgetId) { }

    Fw::AnchorEdge getAnchoredEdge() const { return m_anchoredEdge; }
    std::string getHookedWidgetId() const { return m_hookedWidgetId; }
    Fw::AnchorEdge getHookedEdge() const { return m_hookedEdge; }

private:
    Fw::AnchorEdge m_anchoredEdge;
    Fw::AnchorEdge m_hookedEdge;
    std::string m_hookedWidgetId;
};

typedef std::vector<UIAnchor> UIAnchorList;

class UIAnchorGroup
{
public:
    UIAnchorGroup() : m_updated(true) { }

    void addAnchor(const UIAnchor& anchor);
    const UIAnchorList& getAnchors() { return m_anchors; }
    bool isUpdated() { return m_updated; }
    void setUpdated(bool updated) { m_updated = updated; }

private:
    UIAnchorList m_anchors;
    bool m_updated;
};

// @bindclass
class UIAnchorLayout : public UILayout
{
public:
    UIAnchorLayout(UIWidgetPtr parentWidget) : UILayout(parentWidget) { }

    void addAnchor(const UIWidgetPtr& anchoredWidget, Fw::AnchorEdge anchoredEdge,
                   const std::string& hookedWidgetId, Fw::AnchorEdge hookedEdge);
    void removeAnchors(const UIWidgetPtr& anchoredWidget);
    bool hasAnchors(const UIWidgetPtr& anchoredWidget);
    void centerIn(const UIWidgetPtr& anchoredWidget, const std::string& hookedWidgetId);
    void fill(const UIWidgetPtr& anchoredWidget, const std::string& hookedWidgetId);

    void addWidget(const UIWidgetPtr& widget);
    void removeWidget(const UIWidgetPtr& widget);

    UIAnchorLayoutPtr asUIAnchorLayout() { return std::static_pointer_cast<UIAnchorLayout>(shared_from_this()); }

protected:
    bool internalUpdate();

private:
    bool updateWidget(const UIWidgetPtr& widget, UIAnchorGroup& anchorGroup, UIWidgetPtr first = nullptr);
    std::map<UIWidgetPtr, UIAnchorGroup> m_anchorsGroups;
};

#endif
