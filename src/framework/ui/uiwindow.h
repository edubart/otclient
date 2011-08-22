#ifndef UIWINDOW_H
#define UIWINDOW_H

#include "uiwidget.h"

class UIWindow : public UIWidget
{
public:
    UIWindow();

    static UIWindowPtr create();

    virtual void onStyleApply(const OTMLNodePtr& styleNode);
    virtual void render();

    void setTitle(const std::string& title) { m_title = title; }
    std::string getTitle() const { return m_title; }

protected:
    virtual void onGeometryUpdate(const Rect& oldRect, const Rect& newRect);
    virtual void onFocusChange(bool focused, UI::FocusReason reason);
    virtual bool onMousePress(const Point& mousePos, UI::MouseButton button);
    virtual bool onMouseRelease(const Point& mousePos, UI::MouseButton button);
    virtual bool onMouseMove(const Point& mousePos, const Point& mouseMoved);

private:
    std::string m_title;
    bool m_moving;
    Point m_movingReference;

    // styling
    BorderImagePtr m_headImage;
    ImagePtr m_bodyImage;
    int m_headHeight;
    int m_headMargin;
    AlignmentFlag m_titleAlign;
};

#endif
