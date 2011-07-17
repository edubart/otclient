#include <global.h>
#include <core/dispatcher.h>
#include <ui/uibutton.h>
#include "uicontainer.h"

void UIButton::onInputEvent(const InputEvent& event)
{
    if(event.type == EV_MOUSE_LDOWN && getRect().contains(event.mousePos)) {
        m_state = ButtonDown;
    } else if(event.type == EV_MOUSE_LUP && m_state == ButtonDown) {
        m_state = ButtonUp;
        if(getRect().contains(event.mousePos)) {
            g_dispatcher.addTask(boost::bind(&ScriptObject::callScriptTableField, shared_from_this(), "onClick", 0));
        }
    } else if(event.type == EV_MOUSE_MOVE && m_state != ButtonDown) {
        if(isMouseOver())
            m_state = ButtonMouseOver;
        else
            m_state = ButtonUp;
    }
}
