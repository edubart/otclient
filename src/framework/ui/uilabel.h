#ifndef UILABEL_H
#define UILABEL_H

#include <global.h>
#include <ui/uielement.h>
#include <graphics/font.h>

class UILabel : public UIElement
{
public:
    UILabel() :
        UIElement(UI::Label),
        m_align(AlignLeftCenter) { }

    void setText(const std::string& text);
     std::string getText() const { return m_text; }

    void setAlign(AlignmentFlag align) { m_align = align; }
    AlignmentFlag getAlign() const { return m_align; }

    virtual const char *getScriptObjectType() const { return "UILabel"; }

private:
    std::string m_text;
    AlignmentFlag m_align;
};

typedef boost::shared_ptr<UILabel> UILabelPtr;

#endif // UILABEL_H
