#include <global.h>
#include <ui/uilabel.h>
#include <ui/uielementskin.h>

void UILabel::setText(const std::string& text)
{
    m_text = text;
    // text size changed, reaplly skin
    if(getSkin())
        getSkin()->apply(this);
}
