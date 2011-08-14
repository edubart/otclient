#ifndef UIWIDGET_H
#define UIWIDGET_H

#include "uideclarations.h"
#include "uievent.h"
#include <luascript/luaobject.h>
#include <graphics/graphicsdeclarations.h>
#include <otml/otmldeclarations.h>

class UIWidget : public LuaObject
{
public:
    UIWidget(UIWidgetType type = UITypeWidget);
    virtual ~UIWidget();

    static UIWidgetPtr create();

    /// Remove this widget from parent then destroy it and its children
    virtual void destroy();

    /// Called after the widget is loaded from OTML file, to execute onLoad event
    virtual void load();

    /// Load style from otml node
    virtual void loadStyleFromOTML(const OTMLNodePtr& styleNode);

    /// Draw widget on screen
    virtual void render();

    /// Notifies the layout system that this widget has changed and may need to change geometry
    void updateGeometry();

    void setEnabled(bool enable) { m_enabled = enable; }
    void setLayout(const UILayoutPtr& layout) { m_layout = layout; }
    void setId(const std::string& id) { m_id = id; }
    void setFocusable(bool focusable) { m_focusable = focusable; }
    void setHovered(bool hovered) { m_hovered = hovered; }
    void setVisible(bool visible) { m_visible = visible; }
    void setParent(const UIWidgetPtr& parent);
    void setStyle(const std::string& styleName);
    void setGeometry(const Rect& rect);
    void setLocked(bool locked);
    void setX(int x) { move(Point(x, getY())); }
    void setY(int y) { move(Point(getX(), y)); }
    void setWidth(int width) { resize(Size(width, getHeight())); }
    void setHeight(int height) { resize(Size(getWidth(), height)); }
    void resize(const Size& size) { setGeometry(Rect(getPosition(), size)); updateGeometry(); }
    void move(const Point& pos) { setGeometry(Rect(pos, getSize())); }

    void setImage(const ImagePtr& image) { m_image = image; }
    virtual void setFont(const FontPtr& font) { m_font = font; }
    void setColor(const Color& color) { m_color = color; }
    void setMarginLeft(int margin) { m_marginLeft = margin; updateGeometry(); }
    void setMarginRight(int margin) { m_marginRight = margin; updateGeometry(); }
    void setMarginTop(int margin) { m_marginTop = margin; updateGeometry(); }
    void setMarginBottom(int margin) { m_marginBottom = margin; updateGeometry(); }

    void hide() { setVisible(false); }
    void show() { setVisible(true); }
    void disable() { setEnabled(false); }
    void enable() { setEnabled(true); }
    void lock();

    bool isEnabled();
    bool isExplicitlyEnabled() const { return m_enabled; }
    bool isVisible() const { return m_visible; }
    bool isHovered() const { return m_hovered; }
    bool isFocusable() const { return m_focusable; }
    bool isDestroyed() const { return m_destroyed; }
    bool hasChildren() const { return m_children.size() > 0; }
    bool hasFocus();
    bool hasChild(const UIWidgetPtr& child);

    UIWidgetType getWidgetType() const { return m_type; }
    UILayoutPtr getLayout() const;
    std::string getId() const { return m_id; }
    int getChildCount() const { return m_children.size(); }
    UIWidgetPtr getParent() const { return m_parent.lock(); }
    Point getPosition() const { return m_rect.topLeft(); }
    Size getSize() const { return m_rect.size(); }
    Rect getGeometry() const { return m_rect; }
    int getX() const { return m_rect.x(); }
    int getY() const { return m_rect.y(); }
    int getWidth() const { return m_rect.width(); }
    int getHeight() const { return m_rect.height(); }

    ImagePtr getImage() const { return m_image; }
    FontPtr getFont() const { return m_font; }
    Color getColor() const { return m_color; }
    int getMarginLeft() const { return m_marginLeft; }
    int getMarginRight() const { return m_marginRight; }
    int getMarginTop() const { return m_marginTop; }
    int getMarginBottom() const { return m_marginBottom; }

    UIWidgetList getChildren() const { return m_children; }
    UIWidgetPtr getFocusedChild() const { return m_focusedChild; }
    UIWidgetPtr getChildAfter(const UIWidgetPtr& relativeChild);
    UIWidgetPtr getChildBefore(const UIWidgetPtr& relativeChild);
    UIWidgetPtr getChildById(const std::string& childId);
    UIWidgetPtr getChildByPos(const Point& childPos);
    UIWidgetPtr getChildByIndex(int childIndex);
    UIWidgetPtr recursiveGetChildById(const std::string& childId);
    UIWidgetPtr recursiveGetChildByPos(const Point& childPos);
    UIWidgetPtr backwardsGetWidgetById(const std::string& id);

    void addChild(const UIWidgetPtr& childToAdd);
    void removeChild(const UIWidgetPtr& childToRemove);
    void focusChild(const UIWidgetPtr& childToFocus, FocusReason reason);
    void focusNextChild(FocusReason reason);
    void moveChildToTop(const UIWidgetPtr& childToMove);
    void lockChild(const UIWidgetPtr& childToLock);
    void unlockChild(const UIWidgetPtr& childToUnlock);

    // for using only with anchor layouts
    void addAnchor(AnchorPoint edge, const std::string& targetId, AnchorPoint targetEdge);
    void centerIn(const std::string& targetId);
    void fill(const std::string& targetId);

    UIWidgetPtr asUIWidget() { return std::static_pointer_cast<UIWidget>(shared_from_this()); }

protected:
    /// Triggered when widget is moved or resized
    virtual void onGeometryUpdate(UIGeometryUpdateEvent& event) { }
    // Triggered when widget change visibility/enabled/style/children/parent/layout/...
    //virtual void onChange(const UIEvent& event);
    /// Triggered when widget gets or loses focus
    virtual void onFocusChange(UIFocusEvent& event) { }
    /// Triggered when the mouse enters or leaves widget area
    virtual void onHoverChange(UIHoverEvent& event) { }
    /// Triggered when user presses key while widget has focus
    virtual void onKeyPress(UIKeyEvent& event);
    /// Triggered when user releases key while widget has focus
    virtual void onKeyRelease(UIKeyEvent& event);
    /// Triggered when a mouse button is pressed down while mouse pointer is inside widget area
    virtual void onMousePress(UIMouseEvent& event);
    /// Triggered when a mouse button is released
    virtual void onMouseRelease(UIMouseEvent& event);
    /// Triggered when mouse moves (even when the mouse is outside widget area)
    virtual void onMouseMove(UIMouseEvent& event);
    /// Triggered when mouse middle button wheels inside widget area
    virtual void onMouseWheel(UIMouseEvent& event);

    friend class UIManager;

private:
    void destroyCheck();

protected:
    UIWidgetType m_type;
    bool m_enabled;
    bool m_visible;
    bool m_hovered;
    bool m_focusable;
    bool m_destroyed;
    bool m_updateScheduled;
    Rect m_rect;
    UILayoutPtr m_layout;
    UIWidgetWeakPtr m_parent;
    UIWidgetList m_children;
    UIWidgetList m_lockedWidgets;
    UIWidgetPtr m_focusedChild;
    std::string m_id;

    // basic style components used by all widgets
    ImagePtr m_image;
    FontPtr m_font;
    Color m_color;
    int m_marginLeft;
    int m_marginRight;
    int m_marginTop;
    int m_marginBottom;
};

#endif
