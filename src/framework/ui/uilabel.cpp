#include <global.h>
#include <ui/uilabel.h>

void UILabel::setText(const std::string& text)
{
    m_text = text;
    // text size changed, reaplly skin
    getSkin()->apply(this);
}
