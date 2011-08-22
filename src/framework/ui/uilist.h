#ifndef UILIST_H
#define UILIST_H

#include "uiwidget.h"

class UIList : public UIWidget
{
public:
    UIList();

    virtual void onStyleApply(const OTMLNodePtr& styleNode);
    virtual void render();

protected:
    virtual bool onKeyPress(uchar keyCode, char keyChar, int keyboardModifiers);
    virtual bool onMousePress(const Point& mousePos, UI::MouseButton button);
    virtual bool onMouseMove(const Point& mousePos, const Point& mouseMoved);

private:
    std::list<std::string> m_items;
};

#endif
