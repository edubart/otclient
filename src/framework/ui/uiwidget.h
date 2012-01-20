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
#include <framework/graphics/coordsbuffer.h>

template<typename T = int>
struct EdgeGroup {
    EdgeGroup() { top = right = bottom = left = T(0); }
    void set(T value) { top = right = bottom = left = value; }
    T top;
    T right;
    T bottom;
    T left;
};

class UIWidget : public LuaObject
{
// widget core
public:
    UIWidget();
    virtual ~UIWidget();

protected:
    virtual void draw();
    virtual void drawSelf();
    virtual void drawChildren();

    friend class UIManager;

    std::string m_id;
    Rect m_rect;
    Boolean<true> m_enabled;
    Boolean<true> m_visible;
    Boolean<true> m_focusable;
    Boolean<false> m_fixedSize;
    Boolean<false> m_pressed;
    Boolean<false> m_phantom;
    Boolean<false> m_dragable;
    Boolean<false> m_dragging;
    Boolean<false> m_destroyed;
    UILayoutPtr m_layout;
    UIWidgetWeakPtr m_parent;
    UIWidgetList m_children;
    UIWidgetList m_lockedChildren;
    UIWidgetPtr m_focusedChild;
    OTMLNodePtr m_style;
    Fw::FocusReason m_lastFocusReason;

public:
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
    void applyStyle(const OTMLNodePtr& styleNode);
    void addAnchor(Fw::AnchorEdge anchoredEdge, const std::string& hookedWidgetId, Fw::AnchorEdge hookedEdge);
    void fill(const std::string& hookedWidgetId);
    void centerIn(const std::string& hookedWidgetId);
    void breakAnchors();
    void updateParentLayout();
    void updateLayout();
    void lock();
    void unlock();
    void focus();
    void grabMouse();
    void ungrabMouse();
    void grabKeyboard();
    void ungrabKeyboard();
    void bindRectToParent();
    void destroy();

    void setId(const std::string& id);
    void setParent(const UIWidgetPtr& parent);
    void setLayout(const UILayoutPtr& layout);
    void setRect(const Rect& rect);
    void setStyle(const std::string& styleName);
    void setStyleFromNode(const OTMLNodePtr& styleNode);
    void setEnabled(bool enabled);
    void setVisible(bool visible);
    void setPressed(bool pressed);
    void setOn(bool on);
    void setChecked(bool checked);
    void setFocusable(bool focusable);
    void setPhantom(bool phantom);
    void setDragging(bool dragging);
    void setDragable(bool dragable);
    void setFixedSize(bool fixed);
    void setLastFocusReason(Fw::FocusReason reason);

    bool isVisible();
    bool isChildLocked(const UIWidgetPtr& child);
    bool hasChild(const UIWidgetPtr& child);
    int getChildIndex(const UIWidgetPtr& child);
    Rect getChildrenRect();
    UIAnchorLayoutPtr getAnchoredLayout();
    UIWidgetPtr getRootParent();
    UIWidgetPtr getChildAfter(const UIWidgetPtr& relativeChild);
    UIWidgetPtr getChildBefore(const UIWidgetPtr& relativeChild);
    UIWidgetPtr getChildById(const std::string& childId);
    UIWidgetPtr getChildByPos(const Point& childPos);
    UIWidgetPtr getChildByIndex(int index);
    UIWidgetPtr recursiveGetChildById(const std::string& id);
    UIWidgetPtr recursiveGetChildByPos(const Point& childPos);
    UIWidgetPtr backwardsGetWidgetById(const std::string& id);

    UIWidgetPtr asUIWidget() { return std::static_pointer_cast<UIWidget>(shared_from_this()); }

private:
    Boolean<false> m_updateEventScheduled;
    Boolean<false> m_loadingStyle;


// state managment
protected:
    bool setState(Fw::WidgetState state, bool on);
    bool hasState(Fw::WidgetState state);

private:
    void updateState(Fw::WidgetState state);
    void updateStates();
    void updateChildrenIndexStates();
    void updateStyle();

