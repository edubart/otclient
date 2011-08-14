#include "uianchor.h"
#include "uiwidget.h"

UIAnchor::UIAnchor(const UIWidgetPtr& anchoredWidget, AnchorPoint anchoredEdge, const AnchorLine& anchorLine)
    : m_anchoredWidget(anchoredWidget), m_anchoredEdge(anchoredEdge), m_anchorLine(anchorLine) {
}

UIWidgetPtr UIAnchor::getAnchorLineWidget() const {
    UIWidgetPtr anchoredWidget = m_anchoredWidget.lock();
    if(anchoredWidget && !anchoredWidget->isDestroyed())
        return anchoredWidget->backwardsGetWidgetById(m_anchorLine.widgetId);
    return nullptr;
}


UIWidgetPtr UIAnchor::getAnchoredWidget() const {
    return m_anchoredWidget.lock();
}

AnchorPoint UIAnchor::getAnchoredEdge() const {
    return m_anchoredEdge;
}

int UIAnchor::getAnchorLinePoint() const {
    UIWidgetPtr anchorLineWidget = getAnchorLineWidget();
    if(anchorLineWidget) {
        switch(m_anchorLine.edge) {
            case AnchorLeft:
                return anchorLineWidget->getGeometry().left();
            case AnchorRight:
                return anchorLineWidget->getGeometry().right();
            case AnchorTop:
                return anchorLineWidget->getGeometry().top();
            case AnchorBottom:
                return anchorLineWidget->getGeometry().bottom();
            case AnchorHorizontalCenter:
                return anchorLineWidget->getGeometry().horizontalCenter();
            case AnchorVerticalCenter:
                return anchorLineWidget->getGeometry().verticalCenter();
            default:
                break;
        }
    }
    return -9999;
}