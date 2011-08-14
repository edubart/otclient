#include "uibutton.h"
#include <graphics/borderimage.h>
#include <graphics/font.h>
#include <otml/otmlnode.h>
#include <luascript/luainterface.h>

UIButton::UIButton(): UIWidget(UITypeButton)
{
    m_state = ButtonUp;

    // by default, all callbacks call lua fields
    m_onClick = [this]() { this->callLuaField("onClick"); };
}

UIButtonPtr UIButton::create()
{
    UIButtonPtr button(new UIButton);
    button->setStyle("Button");
    return button;
}

void UIButton::loadStyleFromOTML(const OTMLNodePtr& styleNode)
{
    UIWidget::loadStyleFromOTML(styleNode);

    for(int i=0; i<3; ++i) {
        m_statesStyle[i].image = m_image;
        m_statesStyle[i].color = m_color;
        m_statesStyle[i].fontColor = m_fontColor;
        m_statesStyle[i].textTranslate = Point(0,0);
    }

    if(OTMLNodePtr node = styleNode->get("state.up"))
        loadStateStyle(m_statesStyle[ButtonUp], node);
    if(OTMLNodePtr node = styleNode->get("state.hover"))
        loadStateStyle(m_statesStyle[ButtonHover], node);
    if(OTMLNodePtr node = styleNode->get("state.down"))
        loadStateStyle(m_statesStyle[ButtonDown], node);

    m_text = styleNode->readAt("text", aux::empty_string);

    if(OTMLNodePtr node = styleNode->get("onClick")) {
        g_lua.loadFunction(node->read<std::string>(), "@" + node->source() + "[" + node->tag() + "]");
        luaSetField("onClick");
    }
}

void UIButton::loadStateStyle(ButtonStateStyle& stateStyle, const OTMLNodePtr& stateStyleNode)
{
    if(OTMLNodePtr node = stateStyleNode->get("border-image"))
        stateStyle.image = BorderImage::loadFromOTML(node);
    if(OTMLNodePtr node = stateStyleNode->get("image"))
        stateStyle.image = Image::loadFromOTML(node);
    stateStyle.textTranslate = stateStyleNode->readAt("text-translate", Point());
    stateStyle.color = stateStyleNode->readAt("font-color", m_fontColor);
    stateStyle.color = stateStyleNode->readAt("color", m_color);
}

void UIButton::render()
{
    const ButtonStateStyle& currentStyle = m_statesStyle[m_state];
    Rect textRect = getGeometry();

    if(currentStyle.image)
        currentStyle.image->draw(textRect);

    textRect.translate(currentStyle.textTranslate);
    getFont()->renderText(m_text, textRect, AlignCenter, currentStyle.fontColor);
}

void UIButton::onHoverChange(UIHoverEvent& event)
{
    if(event.mouseEnter() && m_state == ButtonUp)
        m_state = ButtonHover;
    else if(m_state == ButtonHover)
        m_state = ButtonUp;
}

void UIButton::onMousePress(UIMouseEvent& event)
{
    if(event.button() == MouseLeftButton) {
        m_state = ButtonDown;
    } else
        event.ignore();
}

void UIButton::onMouseRelease(UIMouseEvent& event)
{
    if(m_state == ButtonDown) {
        if(m_onClick && getGeometry().contains(event.pos()))
            m_onClick();
        m_state = (isHovered() && isEnabled()) ? ButtonHover : ButtonUp;
    } else
        event.ignore();
}