    Boolean<false> m_updateStyleScheduled;
    Boolean<true> m_firstOnStyle;
    OTMLNodePtr m_stateStyle;
    int m_states;


// event processing
protected:
    virtual void onStyleApply(const std::string& styleName, const OTMLNodePtr& styleNode);
    virtual void onGeometryChange(const Rect& oldRect, const Rect& newRect);
    virtual void onFocusChange(bool focused, Fw::FocusReason reason);
    virtual void onHoverChange(bool hovered);
    virtual void onDragEnter(const Point& mousePos);
    virtual void onDragLeave(UIWidgetPtr droppedWidget, const Point& mousePos);
    virtual void onDrop(UIWidgetPtr draggedWidget, const Point& mousePos);
    virtual bool onKeyText(const std::string& keyText);
    virtual bool onKeyDown(uchar keyCode, int keyboardModifiers);
    virtual bool onKeyPress(uchar keyCode, int keyboardModifiers, bool wouldFilter);
    virtual bool onKeyUp(uchar keyCode, int keyboardModifiers);
    virtual bool onMousePress(const Point& mousePos, Fw::MouseButton button);
    virtual void onMouseRelease(const Point& mousePos, Fw::MouseButton button);
    virtual bool onMouseMove(const Point& mousePos, const Point& mouseMoved);
    virtual bool onMouseWheel(const Point& mousePos, Fw::MouseWheelDirection direction);

    bool propagateOnKeyText(const std::string& keyText);
    bool propagateOnKeyDown(uchar keyCode, int keyboardModifiers);
    bool propagateOnKeyPress(uchar keyCode, int keyboardModifiers, bool wouldFilter);
    bool propagateOnKeyUp(uchar keyCode, int keyboardModifiers);
    bool propagateOnMousePress(const Point& mousePos, Fw::MouseButton button);
    void propagateOnMouseRelease(const Point& mousePos, Fw::MouseButton button);
    bool propagateOnMouseMove(const Point& mousePos, const Point& mouseMoved);
    bool propagateOnMouseWheel(const Point& mousePos, Fw::MouseWheelDirection direction);


// function shortcuts
public:
    void resize(int width, int height) { setRect(Rect(getPos(), Size(width, height))); }
    void move(int x, int y) { setRect(Rect(x, y, getSize())); }
    void hide() { setVisible(false); }
    void show() { setVisible(true); }
    void disable() { setEnabled(false); }
    void enable() { setEnabled(true); }

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
    bool isChecked() { return hasState(Fw::CheckedState); }
    bool isOn() { return hasState(Fw::OnState); }
    bool isHidden() { return !isVisible(); }
    bool isExplicitlyEnabled() { return m_enabled; }
    bool isExplicitlyVisible() { return m_visible; }
    bool isFocusable() { return m_focusable; }
    bool isPhantom() { return m_phantom; }
    bool isDragable() { return m_dragable; }
    bool isDragging() { return m_dragging; }
    bool isFixedSize() { return m_fixedSize; }
    bool isDestroyed() { return m_destroyed; }

    bool hasChildren() { return m_children.size() > 0; }
    bool containsPoint(const Point& point) { return m_rect.contains(point); }

    std::string getId() { return m_id; }
    UIWidgetPtr getParent() { return m_parent.lock(); }
    UIWidgetPtr getFocusedChild() { return m_focusedChild; }
    UIWidgetList getChildren() { return m_children; }
    UIWidgetPtr getFirstChild() { return getChildByIndex(1); }
    UIWidgetPtr getLastChild() { return getChildByIndex(-1); }
    UILayoutPtr getLayout() { return m_layout; }
    OTMLNodePtr getStyle() { return m_style; }
    int getChildCount() { return m_children.size(); }
    Fw::FocusReason getLastFocusReason() { return m_lastFocusReason; }
    std::string getStyleName() { return m_style->tag(); }


// base style
private:
    void initBaseStyle();
    void parseBaseStyle(const OTMLNodePtr& styleNode);

protected:
    void drawBackground(const Rect& screenCoords);
    void drawBorder(const Rect& screenCoords);
    void drawIcon(const Rect& screenCoords);

