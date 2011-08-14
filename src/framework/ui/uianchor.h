#ifndef UIANCHOR_H
#define UIANCHOR_H

#include "uideclarations.h"

struct AnchorLine {
    AnchorLine(std::string widgetId, AnchorPoint edge) : widgetId(widgetId), edge(edge) { }
    std::string widgetId;
    AnchorPoint edge;
};

class UIAnchor
{
public:
    UIAnchor(const UIWidgetPtr& anchoredWidget, AnchorPoint anchoredEdge, const AnchorLine& anchorLine);
    UIWidgetPtr getAnchorLineWidget() const;
    UIWidgetPtr getAnchoredWidget() const;
    AnchorPoint getAnchoredEdge() const;
    int getAnchorLinePoint() const;

private:
    UIWidgetWeakPtr m_anchoredWidget;
    AnchorPoint m_anchoredEdge;
    AnchorLine m_anchorLine;
};

#endif
