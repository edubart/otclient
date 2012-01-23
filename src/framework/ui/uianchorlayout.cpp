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

void UIAnchorLayout::addAnchor(const UIWidgetPtr& anchoredWidget, Fw::AnchorEdge anchoredEdge,
                               const std::string& hookedWidgetId, Fw::AnchorEdge hookedEdge)
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
    addAnchor(anchoredWidget, Fw::AnchorHorizontalCenter, hookedWidgetId, Fw::AnchorHorizontalCenter);
    addAnchor(anchoredWidget, Fw::AnchorVerticalCenter, hookedWidgetId, Fw::AnchorVerticalCenter);
}

void UIAnchorLayout::fill(const UIWidgetPtr& anchoredWidget, const std::string& hookedWidgetId)
{
    addAnchor(anchoredWidget, Fw::AnchorLeft, hookedWidgetId, Fw::AnchorLeft);
    addAnchor(anchoredWidget, Fw::AnchorRight, hookedWidgetId, Fw::AnchorRight);
    addAnchor(anchoredWidget, Fw::AnchorTop, hookedWidgetId, Fw::AnchorTop);
    addAnchor(anchoredWidget, Fw::AnchorBottom, hookedWidgetId, Fw::AnchorBottom);
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
    if(!parentWidget)
        return;

    Rect newRect = widget->getRect();
    bool verticalMoved = false;
    bool horizontalMoved = false;

    // calculates new rect based on anchors
    for(const UIAnchor& anchor : anchorGroup.getAnchors()) {
        // skip invalid anchors
        if(anchor.getHookedEdge() == Fw::AnchorNone)
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
        Rect hookedWidgetRect = hookedWidget->getRect();
        if(hookedWidget == parentWidget)
            hookedWidgetRect = parentWidget->getChildrenRect();

        int point = 0;
        switch(anchor.getHookedEdge()) {
            case Fw::AnchorLeft:
                point = hookedWidgetRect.left();
                break;
            case Fw::AnchorRight:
                point = hookedWidgetRect.right();
                break;
            case Fw::AnchorTop:
                point = hookedWidgetRect.top();
                break;
            case Fw::AnchorBottom:
                point = hookedWidgetRect.bottom();
                break;
            case Fw::AnchorHorizontalCenter:
                point = hookedWidgetRect.horizontalCenter();
                break;
            case Fw::AnchorVerticalCenter:
                point = hookedWidgetRect.verticalCenter();
                break;
            default:
                // must never happens
                assert(false);
                break;
        }

        switch(anchor.getAnchoredEdge()) {
            case Fw::AnchorHorizontalCenter:
                newRect.moveHorizontalCenter(point + widget->getMarginLeft() - widget->getMarginRight());
                horizontalMoved = true;
                break;
            case Fw::AnchorLeft:
                if(!horizontalMoved) {
                    newRect.moveLeft(point + widget->getMarginLeft());
                    horizontalMoved = true;
                } else
                    newRect.setLeft(point + widget->getMarginLeft());
                break;
            case Fw::AnchorRight:
                if(!horizontalMoved) {
                    newRect.moveRight(point - widget->getMarginRight());
                    horizontalMoved = true;
                } else
                    newRect.setRight(point - widget->getMarginRight());
                break;
            case Fw::AnchorVerticalCenter:
                newRect.moveVerticalCenter(point + widget->getMarginTop() - widget->getMarginBottom());
                verticalMoved = true;
                break;
            case Fw::AnchorTop:
                if(!verticalMoved) {
                    newRect.moveTop(point + widget->getMarginTop());
                    verticalMoved = true;
                } else
                    newRect.setTop(point + widget->getMarginTop());
                break;
            case Fw::AnchorBottom:
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

void UIAnchorLayout::internalUpdate()
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
