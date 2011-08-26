#include "uiwindow.h"
#include <framework/graphics/borderimage.h>
#include <framework/graphics/font.h>
#include <framework/graphics/graphics.h>
#include <framework/otml/otml.h>

void UIWindow::setup()
{
    UIWidget::setup();
    m_moving = false;
    m_headHeight = 0;
    m_headMargin = 0;
    m_titleAlign = AlignCenter;
}

void UIWindow::render()
{
    // draw window head
    Rect headRect = getRect();
    headRect.setHeight(m_headHeight);

    if(m_headImage && m_headHeight > 0) {
        g_graphics.bindColor(m_backgroundColor);
        m_headImage->draw(headRect);

        // draw window head text
        Rect headTextRect = headRect;
        if(m_titleAlign & AlignLeft)
            headTextRect.addLeft(-m_headMargin);
        else if(m_titleAlign & AlignRight)
            headTextRect.addRight(-m_headMargin);
        m_font->renderText(m_title, headTextRect, m_titleAlign, m_foregroundColor);
    }

    // draw window body
    Rect bodyRect = getRect();
    bodyRect.setTop(headRect.bottom() + 1);
    if(m_bodyImage) {
        g_graphics.bindColor(m_backgroundColor);
        m_bodyImage->draw(bodyRect);
    }

    // render children
    UIWidget::render();
}

void UIWindow::onStyleApply(const OTMLNodePtr& styleNode)
{
    UIWidget::onStyleApply(styleNode);

    for(OTMLNodePtr node : styleNode->children()) {
        if(node->tag() == "head") {
            if(OTMLNodePtr cnode = node->get("border-image"))
                m_headImage = BorderImage::loadFromOTML(cnode);
            m_headHeight = node->valueAt("height", m_headImage->getDefaultSize().height());
            m_headMargin = node->valueAt("margin", 0);
            m_titleAlign = fw::translateAlignment(node->valueAt("text align", std::string("center")));
        }
        else if(node->tag() == "body") {
            if(OTMLNodePtr cnode = node->get("border-image"))
                m_bodyImage = BorderImage::loadFromOTML(cnode);
        }
        else if(node->tag() == "title") {
            setTitle(node->value());
        }
    }
}

void UIWindow::onGeometryUpdate(const Rect& oldRect, const Rect& newRect)
{
    // bind window rect to parent rect
    Rect boundRect = newRect;
    UIWidgetPtr parent = getParent();
    if(parent) {
        Rect parentRect = parent->getRect();
        if(boundRect.left() < parentRect.left())
            boundRect.moveLeft(parentRect.left());
        if(boundRect.top() < parentRect.top())
            boundRect.moveTop(parentRect.top());
        if(boundRect.bottom() > parentRect.bottom())
            boundRect.moveBottom(parentRect.bottom());
        if(boundRect.right() > parentRect.right())
            boundRect.moveRight(parentRect.right());
    }

    if(boundRect != newRect)
        setRect(boundRect);
}

void UIWindow::onFocusChange(bool focused, FocusReason reason)
{
    // when a window is focused it goes to the top
    if(focused) {
        if(UIWidgetPtr parent = getParent())
            parent->moveChildToTop(asUIWidget());
    }
}

bool UIWindow::onMousePress(const Point& mousePos, MouseButton button)
{
    if(!getChildByPos(mousePos)) {
        m_moving = true;
        m_movingReference = mousePos - getRect().topLeft();
        return true;
    }
    return UIWidget::onMousePress(mousePos, button);
}

bool UIWindow::onMouseRelease(const Point& mousePos, MouseButton button)
{
    if(m_moving) {
        m_moving = false;
        return true;
    }
    return UIWidget::onMouseRelease(mousePos, button);
}

bool UIWindow::onMouseMove(const Point& mousePos, const Point& mouseMoved)
{
    if(m_moving) {
        moveTo(mousePos - m_movingReference);
        return true;
    }
    return UIWidget::onMouseMove(mousePos, mouseMoved);
}
