#include "uilabel.h"
#include <graphics/font.h>
#include <otml/otmlnode.h>

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

    m_text = styleNode->readAt("text", m_text);

    if(styleNode->hasChild("align"))
        m_align = parseAlignment(styleNode->readAt<std::string>("align"));

    // auto resize if no size supplied
    if(!m_text.empty() && !getGeometry().isValid())
        resizeToText();
}

void UILabel::render()
{
    getFont()->renderText(m_text, getGeometry(), m_align, m_fontColor);
}

void UILabel::resizeToText()
{
    resize(getFont()->calculateTextRectSize(m_text));
}
