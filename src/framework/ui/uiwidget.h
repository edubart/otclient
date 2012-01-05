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

#ifndef UIWIDGET_H
#define UIWIDGET_H

#include "declarations.h"
#include <framework/luascript/luaobject.h>
#include <framework/graphics/declarations.h>
#include <framework/otml/otmlnode.h>
#include <framework/graphics/font.h>

class UIWidget : public LuaObject
{
public:
    UIWidget();
    virtual ~UIWidget() { }

    template<class T>
    static std::shared_ptr<T> create() { auto t = std::shared_ptr<T>(new T); return t; }

    void destroy();

protected:
    virtual void render();
    virtual void renderSelf();
    virtual void renderChildren();

    friend class UIManager;

    void drawBackground(const Rect& screenCoords);
    void drawBorder(const Rect& screenCoords);
    void drawImage(const Rect& screenCoords);
    void drawIcon(const Rect& screenCoords);
    void drawText(const Rect& screenCoords);

public:
    void setVisible(bool visible);
    void setEnabled(bool enabled);
    void setPressed(bool pressed) { m_pressed = pressed; updateState(Fw::PressedState); }
    void setId(const std::string& id) { m_id = id; }
    void setFocusable(bool focusable);
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
    void setIcon(const std::string& iconFile);
    void setOpacity(int opacity) { m_opacity = opacity; }
    void setBackgroundColor(const Color& color) { m_backgroundColor = color; }
    void setForegroundColor(const Color& color) { m_foregroundColor = color; }
    void setMarginTop(int margin) { m_marginTop = margin; updateParentLayout(); }
    void setMarginRight(int margin) { m_marginRight = margin; updateParentLayout(); }
    void setMarginBottom(int margin) { m_marginBottom = margin; updateParentLayout(); }
    void setMarginLeft(int margin) { m_marginLeft = margin; updateParentLayout(); }
    void setText(const std::string& text);
    void setTextAlign(Fw::AlignmentFlag align) { m_textAlign = align; }
    void setTextOffset(const Point& offset) { m_textOffset = offset; }
    void setFont(const std::string& fontName);
    void setSizeFixed(bool fixed) { m_fixedSize = fixed; updateParentLayout(); }
    void setLastFocusReason(Fw::FocusReason reason) { m_lastFocusReason = reason; }

    void bindRectToParent();
    void resize(const Size& size) { setRect(Rect(getPos(), size)); }
    void resizeToText() { resize(getTextSize()); }
    void moveTo(const Point& pos) { setRect(Rect(pos, getSize())); }
    void hide() { setVisible(false); }
    void show() { setVisible(true); }
    void disable() { setEnabled(false); }
    void enable() { setEnabled(true); }
    void lock();
    void unlock();
    void focus();
    void grabMouse();
    void ungrabMouse();
    void grabKeyboard();
    void ungrabKeyboard();
    void clearText() { setText(""); }

    bool isActive() { return hasState(Fw::ActiveState); }
    bool isEnabled() { return !hasState(Fw::DisabledState); }
    bool isDisabled() { return hasState(Fw::DisabledState); }
    bool isFocused() { return hasState(Fw::FocusState); }
    bool isHovered() { return hasState(Fw::HoverState); }
    bool isPressed() { return hasState(Fw::PressedState); }
    bool isFirst() { return hasState(Fw::FirstState); }
    bool isMiddle() { return hasState(Fw::MiddleState); }
    bool isLast() { return hasState(Fw::LastState); }
    bool isAlternate() { return hasState(Fw::AlternateState); }
    bool isVisible();
    bool isHidden() { return !isVisible(); }
    bool isExplicitlyEnabled() { return m_enabled; }
    bool isExplicitlyVisible() { return m_visible; }
    bool isFocusable() { return m_focusable; }
    bool isPhantom() { return m_phantom; }
    bool isSizeFixed() { return m_fixedSize; }
    bool containsPoint(const Point& point) { return m_rect.contains(point); }
    bool hasChildren() { return m_children.size() > 0; }
    bool hasChild(const UIWidgetPtr& child);

