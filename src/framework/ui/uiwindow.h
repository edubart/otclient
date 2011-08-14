#ifndef UIWINDOW_H
#define UIWINDOW_H

#include "uiwidget.h"

class UIWindow : public UIWidget
{
public:
    UIWindow();

    static UIWindowPtr create();

    virtual void loadStyleFromOTML(const OTMLNodePtr& styleNode);
    virtual void render();

    void setTitle(const std::string& title) { m_title = title; }
    std::string getTitle() const { return m_title; }

protected:
    virtual void onGeometryUpdate();
    virtual void onMousePress(const UIMouseEvent& event);
    virtual void onMouseRelease(const UIMouseEvent& event);
    virtual void onMouseMove(const UIMouseEvent& event);

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
