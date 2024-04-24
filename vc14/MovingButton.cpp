#include "MovingButton.h"
#include <Math.h>
#include <random>

void MovingButton::drawSelf(Fw::DrawPane drawPane)
{
    //Only Draw on Foreground
    if ((drawPane & Fw::BackgroundPane) == 0)
        return;

    //appeal to parent class behaviour for drawSelf
    UIWidget::drawSelf(drawPane);

    //TODO: Additional Behaviours upon draw call?
}

bool MovingButton::onMousePress(const Point& mousePos, Fw::MouseButton button) {
    //quick data cache
    int random = rand();
  
    UIWidget:setY(random%100);
};

void MovingButton::onStyleApply(const std::string& styleName, const OTMLNodePtr& styleNode) {

    //get the scalar width and cache 10% of it.
    float tenPercent = m_rect.width();

    UIWidget::setX(UIWidget::getX()-tenPercent);
};

