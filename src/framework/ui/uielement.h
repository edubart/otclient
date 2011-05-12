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

#include <prerequisites.h>
#include <core/input.h>
#include <script/scriptable.h>
#include <ui/uiconstants.h>
#include <ui/uielementskin.h>
#include <ui/uilayout.h>

class UIElementSkin;

class UIContainer;
typedef boost::shared_ptr<UIContainer> UIContainerPtr;
typedef boost::weak_ptr<UIContainer> UIContainerWeakPtr;

class UIElement;
typedef boost::shared_ptr<UIElement> UIElementPtr;
typedef boost::weak_ptr<UIElement> UIElementWeakPtr;

class UIElement : public Scriptable
{
public:
    UIElement(UI::ElementType type = UI::Element);
    virtual ~UIElement();

    void destroyLater();
    virtual void destroy();
    virtual void destroyCheck();

    /// Draw element
    virtual void render();

    // events
    virtual void onLoad();
    virtual void onInputEvent(const InputEvent& event) { }
    virtual void onFocusChange() { }
    virtual void onRectUpdate() { }

    UIElementPtr backwardsGetElementById(const std::string& id);

    void moveTo(Point pos);

    void setLayout(const UILayoutPtr& layout) { m_layout = layout; }
    UILayoutPtr getLayout() const;

    void setSkin(const UIElementSkinPtr& skin);
    UIElementSkinPtr getSkin() const { return m_skin; }

    void setParent(UIContainerPtr parent);
    UIContainerPtr getParent() const { return m_parent.lock(); }

    void setId(const std::string& id) { m_id = id; }
    const std::string& getId() const { return m_id; }

    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

    void setFocused(bool focused);
    bool isFocused() const;

    void setMouseOver(bool mouseOver) { m_mouseOver = mouseOver; }
    bool isMouseOver() const { return m_mouseOver; }

    void setVisible(bool visible) { m_visible = visible; }
    bool isVisible() const { return m_visible; }

    virtual bool isFocusable() const { return false; }
    UI::ElementType getElementType() const { return m_type; }

    UIElementPtr asUIElement() { return boost::static_pointer_cast<UIElement>(shared_from_this()); }
    virtual UIContainerPtr asUIContainer() { return UIContainerPtr(); }
    virtual const char *getScriptableName() const { return "UIElement"; }

    void setSize(const Size& size);
    Size getSize() { return m_rect.size(); }

    /// Set the layout rect, always absolute position
    void setRect(const Rect& rect);
    /// Get layout size, it always return the absolute position
    Rect getRect() const { return m_rect; }

    // margins
    void setMargin(int top, int left, int bottom, int right) { m_marginLeft = left; m_marginRight = right; m_marginTop = top; m_marginBottom = bottom; getLayout()->recalculateElementLayout(asUIElement()); }
    void setMargin(int horizontal, int vertical) { m_marginLeft = m_marginRight = horizontal; m_marginTop = m_marginBottom = vertical; getLayout()->recalculateElementLayout(asUIElement()); }
    void setMargin(int margin) { m_marginLeft = m_marginRight = m_marginTop = m_marginBottom = margin; getLayout()->recalculateElementLayout(asUIElement()); }
    void setMarginLeft(int margin) { m_marginLeft = margin; getLayout()->recalculateElementLayout(asUIElement()); }
    void setMarginRight(int margin) { m_marginRight = margin; getLayout()->recalculateElementLayout(asUIElement()); }
    void setMarginTop(int margin) { m_marginTop = margin; getLayout()->recalculateElementLayout(asUIElement()); }
    void setMarginBottom(int margin) { m_marginBottom = margin; getLayout()->recalculateElementLayout(asUIElement()); }

    int getMarginLeft() const { return m_marginLeft; }
    int getMarginRight() const { return m_marginRight; }
    int getMarginTop() const { return m_marginTop; }
    int getMarginBottom() const { return m_marginBottom; }

private:
    UI::ElementType m_type;
    UIContainerWeakPtr m_parent;
    UIElementSkinPtr m_skin;
    UILayoutPtr m_layout;
    std::string m_id;
    bool m_visible;
    bool m_enabled;
    bool m_mouseOver;

    Rect m_rect;
    int m_marginLeft;
    int m_marginRight;
    int m_marginTop;
    int m_marginBottom;
};

#endif // UIELEMENT_H
