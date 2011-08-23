#include "uilabel.h"
#include <framework/graphics/font.h>
#include <framework/otml/otmlnode.h>

UILabel::UILabel()
{
    m_align = AlignLeft;
    m_focusable = false;
}

void UILabel::onStyleApply(const OTMLNodePtr& styleNode)
{
    UIWidget::onStyleApply(styleNode);

    for(const OTMLNodePtr& node : styleNode->children()) {
        if(node->tag() == "text")
            setText(node->value());
        else if(node->tag() == "align")
            setAlign(fw::translateAlignment(styleNode->value()));
    }
}

void UILabel::render()
{
    UIWidget::render();
    m_font->renderText(m_text, m_rect, m_align, m_foregroundColor);
}

void UILabel::setText(const std::string& text)
{
    m_text = text;

    // auto resize if the current rect is invalid
    if(!m_rect.isValid()) {
        Size textSize = m_font->calculateTextRectSize(m_text);
        if(m_rect.width() <= 0)
            m_rect.setWidth(textSize.width());
        if(m_rect.height() <= 0)
            m_rect.setHeight(textSize.height());
    }
}

void UILabel::resizeToText()
{
    resize(m_font->calculateTextRectSize(m_text));
}