    Color m_color;
    Color m_backgroundColor;
    Rect m_backgroundRect;
    TexturePtr m_icon;
    Color m_iconColor;
    Rect m_iconRect;
    EdgeGroup<Color> m_borderColor;
    EdgeGroup<int> m_borderWidth;
    EdgeGroup<int> m_margin;
    EdgeGroup<int> m_padding;
    float m_opacity;

public:
    void setX(int x) { move(x, getY()); }
    void setY(int y) { move(getX(), y); }
    void setWidth(int width) { resize(width, getHeight()); }
    void setHeight(int height) { resize(getWidth(), height); }
    void setSize(const Size& size) { resize(size.width(), size.height()); }
    void setPos(const Point& pos) { move(pos.x, pos.y); }
    void setColor(const Color& color) { m_color = color; }
    void setBackgroundColor(const Color& color) { m_backgroundColor = color; }
    void setBackgroundOffsetX(int x) { m_backgroundRect.setX(x); }
    void setBackgroundOffsetY(int y) { m_backgroundRect.setX(y); }
    void setBackgroundOffset(const Point& pos) { m_backgroundRect.move(pos); }
    void setBackgroundWidth(int width) { m_backgroundRect.setWidth(width); }
    void setBackgroundHeight(int height) { m_backgroundRect.setHeight(height); }
    void setBackgroundSize(const Size& size) { m_backgroundRect.resize(size); }
    void setBackgroundRect(const Rect& rect) { m_backgroundRect = rect; }
    void setIcon(const std::string& iconFile);
    void setIconColor(const Color& color) { m_iconColor = color; }
    void setIconOffsetX(int x) { m_iconRect.setX(x); }
    void setIconOffsetY(int y) { m_iconRect.setX(y); }
    void setIconOffset(const Point& pos) { m_iconRect.move(pos); }
    void setIconWidth(int width) { m_iconRect.setWidth(width); }
    void setIconHeight(int height) { m_iconRect.setHeight(height); }
    void setIconSize(const Size& size) { m_iconRect.resize(size); }
    void setIconRect(const Rect& rect) { m_iconRect = rect; }
    void setBorderWidth(int width) { m_borderWidth.set(width); updateLayout(); }
    void setBorderWidthTop(int width) { m_borderWidth.top = width; }
    void setBorderWidthRight(int width) { m_borderWidth.right = width; }
    void setBorderWidthBottom(int width) { m_borderWidth.bottom = width; }
    void setBorderWidthLeft(int width) { m_borderWidth.left = width; }
    void setBorderColor(const Color& color) { m_borderColor.set(color); updateLayout(); }
    void setBorderColorTop(const Color& color) { m_borderColor.top = color; }
    void setBorderColorRight(const Color& color) { m_borderColor.right = color; }
    void setBorderColorBottom(const Color& color) { m_borderColor.bottom = color; }
    void setBorderColorLeft(const Color& color) { m_borderColor.left = color; }
    void setMargin(int margin) { m_margin.set(margin); updateParentLayout(); }
    void setMarginHorizontal(int margin) { m_margin.right = m_margin.left = margin; updateParentLayout(); }
    void setMarginVertical(int margin) { m_margin.bottom = m_margin.top = margin; updateParentLayout(); }
    void setMarginTop(int margin) { m_margin.top = margin; updateParentLayout(); }
    void setMarginRight(int margin) { m_margin.right = margin; updateParentLayout(); }
    void setMarginBottom(int margin) { m_margin.bottom = margin; updateParentLayout(); }
    void setMarginLeft(int margin) { m_margin.left = margin; updateParentLayout(); }
    void setPadding(int padding) { m_padding.top = m_padding.right = m_padding.bottom = m_padding.left = padding; updateLayout(); }
    void setPaddingHorizontal(int padding) { m_padding.right = m_padding.left = padding; updateLayout(); }
    void setPaddingVertical(int padding) { m_padding.bottom = m_padding.top = padding; updateLayout(); }
    void setPaddingTop(int padding) { m_padding.top = padding; updateLayout(); }
    void setPaddingRight(int padding) { m_padding.right = padding; updateLayout(); }
    void setPaddingBottom(int padding) { m_padding.bottom = padding; updateLayout(); }
    void setPaddingLeft(int padding) { m_padding.left = padding; updateLayout(); }
    void setOpacity(float opacity) { m_opacity = opacity; }