    std::string getId() { return m_id; }
    int getChildCount() { return m_children.size(); }
    UILayoutPtr getLayout() { return m_layout; }
    UIWidgetPtr getParent() { return m_parent.lock(); }
    UIWidgetPtr getRootParent();
    Point getPos() { return m_rect.topLeft(); }
    Size getSize() { return m_rect.size(); }
    Rect getRect() { return m_rect; }
    int getX() { return m_rect.x(); }
    int getY() { return m_rect.y(); }
    int getWidth() { return m_rect.width(); }
    int getHeight() { return m_rect.height(); }
    Color getForegroundColor() { return m_foregroundColor; }
    Color getBackgroundColor() { return m_backgroundColor; }
    int getOpacity() { return m_opacity; }
    int getMarginTop() { return m_marginTop; }
    int getMarginRight() { return m_marginRight; }
    int getMarginBottom() { return m_marginBottom; }
    int getMarginLeft() { return m_marginLeft; }
    std::string getText() { return m_text; }
    Fw::AlignmentFlag getTextAlign() { return m_textAlign; }
    Point getTextOffset() { return m_textOffset; }
    std::string getFont() { return m_font->getName(); }
    Size getTextSize() { return m_font->calculateTextRectSize(m_text); }

    Fw::FocusReason getLastFocusReason() { return m_lastFocusReason; }
    OTMLNodePtr getStyle() { return m_style; }
    std::string getStyleName() { return m_style->tag(); }

    UIWidgetList getChildren() { return m_children; }
    UIWidgetPtr getFocusedChild() { return m_focusedChild; }
    UIWidgetPtr getChildAfter(const UIWidgetPtr& relativeChild);
    UIWidgetPtr getChildBefore(const UIWidgetPtr& relativeChild);
    UIWidgetPtr getChildById(const std::string& childId);
    UIWidgetPtr getChildByPos(const Point& childPos);
    UIWidgetPtr getChildByIndex(int index);
    UIWidgetPtr getFirstChild() { return getChildByIndex(1); }
    UIWidgetPtr getLastChild() { return getChildByIndex(-1); }
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
    void applyStyle(const OTMLNodePtr& styleNode);

    UIWidgetPtr asUIWidget() { return std::static_pointer_cast<UIWidget>(shared_from_this()); }

protected:
    bool setState(Fw::WidgetState state, bool on);
    bool hasState(Fw::WidgetState state);

private:
    void updateState(Fw::WidgetState state);
    void updateStates();
    void updateChildrenIndexStates();

    void updateStyle();

protected:
    virtual void onStyleApply(const std::string& styleName, const OTMLNodePtr& styleNode);
    virtual void onGeometryUpdate(const Rect& oldRect, const Rect& newRect);
    virtual void onFocusChange(bool focused, Fw::FocusReason reason);
    virtual void onHoverChange(bool hovered);
    virtual void onTextChange(const std::string& text);
    virtual void onFontChange(const std::string& font);
    virtual bool onKeyPress(uchar keyCode, std::string keyText, int keyboardModifiers);
    virtual bool onKeyRelease(uchar keyCode, std::string keyText, int keyboardModifiers);
    virtual bool onMousePress(const Point& mousePos, Fw::MouseButton button);
    virtual void onMouseRelease(const Point& mousePos, Fw::MouseButton button);
    virtual bool onMouseMove(const Point& mousePos, const Point& mouseMoved);
    virtual bool onMouseWheel(const Point& mousePos, Fw::MouseWheelDirection direction);

protected:
    std::string m_id;
    Fw::FocusReason m_lastFocusReason;
    Boolean<true> m_enabled;
    Boolean<true> m_visible;
    Boolean<true> m_focusable;
    Boolean<false> m_fixedSize;
    Boolean<false> m_pressed;
    Boolean<false> m_phantom;
    Boolean<false> m_updateEventScheduled;
    Boolean<true> m_firstOnStyle;
    Rect m_rect;
    UILayoutPtr m_layout;
    UIWidgetWeakPtr m_parent;
    UIWidgetList m_children;
    UIWidgetList m_lockedChildren;
    UIWidgetPtr m_focusedChild;
    OTMLNodePtr m_style;
    OTMLNodePtr m_stateStyle;
    ImagePtr m_image;
    TexturePtr m_icon;
    FontPtr m_font;
    Color m_backgroundColor;
    Color m_foregroundColor;
    int m_states;
    int m_opacity;
    int m_marginTop;
    int m_marginRight;
    int m_marginBottom;
    int m_marginLeft;
    std::string m_text;
    Point m_textOffset;
    Fw::AlignmentFlag m_textAlign;
};

#endif
