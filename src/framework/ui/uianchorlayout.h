#ifndef UIANCHORLAYOUT_H
#define UIANCHORLAYOUT_H

#include "uilayout.h"

class UIAnchor
{
public:
    UIAnchor(AnchorEdge anchoredEdge, const std::string& hookedWidgetId, AnchorEdge hookedEdge) :
        m_anchoredEdge(anchoredEdge), m_hookedEdge(hookedEdge), m_hookedWidgetId(hookedWidgetId) { }

    AnchorEdge getAnchoredEdge() const { return m_anchoredEdge; }
    std::string getHookedWidgetId() const { return m_hookedWidgetId; }
    AnchorEdge getHookedEdge() const { return m_hookedEdge; }

private:
    AnchorEdge m_anchoredEdge;
    AnchorEdge m_hookedEdge;
    std::string m_hookedWidgetId;
};

class UIAnchorGroup
{
public:
    UIAnchorGroup() : m_updated(true) { }

    void addAnchor(const UIAnchor& anchor);
    const std::vector<UIAnchor>& getAnchors() const { return m_anchors; }
    bool isUpdated() const { return m_updated; }
    void setUpdated(bool updated) { m_updated = updated; }

private:
    std::vector<UIAnchor> m_anchors;
    bool m_updated;
};

class UIAnchorLayout : public UILayout
{
public:
    UIAnchorLayout(UIWidgetPtr parentWidget) : UILayout(parentWidget) { }

    void addAnchor(const UIWidgetPtr& anchoredWidget, AnchorEdge anchoredEdge,
                   const std::string& hookedWidgetId, AnchorEdge hookedEdge);
    void removeAnchors(const UIWidgetPtr& anchoredWidget);
    void centerIn(const UIWidgetPtr& anchoredWidget, const std::string& hookedWidgetId);
    void fill(const UIWidgetPtr& anchoredWidget, const std::string& hookedWidgetId);

    void update();
    void addWidget(const UIWidgetPtr& widget);
    void removeWidget(const UIWidgetPtr& widget);

    UIAnchorLayoutPtr asUIAnchorLayout() { return std::static_pointer_cast<UIAnchorLayout>(shared_from_this()); }

private:
    void updateWidget(const UIWidgetPtr& widget, UIAnchorGroup& anchorGroup);
    std::map<UIWidgetPtr, UIAnchorGroup> m_anchorsGroups;
};

#endif
