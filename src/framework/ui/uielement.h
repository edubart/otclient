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


#ifndef UIELEMENT_H
#define UIELEMENT_H

#include "../prerequisites.h"
#include "../input.h"
#include "../rect.h"
#include "uiconstants.h"

class UIElementSkin;

class UIContainer;
typedef std::shared_ptr<UIContainer> UIContainerPtr;
typedef std::weak_ptr<UIContainer> UIContainerWeakPtr;

class UIElement;
typedef std::shared_ptr<UIElement> UIElementPtr;
typedef std::weak_ptr<UIElement> UIElementWeakPtr;

enum EAnchorType {
    ANCHOR_LEFT = 0,
    ANCHOR_RIGHT,
    ANCHOR_TOP,
    ANCHOR_BOTTOM,
    ANCHOR_HORIZONTAL_CENTER,
    ANCHOR_VERTICAL_CENTER,
    ANCHOR_NONE
};

class AnchorLine
{
public:
    AnchorLine() : m_anchorType(ANCHOR_NONE) { }
    AnchorLine(const AnchorLine& other) :
        m_relativeElement(other.m_relativeElement), m_anchorType(other.m_anchorType) { }
    AnchorLine(UIElementPtr relativeElement, EAnchorType anchorType) :
        m_relativeElement(relativeElement), m_anchorType(anchorType) { }
    bool isValid() const { return (m_anchorType != ANCHOR_NONE && !m_relativeElement.expired()); }

    int getPos() const;
    EAnchorType getAnchorType() const { return m_anchorType; }
    UIElementPtr getRelativeElement() const { return m_relativeElement.lock(); }

private:
    UIElementWeakPtr m_relativeElement;
    EAnchorType m_anchorType;
};

class UIElement : public std::enable_shared_from_this<UIElement>
{
public:
    UIElement(UI::EElementType type = UI::Element);
    virtual ~UIElement() { }

    virtual void render();
    virtual bool onInputEvent(InputEvent *event) { return false; }

    bool setSkin(const std::string& skinName);
    void setSkin(UIElementSkin *skin);
    UIElementSkin *getSkin() { return m_skin; }

    virtual void setParent(UIContainerPtr parent) { m_parent = parent; }
    UIContainerPtr getParent() const { return m_parent.lock(); }

    void setId(const std::string& id) { m_id = id; }
    const std::string& getId() const { return m_id; }

    void setSize(const Size& size);
    Size getSize() { return m_rect.size(); }

    void setRect(const Rect& rect);
    const Rect& getRect() const{ return m_rect; }

    void setActive(bool active) { m_active = active; }
    bool isActive() const { return m_active; }

    void setVisible(bool visible) { m_visible = visible; }
    bool isVisible() const { return m_visible; }

    UI::EElementType getElementType() const { return m_type; }

    UIElementPtr asUIElement() { return shared_from_this(); }

    void recalculateAnchors();

    void addAnchor(EAnchorType type, const AnchorLine& anchorLine);
    void anchorLeft(const AnchorLine& anchorLine) { addAnchor(ANCHOR_LEFT, anchorLine); }
    void anchorRight(const AnchorLine& anchorLine) { addAnchor(ANCHOR_RIGHT, anchorLine); }
    void anchorTop(const AnchorLine& anchorLine) { addAnchor(ANCHOR_TOP, anchorLine); }
    void anchorBottom(const AnchorLine& anchorLine) { addAnchor(ANCHOR_BOTTOM, anchorLine); }
    void anchorHorizontalCenter(const AnchorLine& anchorLine) { addAnchor(ANCHOR_HORIZONTAL_CENTER, anchorLine); }
    void anchorVerticalCenter(const AnchorLine& anchorLine) { addAnchor(ANCHOR_VERTICAL_CENTER, anchorLine); }

    AnchorLine left() { return AnchorLine(asUIElement(), ANCHOR_LEFT); }
    AnchorLine right() { return AnchorLine(asUIElement(), ANCHOR_RIGHT); }
    AnchorLine top() { return AnchorLine(asUIElement(), ANCHOR_TOP); }
    AnchorLine bottom() { return AnchorLine(asUIElement(), ANCHOR_BOTTOM); }
    AnchorLine horizontalCenter() { return AnchorLine(asUIElement(), ANCHOR_HORIZONTAL_CENTER); }
    AnchorLine verticalCenter() { return AnchorLine(asUIElement(), ANCHOR_VERTICAL_CENTER); }

    void setMargin(int top, int left, int bottom, int right) { m_marginLeft = left; m_marginRight = right; m_marginTop = top; m_marginBottom = bottom; recalculateAnchors(); }
    void setMargin(int horizontal, int vertical) { m_marginLeft = m_marginRight = horizontal; m_marginTop = m_marginBottom = vertical; recalculateAnchors(); }
    void setMargin(int margin) { m_marginLeft = m_marginRight = m_marginTop = m_marginBottom = margin; recalculateAnchors(); }

protected:
    void addAnchoredElement(UIElementPtr anchoredElement);

    UI::EElementType m_type;
    UIContainerWeakPtr m_parent;
    UIElementSkin *m_skin;
    Rect m_rect;
    std::string m_id;
    bool m_visible;
    bool m_active;

private:
    AnchorLine m_anchors[6];

    int m_marginLeft;
    int m_marginRight;
    int m_marginTop;
    int m_marginBottom;
    std::list<UIElementWeakPtr> m_anchoredElements;
};

#endif // UIELEMENT_H
