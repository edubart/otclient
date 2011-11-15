/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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

#ifndef UIWIDGET_H
#define UIWIDGET_H

#include "declarations.h"
#include <framework/luascript/luaobject.h>
#include <framework/graphics/declarations.h>

class UIWidget : public LuaObject
{
public:
    UIWidget();
    virtual ~UIWidget();

    template<class T>
    static std::shared_ptr<T> create() { auto t = std::shared_ptr<T>(new T); t->setup(); return t; }

    void destroy();

    virtual void setup();
    virtual void render();
    void renderSelf();
    void renderChildren();

    void setVisible(bool visible);
    void setEnabled(bool enabled) { m_enabled = enabled; updateState(Fw::DisabledState); }
    void setPressed(bool pressed) { m_pressed = pressed; updateState(Fw::PressedState); }
    void setId(const std::string& id) { m_id = id; }
    void setFocusable(bool focusable) { m_focusable = focusable; }
    void setPhantom(bool phantom) { m_phantom = phantom; }
    void setStyle(const std::string& styleName);
    void setStyleFromNode(const OTMLNodePtr& styleNode);
    void setLayout(const UILayoutPtr& layout) { m_layout = layout; }
    void setParent(const UIWidgetPtr& parent);
    void setRect(const Rect& rect);
    void setX(int x) { moveTo(Point(x, getY())); }
    void setY(int y) { moveTo(Point(getX(), y)); }
    void setWidth(int width) { resize(Size(width, getHeight())); }
    void setHeight(int height) { resize(Size(getWidth(), height)); }
    void setImage(const ImagePtr& image) { m_image = image; }
    virtual void setFont(const FontPtr& font) { m_font = font; }
    void setOpacity(int opacity) { m_opacity = opacity; }
    void setBackgroundColor(const Color& color) { m_backgroundColor = color; }
    void setForegroundColor(const Color& color) { m_foregroundColor = color; }
    void setMarginLeft(int margin) { m_marginLeft = margin; updateParentLayout(); }
    void setMarginRight(int margin) { m_marginRight = margin; updateParentLayout(); }
    void setMarginTop(int margin) { m_marginTop = margin; updateParentLayout(); }
    void setMarginBottom(int margin) { m_marginBottom = margin; updateParentLayout(); }
    void setSizeFixed(bool fixed) { m_fixedSize = fixed; updateParentLayout(); }
    void setLastFocusReason(Fw::FocusReason reason) { m_lastFocusReason = reason; }

    void resize(const Size& size) { setRect(Rect(getPosition(), size)); }
    void moveTo(const Point& pos) { setRect(Rect(pos, getSize())); }
    void hide() { setVisible(false); }
    void show() { setVisible(true); }
    void disable() { setEnabled(false); }
    void enable() { setEnabled(true); }
    void lock();
    void unlock();
    void focus();

    bool isActive()  { return hasState(Fw::ActiveState); }
    bool isEnabled()  { return !hasState(Fw::DisabledState); }
    bool isDisabled()  { return hasState(Fw::DisabledState); }
    bool isFocused()  { return hasState(Fw::FocusState); }
    bool isHovered()  { return hasState(Fw::HoverState); }
    bool isPressed()  { return hasState(Fw::PressedState); }
    bool isVisible();
    bool isHidden() { return !isVisible(); }
    bool isExplicitlyEnabled()  { return m_enabled; }
    bool isExplicitlyVisible()  { return m_visible; }
    bool isFocusable()  { return m_focusable; }
    bool isPhantom()  { return m_phantom; }
    bool isSizeFixed()  { return m_fixedSize; }
    bool hasChildren()  { return m_children.size() > 0; }
    bool hasChild(const UIWidgetPtr& child);

    std::string getId()  { return m_id; }
    int getChildCount()  { return m_children.size(); }
    UILayoutPtr getLayout()  { return m_layout; }
    UIWidgetPtr getParent()  { return m_parent.lock(); }
    UIWidgetPtr getRootParent();
    Point getPosition()  { return m_rect.topLeft(); }
    Size getSize()  { return m_rect.size(); }
    Rect getRect()  { return m_rect; }
    int getX()  { return m_rect.x(); }
    int getY()  { return m_rect.y(); }
    int getWidth()  { return m_rect.width(); }
    int getHeight()  { return m_rect.height(); }
    ImagePtr getImage()  { return m_image; }
    FontPtr getFont()  { return m_font; }
    Color getForegroundColor()  { return m_foregroundColor; }
    Color getBackgroundColor()  { return m_backgroundColor; }
    int getOpacity()  { return m_opacity; }
    int getMarginLeft()  { return m_marginLeft; }
    int getMarginRight()  { return m_marginRight; }
    int getMarginTop()  { return m_marginTop; }
    int getMarginBottom()  { return m_marginBottom; }
    Fw::FocusReason getLastFocusReason()  { return m_lastFocusReason; }
    OTMLNodePtr getStyle()  { return m_style; }

