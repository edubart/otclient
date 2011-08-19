#include "uilabel.h"
#include <framework/graphics/font.h>
#include <framework/otml/otmlnode.h>

UILabel::UILabel() : UIWidget(UITypeLabel)
{
    m_align = AlignLeft;
}

UILabelPtr UILabel::create()
{
    UILabelPtr label(new UILabel);
    label->setStyle("Label");
    return label;
}

void UILabel::loadStyleFromOTML(const OTMLNodePtr& styleNode)
{
    UIWidget::loadStyleFromOTML(styleNode);

    m_text = styleNode->valueAt("text", m_text);

    if(styleNode->hasChildAt("align"))
        m_align = fw::translateAlignment(styleNode->valueAt("align"));

    // auto resize if no size supplied
    if(!m_text.empty() && !getGeometry().isValid())
        resizeToText();
}

void UILabel::render()
{
    m_font->renderText(m_text, m_rect, m_align, m_fontColor);
}

void UILabel::resizeToText()
{
    resize(getFont()->calculateTextRectSize(m_text));
}
