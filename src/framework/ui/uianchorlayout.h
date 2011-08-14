#ifndef UIANCHORLAYOUT_H
#define UIANCHORLAYOUT_H

#include "uilayout.h"
#include "uianchor.h"

class UIAnchorLayout : public UILayout
{
public:
    bool addAnchor(const UIWidgetPtr& anchoredWidget, AnchorPoint anchoredEdge, const AnchorLine& anchorLine);
    void updateWidget(const UIWidgetPtr& widget);
    void updateWidgetChildren(const UIWidgetPtr& parent);

    bool hasWidgetInAnchorTree(const UIWidgetPtr& widget, const UIWidgetPtr& treeAnchor);

private:
    std::list<UIAnchor> m_anchors;
};

#endif
