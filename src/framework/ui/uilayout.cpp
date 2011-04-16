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


#include "uilayout.h"
#include "uielement.h"

int AnchorLine::getPos() const
{
    UILayoutPtr element = m_relativeElement.lock();
    if(element) {
        switch(m_anchorType) {
            case ANCHOR_LEFT:
                return element->getRect().left();
            case ANCHOR_RIGHT:
                return element->getRect().right();
            case ANCHOR_TOP:
                return element->getRect().top();
            case ANCHOR_BOTTOM:
                return element->getRect().bottom();
            case ANCHOR_HORIZONTAL_CENTER:
                return element->getRect().horizontalCenter();
            case ANCHOR_VERTICAL_CENTER:
                return element->getRect().verticalCenter();
            default:
                return 0;
        }
    }
    logError("anchor line of an element has expired, your UI is missconfigured");
    return 0;
}

void UILayout::setSize(const Size& size)
{
    if(m_rect.isValid())
        m_rect.setSize(size);
    else
        m_rect = Rect(0, 0, size);

    // rect updated, recalculate itself and anchored elements positions
    recalculateLayout();
}

void UILayout::setRect(const Rect& rect)
{
    m_rect = rect;

    // rect updated, recalculate itself and anchored elements positions
    recalculateAnchoredLayout();
}

bool UILayout::addAnchor(EAnchorType type, const AnchorLine& anchorLine)
{
    // we can never anchor with itself
    if(anchorLine.getRelativeElement() == asUILayout()) {
        logError("anchoring with itself is not possible");
        return false;
    }

    // check if this layout is already anchored with the relative element
    // this only happens in missconfigurations
    for(auto it = m_anchoredElements.begin(); it != m_anchoredElements.end(); ++it) {
        if((*it).lock() == anchorLine.getRelativeElement()) {
            logError("anchoring elements with each other is not possible");
            return false;
        }
    }

    // setup the anchor
    m_anchors[type] = anchorLine;
    anchorLine.getRelativeElement()->addAnchoredElement(asUILayout());

    // recalculate itself and anchored elements
    recalculateLayout();
    return true;
}

void UILayout::addAnchoredElement(UILayoutPtr anchoredElement)
{
    // check if is already anchored
    bool found = false;
    for(auto it = m_anchoredElements.begin(); it != m_anchoredElements.end(); ++it) {
        if((*it).lock() == anchoredElement) {
            found = true;
            break;
        }
    }

    // if not anchor it
    if(!found)
        m_anchoredElements.push_back(anchoredElement);
}

void UILayout::recalculateLayout()
{
    // recalculate horizontal position
    if(m_anchors[ANCHOR_HORIZONTAL_CENTER].isValid()) {
        m_rect.moveHorizontalCenter(m_anchors[ANCHOR_HORIZONTAL_CENTER].getPos() + m_marginLeft - m_marginRight);
    } else {
        if(m_anchors[ANCHOR_LEFT].isValid() && m_anchors[ANCHOR_RIGHT].isValid()) {
            m_rect.setLeft(m_anchors[ANCHOR_LEFT].getPos() + m_marginLeft);
            m_rect.setRight(m_anchors[ANCHOR_RIGHT].getPos() - m_marginRight);
        } else if(m_anchors[ANCHOR_LEFT].isValid()) {
            m_rect.moveLeft(m_anchors[ANCHOR_LEFT].getPos() + m_marginLeft);
        } else if(m_anchors[ANCHOR_RIGHT].isValid()) {
            m_rect.moveRight(m_anchors[ANCHOR_RIGHT].getPos() - m_marginRight);
        }
    }

    // recalculate vertical position
    if(m_anchors[ANCHOR_VERTICAL_CENTER].isValid()) {
        m_rect.moveVerticalCenter(m_anchors[ANCHOR_VERTICAL_CENTER].getPos() + m_marginTop - m_marginBottom);
    } else {
        if(m_anchors[ANCHOR_TOP].isValid() && m_anchors[ANCHOR_BOTTOM].isValid()) {
            m_rect.setLeft(m_anchors[ANCHOR_TOP].getPos() + m_marginTop);
            m_rect.setRight(m_anchors[ANCHOR_BOTTOM].getPos() - m_marginBottom);
        } else if(m_anchors[ANCHOR_TOP].isValid()) {
            m_rect.moveTop(m_anchors[ANCHOR_TOP].getPos() + m_marginTop);
        } else if(m_anchors[ANCHOR_BOTTOM].isValid()) {
            m_rect.moveBottom(m_anchors[ANCHOR_BOTTOM].getPos() - m_marginBottom);
        }
    }

    recalculateAnchoredLayout();

    // fire layout update event
    onLayoutRectChange(m_rect);
}

void UILayout::recalculateAnchoredLayout()
{
    // recalculate anchored elements positions
    for(auto it = m_anchoredElements.begin(); it != m_anchoredElements.end(); ++it) {
        UILayoutPtr element = (*it).lock();
        if(element)
            element->recalculateLayout();
    }
}

