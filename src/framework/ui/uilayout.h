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


#ifndef UILAYOUT_H
#define UILAYOUT_H

#include <prerequisites.h>
#include <ui/uiconstants.h>
#include <script/scriptable.h>

enum EAnchorType {
    ANCHOR_LEFT = 0,
    ANCHOR_RIGHT,
    ANCHOR_TOP,
    ANCHOR_BOTTOM,
    ANCHOR_HORIZONTAL_CENTER,
    ANCHOR_VERTICAL_CENTER,
    ANCHOR_NONE
};

class UILayout;
typedef boost::shared_ptr<UILayout> UILayoutPtr;
typedef boost::weak_ptr<UILayout> UILayoutWeakPtr;

class AnchorLine
{
public:
    AnchorLine() : m_anchorType(ANCHOR_NONE) { }
    AnchorLine(const AnchorLine& other) :
        m_relativeElement(other.m_relativeElement), m_anchorType(other.m_anchorType) { }
    AnchorLine(UILayoutPtr relativeElement, EAnchorType anchorType) :
        m_relativeElement(relativeElement), m_anchorType(anchorType) { }
    bool isValid() const { return (m_anchorType != ANCHOR_NONE && !m_relativeElement.expired()); }

    /// Get the position relative to this anchor line
    int getPos() const;
    EAnchorType getAnchorType() const { return m_anchorType; }
    UILayoutPtr getRelativeElement() const { return m_relativeElement.lock(); }

private:
    UILayoutWeakPtr m_relativeElement;
    EAnchorType m_anchorType;
};

class UILayout : public Scriptable
{
public:
    UILayout() :
        m_marginLeft(0),
        m_marginRight(0),
        m_marginTop(0),
        m_marginBottom(0) { }
    virtual ~UILayout() { }

    void setSize(const Size& size);
    Size getSize() { return m_rect.size(); }

    /// Set the layout rect, always absolute position
    void setRect(const Rect& rect);
    /// Get layout size, it always return the absolute position
    const Rect& getRect() const{ return m_rect; }

    // anchors add methods
    bool addAnchor(EAnchorType type, const AnchorLine& anchorLine);
    void anchorLeft(const AnchorLine& anchorLine) { addAnchor(ANCHOR_LEFT, anchorLine); }
    void anchorRight(const AnchorLine& anchorLine) { addAnchor(ANCHOR_RIGHT, anchorLine); }
    void anchorTop(const AnchorLine& anchorLine) { addAnchor(ANCHOR_TOP, anchorLine); }
    void anchorBottom(const AnchorLine& anchorLine) { addAnchor(ANCHOR_BOTTOM, anchorLine); }
    void anchorHorizontalCenter(const AnchorLine& anchorLine) { addAnchor(ANCHOR_HORIZONTAL_CENTER, anchorLine); }
    void anchorVerticalCenter(const AnchorLine& anchorLine) { addAnchor(ANCHOR_VERTICAL_CENTER, anchorLine); }

    // anchor lines
    AnchorLine left() { return AnchorLine(asUILayout(), ANCHOR_LEFT); }
    AnchorLine right() { return AnchorLine(asUILayout(), ANCHOR_RIGHT); }
    AnchorLine top() { return AnchorLine(asUILayout(), ANCHOR_TOP); }
    AnchorLine bottom() { return AnchorLine(asUILayout(), ANCHOR_BOTTOM); }
    AnchorLine horizontalCenter() { return AnchorLine(asUILayout(), ANCHOR_HORIZONTAL_CENTER); }
    AnchorLine verticalCenter() { return AnchorLine(asUILayout(), ANCHOR_VERTICAL_CENTER); }

    // margins
    void setMargin(int top, int left, int bottom, int right) { m_marginLeft = left; m_marginRight = right; m_marginTop = top; m_marginBottom = bottom; recalculateLayout(); }
    void setMargin(int horizontal, int vertical) { m_marginLeft = m_marginRight = horizontal; m_marginTop = m_marginBottom = vertical; recalculateLayout(); }
    void setMargin(int margin) { m_marginLeft = m_marginRight = m_marginTop = m_marginBottom = margin; recalculateLayout(); }
    void setMarginLeft(int margin) { m_marginLeft = margin; recalculateLayout(); }
    void setMarginRight(int margin) { m_marginRight = margin; recalculateLayout(); }
    void setMarginTop(int margin) { m_marginTop = margin; recalculateLayout(); }
    void setMarginBottom(int margin) { m_marginBottom = margin; recalculateLayout(); }

    UILayoutPtr asUILayout() { return boost::static_pointer_cast<UILayout>(shared_from_this()); }

    virtual const char *getScriptableName() const { return "UILayout"; }

protected:
    virtual void onLayoutRectChange(const Rect& newRect) { }

private:
    /// Recalculate itself and anchored elements positions
    void recalculateLayout();
    /// Recalculate anchored elements positions
    void recalculateAnchoredLayout();
    void addAnchoredElement(UILayoutPtr anchoredElement);

    AnchorLine m_anchors[6];

    Rect m_rect;
    int m_marginLeft;
    int m_marginRight;
    int m_marginTop;
    int m_marginBottom;
    std::list<UILayoutWeakPtr> m_anchoredElements;
};

#endif // UILAYOUT_H
