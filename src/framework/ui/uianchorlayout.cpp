#include "uianchorlayout.h"
#include "uiwidget.h"

void UIAnchorGroup::addAnchor(const UIAnchor& anchor)
{
    // duplicated anchors must be replaced
    for(UIAnchor& other : m_anchors) {
        if(other.getAnchoredEdge() == anchor.getAnchoredEdge()) {
            other = anchor;
            return;
        }
    }
    m_anchors.push_back(anchor);
}

void UIAnchorLayout::addAnchor(const UIWidgetPtr& anchoredWidget, AnchorEdge anchoredEdge,
                               const std::string& hookedWidgetId, AnchorEdge hookedEdge)
{
    if(!anchoredWidget)
        return;

    assert(anchoredWidget != getParentWidget());

    UIAnchor anchor(anchoredEdge, hookedWidgetId, hookedEdge);
    UIAnchorGroup& anchorGroup = m_anchorsGroups[anchoredWidget];
    anchorGroup.addAnchor(anchor);

    // layout must be updated because a new anchor got in
    update();
}

void UIAnchorLayout::removeAnchors(const UIWidgetPtr& anchoredWidget)
{
    m_anchorsGroups.erase(anchoredWidget);
    update();
}

void UIAnchorLayout::centerIn(const UIWidgetPtr& anchoredWidget, const std::string& hookedWidgetId)
{
    addAnchor(anchoredWidget, AnchorHorizontalCenter, hookedWidgetId, AnchorHorizontalCenter);
    addAnchor(anchoredWidget, AnchorVerticalCenter, hookedWidgetId, AnchorVerticalCenter);
}

void UIAnchorLayout::fill(const UIWidgetPtr& anchoredWidget, const std::string& hookedWidgetId)
{
    addAnchor(anchoredWidget, AnchorLeft, hookedWidgetId, AnchorLeft);
    addAnchor(anchoredWidget, AnchorRight, hookedWidgetId, AnchorRight);
    addAnchor(anchoredWidget, AnchorTop, hookedWidgetId, AnchorTop);
    addAnchor(anchoredWidget, AnchorBottom, hookedWidgetId, AnchorBottom);
}

void UIAnchorLayout::update()
{
    // reset all anchors groups update state
    for(auto& it : m_anchorsGroups) {
        UIAnchorGroup& anchorGroup = it.second;
        anchorGroup.setUpdated(false);
    }

    // update all anchors
    for(auto& it : m_anchorsGroups) {
        const UIWidgetPtr& widget = it.first;
        UIAnchorGroup& anchorGroup = it.second;
        if(!anchorGroup.isUpdated())
            updateWidget(widget, anchorGroup);
    }
}

void UIAnchorLayout::addWidget(const UIWidgetPtr& widget)
{
    update();
}

void UIAnchorLayout::removeWidget(const UIWidgetPtr& widget)
{
    removeAnchors(widget);
}

void UIAnchorLayout::updateWidget(const UIWidgetPtr& widget, UIAnchorGroup& anchorGroup)
{
    UIWidgetPtr parentWidget = getParentWidget();
    Rect newRect = widget->getRect();
    bool verticalMoved = false;
    bool horizontalMoved = false;

    // calculates new rect based on anchors
    for(const UIAnchor& anchor : anchorGroup.getAnchors()) {
        // skip invalid anchors
        if(anchor.getHookedEdge() == AnchorNone)
            continue;

        // determine hooked widget
        UIWidgetPtr hookedWidget;
        if(parentWidget) {
            if(anchor.getHookedWidgetId() == "parent")
                hookedWidget = parentWidget;
            else if(anchor.getHookedWidgetId() == "next")
                hookedWidget = parentWidget->getChildAfter(widget);
            else if(anchor.getHookedWidgetId() == "prev")
                hookedWidget = parentWidget->getChildBefore(widget);
            else
                hookedWidget = parentWidget->getChildById(anchor.getHookedWidgetId());
        }

        // skip invalid anchors
        if(!hookedWidget)
            continue;

        if(hookedWidget != getParentWidget()) {
            // update this hooked widget anchors
            auto it = m_anchorsGroups.find(hookedWidget);
            if(it != m_anchorsGroups.end()) {
                UIAnchorGroup& hookedAnchorGroup = it->second;
                if(!hookedAnchorGroup.isUpdated())
                    updateWidget(hookedWidget, hookedAnchorGroup);
            }
        }

        // determine hooked widget edge point
        int point = 0;
        switch(anchor.getHookedEdge()) {
            case AnchorLeft:
                point = hookedWidget->getRect().left();
                break;
            case AnchorRight:
                point = hookedWidget->getRect().right();
                break;
            case AnchorTop:
                point = hookedWidget->getRect().top();
                break;
            case AnchorBottom:
                point = hookedWidget->getRect().bottom();
                break;
            case AnchorHorizontalCenter:
                point = hookedWidget->getRect().horizontalCenter();
                break;
            case AnchorVerticalCenter:
                point = hookedWidget->getRect().verticalCenter();
                break;
            default:
                // must never happens
                assert(false);
                break;
        }

        switch(anchor.getAnchoredEdge()) {
            case AnchorHorizontalCenter:
                newRect.moveHorizontalCenter(point + widget->getMarginLeft() - widget->getMarginRight());
                horizontalMoved = true;
                break;
            case AnchorLeft:
                if(!horizontalMoved) {
                    newRect.moveLeft(point + widget->getMarginLeft());
                    horizontalMoved = true;
                } else
                    newRect.setLeft(point + widget->getMarginLeft());
                break;
            case AnchorRight:
                if(!horizontalMoved) {
                    newRect.moveRight(point - widget->getMarginRight());
                    horizontalMoved = true;
                } else
                    newRect.setRight(point - widget->getMarginRight());
                break;
            case AnchorVerticalCenter:
                newRect.moveVerticalCenter(point + widget->getMarginTop() - widget->getMarginBottom());
                verticalMoved = true;
                break;
            case AnchorTop:
                if(!verticalMoved) {
                    newRect.moveTop(point + widget->getMarginTop());
                    verticalMoved = true;
                } else
                    newRect.setTop(point + widget->getMarginTop());
                break;
            case AnchorBottom:
                if(!verticalMoved) {
                    newRect.moveBottom(point - widget->getMarginBottom());
                    verticalMoved = true;
                } else
                    newRect.setBottom(point - widget->getMarginBottom());
                break;
            default:
                break;
        }
    }

    widget->setRect(newRect);
    anchorGroup.setUpdated(true);
}
