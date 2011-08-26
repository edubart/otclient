#ifndef UIBUTTON_H
#define UIBUTTON_H

#include "uiwidget.h"

class UIButton : public UIWidget
{
public:
    virtual void setup();
    virtual void render();

    void setOnClick(const SimpleCallback& onClick) { m_onClick = onClick; }
    void setText(const std::string& text) { m_text = text; }

    SimpleCallback getOnClick() const { return m_onClick; }
    std::string getText() const { return m_text; }

    UIButtonPtr asUIButton() { return std::static_pointer_cast<UIButton>(shared_from_this()); }

protected:
    virtual void onStyleApply(const OTMLNodePtr& styleNode);
    virtual bool onMouseRelease(const Point& mousePos, MouseButton button);

    SimpleCallback m_onClick;
    Point m_textTranslate;
    std::string m_text;
};

#endif
