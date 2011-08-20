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
    return label;
}

void UILabel::loadStyleFromOTML(const OTMLNodePtr& styleNode)
{
    UIWidget::loadStyleFromOTML(styleNode);

    m_text = styleNode->valueAt("text", m_text);

    if(styleNode->hasChildAt("align"))
        m_align = fw::translateAlignment(styleNode->valueAt("align"));

    // auto resize if needed
    if(!m_text.empty() && !m_rect.isValid()) {
        Size textSize = m_font->calculateTextRectSize(m_text);
        if(m_rect.width() <= 0)
            m_rect.setWidth(textSize.width());
        if(m_rect.height() <= 0)
            m_rect.setHeight(textSize.height());
    }
}

void UILabel::render()
{
    UIWidget::render();
    m_font->renderText(m_text, m_rect, m_align, m_foregroundColor);
}

void UILabel::resizeToText()
{
    resize(m_font->calculateTextRectSize(m_text));
}
