#include "uilineedit.h"
#include <graphics/font.h>
#include <otml/otmlnode.h>

UILineEdit::UILineEdit() : UIWidget(UITypeLabel)
{
}

UILineEditPtr UILineEdit::create()
{
    UILineEditPtr lineEdit(new UILineEdit);
    lineEdit->setStyle("LineEdit");
    return lineEdit;
}

void UILineEdit::loadStyleFromOTML(const OTMLNodePtr& styleNode)
{
    UIWidget::loadStyleFromOTML(styleNode);

    m_textArea.setFont(getFont());
    m_textArea.setColor(getColor());
    setText(styleNode->readAt("text", getText()));
}

void UILineEdit::render()
{
    UIWidget::render();
    m_textArea.draw();
}

void UILineEdit::onGeometryUpdate()
{
    m_textArea.setScreenCoords(getGeometry());
}