    UIWidgetList getChildren()  { return m_children; }
    UIWidgetPtr getFocusedChild()  { return m_focusedChild; }
    UIWidgetPtr getChildAfter(const UIWidgetPtr& relativeChild);
    UIWidgetPtr getChildBefore(const UIWidgetPtr& relativeChild);
    UIWidgetPtr getChildById(const std::string& childId);
    UIWidgetPtr getChildByPos(const Point& childPos);
    UIWidgetPtr getChildByIndex(int index);
    UIWidgetPtr recursiveGetChildById(const std::string& id);
    UIWidgetPtr recursiveGetChildByPos(const Point& childPos);
    UIWidgetPtr backwardsGetWidgetById(const std::string& id);

    void addChild(const UIWidgetPtr& child);
    void insertChild(int index, const UIWidgetPtr& child);
    void removeChild(const UIWidgetPtr& child);
    void focusChild(const UIWidgetPtr& child, Fw::FocusReason reason);
    void focusNextChild(Fw::FocusReason reason);
    void focusPreviousChild(Fw::FocusReason reason);
    void moveChildToTop(const UIWidgetPtr& child);
    void moveChildToIndex(const UIWidgetPtr& child, int index);
    void lockChild(const UIWidgetPtr& child);
    void unlockChild(const UIWidgetPtr& child);
    bool isChildLocked(const UIWidgetPtr& child);
    int getChildIndex(const UIWidgetPtr& child);

    void updateParentLayout();
    void updateLayout();

    void updateStates();
    virtual void updateState(Fw::WidgetState state);
    void setState(Fw::WidgetState state, bool on);
    bool hasState(Fw::WidgetState state);

    void updateStyle();
    void applyStyle(const OTMLNodePtr& styleNode);

    UIWidgetPtr asUIWidget() { return std::static_pointer_cast<UIWidget>(shared_from_this()); }

protected:
    /// Triggered when widget style is changed
    virtual void onStyleApply(const OTMLNodePtr& styleNode);
    /// Triggered when widget is moved or resized
    virtual void onGeometryUpdate(const Rect& oldRect, const Rect& newRect);
    /// Triggered when widget gets or loses focus
    virtual void onFocusChange(bool focused, Fw::FocusReason reason);
    /// Triggered when the mouse enters or leaves widget area
    virtual void onHoverChange(bool hovered);
    /// Triggered when user presses key while widget has focus
    virtual bool onKeyPress(uchar keyCode, char keyChar, int keyboardModifiers);
    /// Triggered when user releases key while widget has focus
    virtual bool onKeyRelease(uchar keyCode, char keyChar, int keyboardModifiers);
    /// Triggered when a mouse button is pressed down while mouse pointer is inside widget area
    virtual bool onMousePress(const Point& mousePos, Fw::MouseButton button);
    /// Triggered when a mouse button is released
    virtual void onMouseRelease(const Point& mousePos, Fw::MouseButton button);
    /// Triggered when mouse moves (even when the mouse is outside widget area)
    virtual bool onMouseMove(const Point& mousePos, const Point& mouseMoved);
    /// Triggered when mouse middle button wheels inside widget area
    virtual bool onMouseWheel(const Point& mousePos, Fw::MouseWheelDirection direction);

    friend class UIManager;

protected:
    std::string m_id;
    Fw::FocusReason m_lastFocusReason;
    bool m_enabled;
    bool m_visible;
    bool m_focusable;
    bool m_fixedSize;
    bool m_pressed;
    bool m_phantom;
    bool m_updateEventScheduled;
    bool m_firstOnStyle;
    Rect m_rect;
    UILayoutPtr m_layout;
    UIWidgetWeakPtr m_parent;
    UIWidgetList m_children;
    UIWidgetList m_lockedChildren;
    UIWidgetPtr m_focusedChild;
    OTMLNodePtr m_style;
    OTMLNodePtr m_stateStyle;
    int m_states;

    // basic style components used by all widgets
    ImagePtr m_image;
    FontPtr m_font;
    int m_opacity;
    Color m_backgroundColor;
    Color m_foregroundColor;
    int m_marginLeft;
    int m_marginRight;
    int m_marginTop;
    int m_marginBottom;
};

#endif
