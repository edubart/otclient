#ifndef UILIST_H
#define UILIST_H

#include "uiwidget.h"

class UIList : public UIWidget
{
public:
    UIList();

    virtual void loadStyleFromOTML(const OTMLNodePtr& styleNode);
    virtual void render();

protected:
    virtual void onKeyPress(UIKeyEvent& event);
    virtual void onMousePress(UIMouseEvent& event);
    virtual void onMouseMove(UIMouseEvent& event);

private:
    std::list<std::string> m_items;
};

#endif
