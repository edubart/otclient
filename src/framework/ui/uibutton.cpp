#include "uibutton.h"
#include <framework/graphics/borderimage.h>
#include <framework/graphics/font.h>
#include <framework/otml/otmlnode.h>
#include <framework/luascript/luainterface.h>
#include <framework/graphics/graphics.h>

void UIButton::setup()
{
    UIWidget::setup();
    setFocusable(false);

    // by default, all callbacks call lua fields
    m_onClick = [this]() { this->callLuaField("onClick"); };
}

void UIButton::render()
{
    UIWidget::render();
    Rect textRect = m_rect;
    textRect.translate(m_textTranslate);
    m_font->renderText(m_text, textRect, AlignCenter, m_foregroundColor);
}

void UIButton::onStyleApply(const OTMLNodePtr& styleNode)
{
    UIWidget::onStyleApply(styleNode);

    for(OTMLNodePtr node : styleNode->children()) {
        if(node->tag() == "text-translate") {
            m_textTranslate = node->value<Point>();
        } else if(node->tag() == "text") {
            m_text = node->value();
        } else if(node->tag() == "onClick") {
            g_lua.loadFunction(node->value(), "@" + node->source() + "[" + node->tag() + "]");
            luaSetField(node->tag());
        }
    }
}

bool UIButton::onMouseRelease(const Point& mousePos, MouseButton button)
{
    if(isPressed()) {
        if(m_onClick && getRect().contains(mousePos))
            m_onClick();
        return true;
    }
    return false;
}
