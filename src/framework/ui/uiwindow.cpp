#include "uiwindow.h"
#include <graphics/borderimage.h>
#include <graphics/font.h>
#include <otml/otml.h>

UIWindow::UIWindow(): UIWidget(UITypeWindow)
{
    m_moving = false;
    setFocusable(true);
}

UIWindowPtr UIWindow::create()
{
    UIWindowPtr window(new UIWindow);
    window->setStyle("Window");
    return window;
}

void UIWindow::loadStyleFromOTML(const OTMLNodePtr& styleNode)
{
    UIWidget::loadStyleFromOTML(styleNode);

    if(OTMLNodePtr headNode = styleNode->get("head")) {
        if(OTMLNodePtr node = headNode->get("border-image"))
            m_headImage = BorderImage::loadFromOTML(node);
        m_headHeight = headNode->readAt("height", m_headImage->getDefaultSize().height());
        m_headMargin = headNode->readAt("margin", 0);
        m_titleAlign = parseAlignment(headNode->readAt("text align", std::string("center")));
    } else {
        m_headHeight = 0;
        m_headMargin = 0;
        m_titleAlign = AlignCenter;
    }

    if(OTMLNodePtr bodyNode = styleNode->get("body")) {
        if(OTMLNodePtr node = bodyNode->get("border-image"))
            m_bodyImage = BorderImage::loadFromOTML(node);
    }

    m_title = styleNode->readAt("title", aux::empty_string);
}

void UIWindow::render()
{
    // draw window head
    Rect headRect = getGeometry();
    headRect.setHeight(m_headHeight);

    if(m_headImage && m_headHeight > 0) {
        m_headImage->draw(headRect);

        // draw window head text
        Rect headTextRect = headRect;
        if(m_titleAlign & AlignLeft)
            headTextRect.addLeft(-m_headMargin);
        else if(m_titleAlign & AlignRight)
            headTextRect.addRight(-m_headMargin);
        getFont()->renderText(m_title, headTextRect, m_titleAlign, getColor());
    }

    // draw window body
    Rect bodyRect = getGeometry();
    bodyRect.setTop(headRect.bottom() + 1);
    if(m_bodyImage)
        m_bodyImage->draw(bodyRect);

    // render children
    UIWidget::render();
}

void UIWindow::onGeometryUpdate(UIGeometryUpdateEvent& event)
{
    // bind window rect to parent rect
    Rect boundRect = event.rect();
    UIWidgetPtr parent = getParent();
    if(parent) {
        Rect parentRect = parent->getGeometry();
        if(boundRect.left() < parentRect.left())
            boundRect.moveLeft(parentRect.left());
        if(boundRect.top() < parentRect.top())
            boundRect.moveTop(parentRect.top());
        if(boundRect.bottom() > parentRect.bottom())
            boundRect.moveBottom(parentRect.bottom());
        if(boundRect.right() > parentRect.right())
            boundRect.moveRight(parentRect.right());
    }

    if(boundRect != event.rect())
        setGeometry(boundRect);
}

void UIWindow::onMousePress(UIMouseEvent& event)
{
    Rect headRect = getGeometry();
    headRect.setHeight(m_headHeight);
    if(headRect.contains(event.pos())) {
        m_moving = true;
        m_movingReference = event.pos() - getGeometry().topLeft();
    } else
        event.ignore();
}

void UIWindow::onMouseRelease(UIMouseEvent& event)
{
    if(m_moving)
        m_moving = false;
    else
        event.ignore();
}

void UIWindow::onMouseMove(UIMouseEvent& event)
{
    if(m_moving)
        move(event.pos() - m_movingReference);
    else
        event.ignore();
}
