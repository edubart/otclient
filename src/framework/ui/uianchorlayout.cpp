#include "uianchorlayout.h"
#include "uiwidget.h"

bool UIAnchorLayout::addAnchor(const UIWidgetPtr& anchoredWidget, AnchorPoint anchoredEdge, const AnchorLine& anchorLine)
{
    UIAnchor anchor(anchoredWidget, anchoredEdge, anchorLine);
    UIWidgetPtr anchorLineWidget = anchor.getAnchorLineWidget();

    /*
    if(!anchorLineWidget) {
        logError("ERROR: could not find the widget to anchor on, wrong id?");
        return false;
    }
    */

    // we can never anchor with itself
    if(anchoredWidget == anchorLineWidget) {
        logError("ERROR: anchoring with itself is not possible");
        return false;
    }

    // we must never anchor to an anchor child
    if(anchoredWidget && hasWidgetInAnchorTree(anchorLineWidget, anchoredWidget)) {
        logError("ERROR: anchors is miss configured, you must never make an anchor chains in loops");
        return false;
    }

    // setup the anchor
    m_anchors.push_back(anchor);

    // recalculate anchored widget layout
    updateWidget(anchoredWidget);
    return true;
}

void UIAnchorLayout::updateWidget(const UIWidgetPtr& widget)
{
    Rect rect = widget->getGeometry();
    bool verticalMoved = false;
    bool horizontalMoved = false;

    // TODO: remove expired anchors
    for(const UIAnchor& anchor : m_anchors) {
        if(anchor.getAnchoredWidget() == widget && anchor.getAnchorLineWidget()) {
            int point = anchor.getAnchorLinePoint();
            switch(anchor.getAnchoredEdge()) {
                case AnchorHorizontalCenter:
                    rect.moveHorizontalCenter(point + widget->getMarginLeft() - widget->getMarginRight());
                    horizontalMoved = true;
                    break;
                case AnchorLeft:
                    if(!horizontalMoved) {
                        rect.moveLeft(point + widget->getMarginLeft());
                        horizontalMoved = true;
                    } else
                        rect.setLeft(point + widget->getMarginLeft());
                    break;
                case AnchorRight:
                    if(!horizontalMoved) {
                        rect.moveRight(point - widget->getMarginRight());
                        horizontalMoved = true;
                    } else
                        rect.setRight(point - widget->getMarginRight());
                    break;
                case AnchorVerticalCenter:
                    rect.moveVerticalCenter(point + widget->getMarginTop() - widget->getMarginBottom());
                    verticalMoved = true;
                    break;
                case AnchorTop:
                    if(!verticalMoved) {
                        rect.moveTop(point + widget->getMarginTop());
                        verticalMoved = true;
                    } else
                        rect.setTop(point + widget->getMarginTop());
                    break;
                case AnchorBottom:
                    if(!verticalMoved) {
                        rect.moveBottom(point - widget->getMarginBottom());
                        verticalMoved = true;
                    } else
                        rect.setBottom(point - widget->getMarginBottom());
                    break;
                default:
                    break;
            }
        }
    }

    if(rect != widget->getGeometry())
        widget->setGeometry(rect);
}

void UIAnchorLayout::updateWidgetChildren(const UIWidgetPtr& parent)
{
    for(const UIAnchor& anchor : m_anchors) {
        if(anchor.getAnchorLineWidget() == parent) {
            UIWidgetPtr child = anchor.getAnchoredWidget();
            if(child)
                updateWidget(child);
        }
    }
}

bool UIAnchorLayout::hasWidgetInAnchorTree(const UIWidgetPtr& widget, const UIWidgetPtr& treeAnchor)
{
    for(const UIAnchor& anchor : m_anchors) {
        if(anchor.getAnchorLineWidget() == treeAnchor) {
            if(anchor.getAnchoredWidget() == widget || hasWidgetInAnchorTree(widget, anchor.getAnchoredWidget()))
                return true;
        }
    }
    return false;
}