    int getX() { return m_rect.x(); }
    int getY() { return m_rect.y(); }
    Point getPos() { return m_rect.topLeft(); }
    int getWidth() { return m_rect.width(); }
    int getHeight() { return m_rect.height(); }
    Size getSize() { return m_rect.size(); }
    Rect getRect() { return m_rect; }
    Color getColor() { return m_color; }
    Color getBackgroundColor() { return m_backgroundColor; }
    int getBackgroundOffsetX() { return m_backgroundRect.x(); }
    int getBackgroundOffsetY() { return m_backgroundRect.y(); }
    Point getBackgroundOffset() { return m_backgroundRect.topLeft(); }
    int getBackgroundWidth() { return m_backgroundRect.width(); }
    int getBackgroundHeight() { return m_backgroundRect.height(); }
    Size getBackgroundSize() { return m_backgroundRect.size(); }
    Rect getBackgroundRect() { return m_backgroundRect; }
    Color getIconColor() { return m_iconColor; }
    int getIconOffsetX() { return m_iconRect.x(); }
    int getIconOffsetY() { return m_iconRect.y(); }
    Point getIconOffset() { return m_iconRect.topLeft(); }
    int getIconWidth() { return m_iconRect.width(); }
    int getIconHeight() { return m_iconRect.height(); }
    Size getIconSize() { return m_iconRect.size(); }
    Rect getIconRect() { return m_iconRect; }
    Color getBorderTopColor() { return m_borderColor.top; }
    Color getBorderRightColor() { return m_borderColor.right; }
    Color getBorderBottomColor() { return m_borderColor.bottom; }
    Color getBorderLeftColor() { return m_borderColor.left; }
    int getBorderTopWidth() { return m_borderWidth.top; }
    int getBorderRightWidth() { return m_borderWidth.right; }
    int getBorderBottomWidth() { return m_borderWidth.bottom; }
    int getBorderLeftWidth() { return m_borderWidth.left; }
    int getMarginTop() { return m_margin.top; }
    int getMarginRight() { return m_margin.right; }
    int getMarginBottom() { return m_margin.bottom; }
    int getMarginLeft() { return m_margin.left; }
    int getPaddingTop() { return m_padding.top; }
    int getPaddingRight() { return m_padding.right; }
    int getPaddingBottom() { return m_padding.bottom; }
    int getPaddingLeft() { return m_padding.left; }
    float getOpacity() { return m_opacity; }


// image
private:
    void initImage() { }
    void parseImageStyle(const OTMLNodePtr& styleNode);

    void updateImageCache() { m_imageMustRecache = true; }
    void configureBorderImage() { m_imageBordered = true; updateImageCache(); }

    CoordsBuffer m_imageCoordsBuffer;
    Rect m_imageCachedScreenCoords;
    Boolean<true> m_imageMustRecache;
    Boolean<false> m_imageBordered;

protected:
    void drawImage(const Rect& screenCoords);

