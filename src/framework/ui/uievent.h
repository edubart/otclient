#ifndef UIEVENT_H
#define UIEVENT_H

#include <framework/platform/platformevent.h>
#include "declarations.h"

class UIEvent
{
public:
    UIEvent() : m_accepted(true) { }

    void setAccepted(bool accepted) { m_accepted = accepted; }
    void accept() { m_accepted = true; }
    void ignore() { m_accepted = false; }

    bool isAccepted() { return m_accepted; }

private:
    bool m_accepted;
};

class UIMouseEvent : public UIEvent
{
public:
    UIMouseEvent(const Point& pos, MouseButton button)
        : m_pos(pos), m_button(button), m_wheelDirection(MouseNoWheel) { }
    UIMouseEvent(const Point& pos, MouseWheelDirection wheelDirection)
        : m_pos(pos), m_button(MouseNoButton), m_wheelDirection(wheelDirection) { }
    UIMouseEvent(const Point& pos, Point moved)
        : m_pos(pos), m_moved(moved), m_button(MouseNoButton), m_wheelDirection(MouseNoWheel) { }

    Point pos() const { return m_pos; }
    Point oldPos() const { return m_pos - m_moved; }
    Point moved() const { return m_moved; }
    int x() const { return m_pos.x; }
    int y() const { return m_pos.y; }
    MouseButton button() const { return m_button; }
    MouseWheelDirection wheelDirection() const { return m_wheelDirection; }

private:
    Point m_pos;
    Point m_moved;
    MouseButton m_button;
    MouseWheelDirection m_wheelDirection;
};

class UIKeyEvent : public UIEvent
{
public:
    UIKeyEvent(uchar keyCode, char keyChar, int keyboardModifiers)
        : m_keyCode(keyCode), m_keyChar(keyChar), m_keyboardModifiers(keyboardModifiers) { }

    uchar keyCode() const { return m_keyCode; }
    char keyChar() const  { return m_keyChar; }
    int keyboardModifiers() const { return m_keyboardModifiers; }

private:
    uchar m_keyCode;
    char m_keyChar;
    int m_keyboardModifiers;
};

class UIFocusEvent : public UIEvent
{
public:
    UIFocusEvent(FocusReason reason, bool gotFocus)
        : m_reason(reason), m_gotFocus(gotFocus) { }

    bool gotFocus() const { return m_gotFocus; }
    bool lostFocus() const { return !m_gotFocus; }
    bool reason() const { return m_reason; }

private:
    FocusReason m_reason;
    bool m_gotFocus;
};

class UIHoverEvent : public UIEvent
{
public:
    UIHoverEvent(bool hovered) : m_hovered(hovered) { }

    bool hovered() const { return m_hovered; }
    bool mouseEnter() const { return m_hovered; }
    bool mouseLeave() const { return !m_hovered; }

private:
    bool m_hovered;
};

class UIRectUpdateEvent : public UIEvent
{
public:
    UIRectUpdateEvent(const Rect& oldRect, const Rect& rect)
        : m_oldRect(oldRect), m_rect(rect) { }

    Point pos() const { return m_rect.topLeft(); }
    Size size() const { return m_rect.size(); }
    Rect rect() const { return m_rect; }
    Point oldPos() const { return m_oldRect.topLeft(); }
    Size oldSize() const { return m_oldRect.size(); }
    Rect oldRect() const { return m_oldRect; }
    Point moved() const { return m_rect.topLeft() - m_oldRect.topLeft(); }
    Size resized() const { return m_rect.size() - m_oldRect.size(); }
    bool hasMoved() const { return m_rect.topLeft() != m_oldRect.topLeft(); }
    bool hasResized() const { return m_rect.size() != m_oldRect.size(); }

private:
    Rect m_oldRect;
    Rect m_rect;
};

#endif
