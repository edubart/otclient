/* The MIT License
 *
 * Copyright (c) 2010 OTClient, https://github.com/edubart/otclient
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


#include <prerequisites.h>
#include <ui/uianchorlayout.h>

UIElementPtr Anchor::getAnchorLineElement() const
{
    if(!m_anchoredElement.expired())
        return m_anchoredElement.lock()->backwardsGetElementById(m_anchorLine.getElementId());
    return UIElementPtr();
}

int Anchor::getAnchorLinePoint() const
{
    UIElementPtr anchorLineElement = getAnchorLineElement();
    if(anchorLineElement) {
        switch(m_anchorLine.getEdge()) {
            case UI::AnchorLeft:
                return anchorLineElement->getRect().left();
            case UI::AnchorRight:
                return anchorLineElement->getRect().right();
            case UI::AnchorTop:
                return anchorLineElement->getRect().top();
            case UI::AnchorBottom:
                return anchorLineElement->getRect().bottom();
            case UI::AnchorHorizontalCenter:
                return anchorLineElement->getRect().horizontalCenter();
            case UI::AnchorVerticalCenter:
                return anchorLineElement->getRect().verticalCenter();
            default:
                break;
        }
    }
    return -9999;
}

bool UIAnchorLayout::addAnchor(const UIElementPtr& anchoredElement, UI::AnchorPoint anchoredEdge, const AnchorLine& anchorLine)
{
    Anchor anchor(anchoredElement, anchoredEdge, anchorLine);
    UIElementPtr anchorLineElement = anchor.getAnchorLineElement();

    // we can never anchor with itself
    if(anchoredElement == anchorLineElement) {
        logError("ERROR: anchoring with itself is not possible");
        return false;
    }

    // TODO: check if itself is already anchored on the anchor element

    // setup the anchor
    m_anchors.push_back(anchor);

    // recalculate anchored element layout
    recalculateElementLayout(anchoredElement);
    return true;
}

void UIAnchorLayout::recalculateElementLayout(const UIElementPtr& element)
{
    Rect rect = element->getRect();
    bool verticalMoved = false;
    bool horizontalMoved = false;

    foreach(const Anchor& anchor, m_anchors) {
        if(anchor.getAnchoredElement() == element && anchor.getAnchorLineElement()) {
            int point = anchor.getAnchorLinePoint();
            switch(anchor.getAnchoredEdge()) {
                case UI::AnchorHorizontalCenter:
                    rect.moveHorizontalCenter(point + element->getMarginLeft() - element->getMarginRight());
                    horizontalMoved = true;
                    break;
                case UI::AnchorLeft:
                    if(!horizontalMoved) {
                        rect.moveLeft(point + element->getMarginLeft());
                        horizontalMoved = true;
                    } else
                        rect.setLeft(point + element->getMarginLeft());
                    break;
                case UI::AnchorRight:
                    if(!horizontalMoved) {
                        rect.moveRight(point - element->getMarginRight());
                        horizontalMoved = true;
                    } else
                        rect.setRight(point - element->getMarginRight());
                    break;
                case UI::AnchorVerticalCenter:
                    rect.moveVerticalCenter(point + element->getMarginTop() - element->getMarginBottom());
                    verticalMoved = true;
                    break;
                case UI::AnchorTop:
                    if(!verticalMoved) {
                        rect.moveTop(point + element->getMarginTop());
                        verticalMoved = true;
                    } else
                        rect.setTop(point + element->getMarginTop());
                    break;
                case UI::AnchorBottom:
                    if(!verticalMoved) {
                        rect.moveBottom(point - element->getMarginBottom());
                        verticalMoved = true;
                    } else
                        rect.setBottom(point - element->getMarginBottom());
                    break;
                default:
                    break;
            }
        }
    }

    if(rect != element->getRect())
        element->setRect(rect);
}

void UIAnchorLayout::recalculateChildrenLayout(const UIElementPtr& parent)
{
    foreach(const Anchor& anchor, m_anchors) {
        if(anchor.getAnchorLineElement() == parent) {
            UIElementPtr child = anchor.getAnchoredElement();
            if(child)
                recalculateElementLayout(child);
        }
    }
}

UI::AnchorPoint UIAnchorLayout::parseAnchorPoint(const std::string& anchorPointStr)
{
    if(anchorPointStr == "left")
        return UI::AnchorLeft;
    else if(anchorPointStr == "right")
        return UI::AnchorRight;
    else if(anchorPointStr == "top")
        return UI::AnchorTop;
    else if(anchorPointStr == "bottom")
        return UI::AnchorBottom;
    else if(anchorPointStr == "horizontalCenter")
        return UI::AnchorHorizontalCenter;
    else if(anchorPointStr == "verticalCenter")
        return UI::AnchorVerticalCenter;
    return UI::AnchorNone;
}
