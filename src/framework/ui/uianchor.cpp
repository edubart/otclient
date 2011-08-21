#include "uianchor.h"
#include "uiwidget.h"

UIAnchor::UIAnchor(AnchorEdge anchoredEdge, const std::string& hookedWidgetId, AnchorEdge hookedEdge) :
    m_anchoredEdge(anchoredEdge), m_hookedWidgetId(hookedWidgetId), m_hookedEdge(hookedEdge) {
}

int UIAnchor::getHookedPoint() const {
    UIWidgetPtr hookedWidget = getHookedWidget();

    if(hookedWidget) {
        switch(m_hookedEdge) {
            case AnchorLeft:
                return hookedWidget->getRect().left();
            case AnchorRight:
                return hookedWidget->getRect().right();
            case AnchorTop:
                return hookedWidget->getRect().top();
            case AnchorBottom:
                return hookedWidget->getRect().bottom();
            case AnchorHorizontalCenter:
                return hookedWidget->getRect().horizontalCenter();
            case AnchorVerticalCenter:
                return hookedWidget->getRect().verticalCenter();
            default:
                break;
        }
    }

    return INVALID_POINT;
}