    TexturePtr m_imageTexture;
    Rect m_imageClipRect;
    Rect m_imageRect;
    Color m_imageColor;
    Boolean<false> m_imageFixedRatio;
    Boolean<false> m_imageRepeated;
    Boolean<false> m_imageSmooth;
    EdgeGroup<int> m_imageBorder;

public:
    void setImageSource(const std::string& source);
    void setImageClip(const Rect& clipRect) { m_imageClipRect = clipRect; updateImageCache(); }
    void setImageOffsetX(int x) { m_imageRect.setX(x); updateImageCache(); }
    void setImageOffsetY(int y) { m_imageRect.setX(y); updateImageCache(); }
    void setImageOffset(const Point& pos) { m_imageRect.move(pos); updateImageCache(); }
    void setImageWidth(int width) { m_imageRect.setWidth(width); updateImageCache(); }
    void setImageHeight(int height) { m_imageRect.setHeight(height); updateImageCache(); }
    void setImageSize(const Size& size) { m_imageRect.resize(size); updateImageCache(); }
    void setImageRect(const Rect& rect) { m_imageRect = rect; updateImageCache(); }
    void setImageColor(const Color& color) { m_imageColor = color; updateImageCache(); }
    void setImageFixedRatio(bool fixedRatio) { m_imageFixedRatio = fixedRatio; updateImageCache(); }
    void setImageRepeated(bool repeated) { m_imageRepeated = repeated; updateImageCache(); }
    void setImageSmooth(bool smooth) { m_imageSmooth = smooth; }
    void setImageBorderTop(int border) { m_imageBorder.top = border; configureBorderImage(); }
    void setImageBorderRight(int border) { m_imageBorder.right = border; configureBorderImage(); }
    void setImageBorderBottom(int border) { m_imageBorder.bottom = border; configureBorderImage(); }
    void setImageBorderLeft(int border) { m_imageBorder.left = border; configureBorderImage(); }
    void setImageBorder(int border) { m_imageBorder.set(border); configureBorderImage(); }

    Rect getImageClip() { return m_imageClipRect; }
    int getImageOffsetX() { return m_imageRect.x(); }
    int getImageOffsetY() { return m_imageRect.y(); }
    Point getImageOffset() { return m_imageRect.topLeft(); }
    int getImageWidth() { return m_imageRect.width(); }
    int getImageHeight() { return m_imageRect.height(); }
    Size getImageSize() { return m_imageRect.size(); }
    Rect getImageRect() { return m_imageRect; }
    Color getImageColor() { return m_imageColor; }
    bool isImageFixedRatio() { return m_imageFixedRatio; }
    bool isImageSmooth() { return m_imageSmooth; }
    int getImageBorderTop() { return m_imageBorder.top; }
    int getImageBorderRight() { return m_imageBorder.right; }
    int getImageBorderBottom() { return m_imageBorder.bottom; }
    int getImageBorderLeft() { return m_imageBorder.left; }

// text related
private:
    void initText();
    void parseTextStyle(const OTMLNodePtr& styleNode);

    Boolean<true> m_textMustRecache;
    FrameBufferPtr m_textFramebuffer;
    Size m_textCachedBoxSize;

protected:
    void drawText(const Rect& screenCoords);

    virtual void onTextChange(const std::string& text);
    virtual void onFontChange(const std::string& font);

    std::string m_text;
    Fw::AlignmentFlag m_textAlign;
    Point m_textOffset;
    FontPtr m_font;

public:
    void resizeToText() { setSize(getTextSize()); }
    void clearText() { setText(""); }

    void setText(const std::string& text);
    void setTextAlign(Fw::AlignmentFlag align) { m_textAlign = align; m_textMustRecache = true; }
    void setTextOffset(const Point& offset) { m_textOffset = offset; m_textMustRecache = true; }
    void setFont(const std::string& fontName);

    std::string getText() { return m_text; }
    Fw::AlignmentFlag getTextAlign() { return m_textAlign; }
    Point getTextOffset() { return m_textOffset; }
    std::string getFont() { return m_font->getName(); }
    Size getTextSize() { return m_font->calculateTextRectSize(m_text); }
};

#endif
