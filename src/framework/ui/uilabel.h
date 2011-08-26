#ifndef UILABEL_H
#define UILABEL_H

#include "uiwidget.h"

class UILabel : public UIWidget
{
public:
    virtual void setup();
    virtual void render();

    void resizeToText();

    void setText(const std::string& text);
    void setAlign(AlignmentFlag align) { m_align = align; }
    void setOffset(const Point& offset) { m_offset = offset; }

    std::string getText() const { return m_text; }
    AlignmentFlag getAlign() const { return m_align; }
    Point getOffset() const { return m_offset; }

protected:
    virtual void onStyleApply(const OTMLNodePtr& styleNode);

private:
    std::string m_text;
    Point m_offset;
    AlignmentFlag m_align;
};

#endif
