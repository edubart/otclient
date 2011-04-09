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


#ifndef ANCHORLAYOUT_H
#define ANCHORLAYOUT_H

#include "../prerequisites.h"
#include "../rect.h"
#include "uiconstants.h"

enum EAnchorType {
    ANCHOR_LEFT = 0,
    ANCHOR_RIGHT,
    ANCHOR_TOP,
    ANCHOR_BOTTOM,
    ANCHOR_HORIZONTAL_CENTER,
    ANCHOR_VERTICAL_CENTER,
    ANCHOR_NONE
};

class AnchorLayout;
typedef std::shared_ptr<AnchorLayout> AnchorLayoutPtr;
typedef std::weak_ptr<AnchorLayout> AnchorLayoutWeakPtr;

class AnchorLine
{
public:
    AnchorLine() : m_anchorType(ANCHOR_NONE) { }
    AnchorLine(const AnchorLine& other) :
        m_relativeElement(other.m_relativeElement), m_anchorType(other.m_anchorType) { }
    AnchorLine(AnchorLayoutPtr relativeElement, EAnchorType anchorType) :
        m_relativeElement(relativeElement), m_anchorType(anchorType) { }
    bool isValid() const { return (m_anchorType != ANCHOR_NONE && !m_relativeElement.expired()); }

    int getPos() const;
    EAnchorType getAnchorType() const { return m_anchorType; }
    AnchorLayoutPtr getRelativeElement() const { return m_relativeElement.lock(); }

private:
    AnchorLayoutWeakPtr m_relativeElement;
    EAnchorType m_anchorType;
};

class AnchorLayout : public std::enable_shared_from_this<AnchorLayout>
{
public:
    AnchorLayout() :
        m_marginLeft(0),
        m_marginRight(0),
        m_marginTop(0),
        m_marginBottom(0) { }
    virtual ~AnchorLayout() { }

    void setSize(const Size& size);
    Size getSize() { return m_rect.size(); }

    void setRect(const Rect& rect);
    const Rect& getRect() const{ return m_rect; }

    void addAnchor(EAnchorType type, const AnchorLine& anchorLine);
    void anchorLeft(const AnchorLine& anchorLine) { addAnchor(ANCHOR_LEFT, anchorLine); }
    void anchorRight(const AnchorLine& anchorLine) { addAnchor(ANCHOR_RIGHT, anchorLine); }
    void anchorTop(const AnchorLine& anchorLine) { addAnchor(ANCHOR_TOP, anchorLine); }
    void anchorBottom(const AnchorLine& anchorLine) { addAnchor(ANCHOR_BOTTOM, anchorLine); }
    void anchorHorizontalCenter(const AnchorLine& anchorLine) { addAnchor(ANCHOR_HORIZONTAL_CENTER, anchorLine); }
    void anchorVerticalCenter(const AnchorLine& anchorLine) { addAnchor(ANCHOR_VERTICAL_CENTER, anchorLine); }

    AnchorLine left() { return AnchorLine(asAnchorLayout(), ANCHOR_LEFT); }
    AnchorLine right() { return AnchorLine(asAnchorLayout(), ANCHOR_RIGHT); }
    AnchorLine top() { return AnchorLine(asAnchorLayout(), ANCHOR_TOP); }
    AnchorLine bottom() { return AnchorLine(asAnchorLayout(), ANCHOR_BOTTOM); }
    AnchorLine horizontalCenter() { return AnchorLine(asAnchorLayout(), ANCHOR_HORIZONTAL_CENTER); }
    AnchorLine verticalCenter() { return AnchorLine(asAnchorLayout(), ANCHOR_VERTICAL_CENTER); }

    void setMargin(int top, int left, int bottom, int right) { m_marginLeft = left; m_marginRight = right; m_marginTop = top; m_marginBottom = bottom; recalculateAnchors(); }
    void setMargin(int horizontal, int vertical) { m_marginLeft = m_marginRight = horizontal; m_marginTop = m_marginBottom = vertical; recalculateAnchors(); }
    void setMargin(int margin) { m_marginLeft = m_marginRight = m_marginTop = m_marginBottom = margin; recalculateAnchors(); }

    AnchorLayoutPtr asAnchorLayout() { return shared_from_this(); }

private:
    void recalculateAnchors();
    void addAnchoredElement(AnchorLayoutPtr anchoredElement);

    AnchorLine m_anchors[6];

    Rect m_rect;
    int m_marginLeft;
    int m_marginRight;
    int m_marginTop;
    int m_marginBottom;
    std::list<AnchorLayoutWeakPtr> m_anchoredElements;
};

#endif // ANCHORLAYOUT_H
