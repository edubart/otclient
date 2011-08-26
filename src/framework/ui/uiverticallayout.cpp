#include "uiverticallayout.h"
#include "uiwidget.h"

UIVerticalLayout::UIVerticalLayout(UIWidgetPtr parentWidget)
    : UILayout(parentWidget)
{
}

void UIVerticalLayout::update()
{
    UIWidgetPtr parentWidget = getParentWidget();
    UIWidgetList widgets = parentWidget->getChildren();
    Point pos = parentWidget->getPosition();
    for(const UIWidgetPtr& widget : widgets) {
        Size size = widget->getSize();
        pos.y += widget->getMarginTop();
        if(widget->isSizeFixed()) {
            pos.x = parentWidget->getX() + (parentWidget->getWidth() - (widget->getMarginLeft() + widget->getWidth() + widget->getMarginRight()))/2;
            pos.x = std::max(pos.x, parentWidget->getX());
        } else {
            size.setWidth(parentWidget->getWidth() - (widget->getMarginLeft() + widget->getMarginRight()));
            pos.x = std::max(pos.x, parentWidget->getX() + (parentWidget->getWidth() - size.width())/2);
        }
        widget->setRect(Rect(pos, size));
        pos.y += widget->getHeight() + widget->getMarginBottom();
    }
}

void UIVerticalLayout::addWidget(const UIWidgetPtr& widget)
{
    update();
}

void UIVerticalLayout::removeWidget(const UIWidgetPtr& widget)
{
    update();
}
