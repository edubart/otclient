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


#include "anchorlayout.h"

int AnchorLine::getPos() const
{
    AnchorLayoutPtr element = m_relativeElement.lock();
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
    logError("anchor line of an element have expired");
    return 0;
}

void AnchorLayout::setSize(const Size& size)
{
    m_rect.setSize(size);
    recalculateAnchors();
}

void AnchorLayout::setRect(const Rect& rect)
{
    m_rect = rect;
    recalculateAnchors();
}

void AnchorLayout::addAnchor(EAnchorType type, const AnchorLine& anchorLine)
{
    if(!anchorLine.isValid()) {
        logError("anchoring for an element has failed, got an invalid anchor line");
        return;
    }
    m_anchors[type] = anchorLine;
    anchorLine.getRelativeElement()->addAnchoredElement(asAnchorLayout());
    recalculateAnchors();
}

void AnchorLayout::addAnchoredElement(AnchorLayoutPtr anchoredElement)
{
    bool found = false;
    for(auto it = m_anchoredElements.begin(); it != m_anchoredElements.end(); ++it) {
        if((*it).lock() == anchoredElement) {
            found = true;
            break;
        }
    }
    if(!found)
        m_anchoredElements.push_back(anchoredElement);
}

void AnchorLayout::recalculateAnchors()
{
    // horizontal
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

    // vertical
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

    for(auto it = m_anchoredElements.begin(); it != m_anchoredElements.end(); ++it) {
        AnchorLayoutPtr element = (*it).lock();
        if(element)
            element->recalculateAnchors();
    }
}
