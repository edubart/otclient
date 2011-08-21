#ifndef UIANCHOR_H
#define UIANCHOR_H

#include "declarations.h"

class UIAnchor
{
public:
    enum {
        INVALID_POINT = -999999
    };

    UIAnchor(AnchorEdge anchoredEdge, const std::string& hookedWidgetId, AnchorEdge hookedEdge);

    AnchorEdge getAnchoredEdge() const { return m_anchoredEdge; }
    UIWidgetPtr getHookedWidget() const { return m_hookedWidget.lock(); }
    std::string getHookedWidgetId() const { return m_hookedWidgetId; }
    int getHookedPoint() const;

    void setHookedWidget(const UIWidgetPtr hookedWidget) { m_hookedWidget = hookedWidget; }

private:
    AnchorEdge m_anchoredEdge;
    UIWidgetWeakPtr m_hookedWidget;
    std::string m_hookedWidgetId;
    AnchorEdge m_hookedEdge;
};

#endif
