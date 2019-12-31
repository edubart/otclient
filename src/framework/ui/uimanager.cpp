/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "uimanager.h"
#include "ui.h"

#include <framework/otml/otml.h>
#include <framework/graphics/graphics.h>
#include <framework/platform/platformwindow.h>
#include <framework/core/eventdispatcher.h>
#include <framework/core/application.h>
#include <framework/core/resourcemanager.h>

UIManager g_ui;

void UIManager::init()
{
    // creates root widget
    m_rootWidget = UIWidgetPtr(new UIWidget);
    m_rootWidget->setId("root");
    m_mouseReceiver = m_rootWidget;
    m_keyboardReceiver = m_rootWidget;
}

void UIManager::terminate()
{
    // destroy root widget and its children
    m_rootWidget->destroy();
    m_mouseReceiver = nullptr;
    m_keyboardReceiver = nullptr;
    m_rootWidget = nullptr;
    m_draggingWidget = nullptr;
    m_hoveredWidget = nullptr;
    m_pressedWidget = nullptr;
    m_styles.clear();
    m_destroyedWidgets.clear();
    m_checkEvent = nullptr;
}

void UIManager::render(Fw::DrawPane drawPane)
{
    m_rootWidget->draw(m_rootWidget->getRect(), drawPane);
}

void UIManager::resize(const Size& size)
{
    m_rootWidget->setSize(g_window.getSize());
}

void UIManager::inputEvent(const InputEvent& event)
{
    UIWidgetList widgetList;
    switch(event.type) {
        case Fw::KeyTextInputEvent:
            m_keyboardReceiver->propagateOnKeyText(event.keyText);
            break;
        case Fw::KeyDownInputEvent:
            m_keyboardReceiver->propagateOnKeyDown(event.keyCode, event.keyboardModifiers);
            break;
        case Fw::KeyPressInputEvent:
            m_keyboardReceiver->propagateOnKeyPress(event.keyCode, event.keyboardModifiers, event.autoRepeatTicks);
            break;
        case Fw::KeyUpInputEvent:
            m_keyboardReceiver->propagateOnKeyUp(event.keyCode, event.keyboardModifiers);
            break;
        case Fw::MousePressInputEvent:
            if(event.mouseButton == Fw::MouseLeftButton && m_mouseReceiver->isVisible()) {
                UIWidgetPtr pressedWidget = m_mouseReceiver->recursiveGetChildByPos(event.mousePos, false);
                if(pressedWidget && !pressedWidget->isEnabled())
                    pressedWidget = nullptr;
                updatePressedWidget(pressedWidget, event.mousePos);
            }

            m_mouseReceiver->propagateOnMouseEvent(event.mousePos, widgetList);
            for(const UIWidgetPtr& widget : widgetList) {
                widget->recursiveFocus(Fw::MouseFocusReason);
                if(widget->onMousePress(event.mousePos, event.mouseButton))
                    break;
            }

            break;
        case Fw::MouseReleaseInputEvent: {
            // release dragging widget
            bool accepted = false;
            if(m_draggingWidget && event.mouseButton == Fw::MouseLeftButton)
                accepted = updateDraggingWidget(nullptr, event.mousePos);

            if(!accepted) {
                m_mouseReceiver->propagateOnMouseEvent(event.mousePos, widgetList);

                // mouse release is always fired first on the pressed widget
                if(m_pressedWidget) {
                    auto it = std::find(widgetList.begin(), widgetList.end(), m_pressedWidget);
                    if(it != widgetList.end())
                        widgetList.erase(it);
                    widgetList.push_front(m_pressedWidget);
                }

                for(const UIWidgetPtr& widget : widgetList) {
                    if(widget->onMouseRelease(event.mousePos, event.mouseButton))
                        break;
                }
            }

            if(m_pressedWidget && event.mouseButton == Fw::MouseLeftButton)
                updatePressedWidget(nullptr, event.mousePos, !accepted);
            break;
        }
        case Fw::MouseMoveInputEvent: {
            // start dragging when moving a pressed widget
            if(m_pressedWidget && m_pressedWidget->isDraggable() && m_draggingWidget != m_pressedWidget) {
                // only drags when moving more than 4 pixels
                if((event.mousePos - m_pressedWidget->getLastClickPosition()).length() >= 4)
                    updateDraggingWidget(m_pressedWidget, event.mousePos - event.mouseMoved);
            }

            // mouse move can change hovered widgets
            updateHoveredWidget(true);

            // first fire dragging move
            if(m_draggingWidget) {
                if(m_draggingWidget->onDragMove(event.mousePos, event.mouseMoved))
                    break;
            }

            if(m_pressedWidget) {
                if(m_pressedWidget->onMouseMove(event.mousePos, event.mouseMoved)) {
                    break;
                }
            }

            m_mouseReceiver->propagateOnMouseMove(event.mousePos, event.mouseMoved, widgetList);
            for(const UIWidgetPtr& widget : widgetList) {
                if(widget->onMouseMove(event.mousePos, event.mouseMoved))
                    break;
            }
            break;
        }
        case Fw::MouseWheelInputEvent:
            m_rootWidget->propagateOnMouseEvent(event.mousePos, widgetList);
            for(const UIWidgetPtr& widget : widgetList) {
                if(widget->onMouseWheel(event.mousePos, event.wheelDirection))
                    break;
            }
            break;
        default:
            break;
    };
}

void UIManager::updatePressedWidget(const UIWidgetPtr& newPressedWidget, const Point& clickedPos, bool fireClicks)
{
    UIWidgetPtr oldPressedWidget = m_pressedWidget;
    m_pressedWidget = newPressedWidget;

    // when releasing mouse inside pressed widget area send onClick event
    if(fireClicks && oldPressedWidget && oldPressedWidget->isEnabled() && oldPressedWidget->containsPoint(clickedPos))
        oldPressedWidget->onClick(clickedPos);

    if(newPressedWidget)
        newPressedWidget->updateState(Fw::PressedState);

    if(oldPressedWidget)
        oldPressedWidget->updateState(Fw::PressedState);
}

bool UIManager::updateDraggingWidget(const UIWidgetPtr& draggingWidget, const Point& clickedPos)
{
    bool accepted = false;

    UIWidgetPtr oldDraggingWidget = m_draggingWidget;
    m_draggingWidget = nullptr;
    if(oldDraggingWidget) {
        UIWidgetPtr droppedWidget;
        if(!clickedPos.isNull()) {
            auto clickedChildren = m_rootWidget->recursiveGetChildrenByPos(clickedPos);
            for(const UIWidgetPtr& child : clickedChildren) {
                if(child->onDrop(oldDraggingWidget, clickedPos)) {
                    droppedWidget = child;
                    break;
                }
            }
        }

        accepted = oldDraggingWidget->onDragLeave(droppedWidget, clickedPos);
        oldDraggingWidget->updateState(Fw::DraggingState);
    }

    if(draggingWidget) {
        if(draggingWidget->onDragEnter(clickedPos)) {
            m_draggingWidget = draggingWidget;
            draggingWidget->updateState(Fw::DraggingState);
            accepted = true;
        }
    }

    return accepted;
}

void UIManager::updateHoveredWidget(bool now)
{
    if(m_hoverUpdateScheduled && !now)
        return;

    auto func = [this] {
        if(!m_rootWidget)
            return;

        m_hoverUpdateScheduled = false;
        UIWidgetPtr hoveredWidget;
        //if(!g_window.isMouseButtonPressed(Fw::MouseLeftButton) && !g_window.isMouseButtonPressed(Fw::MouseRightButton)) {
            hoveredWidget = m_rootWidget->recursiveGetChildByPos(g_window.getMousePosition(), false);
            if(hoveredWidget && !hoveredWidget->isEnabled())
                hoveredWidget = nullptr;
        //}

        if(hoveredWidget != m_hoveredWidget) {
            UIWidgetPtr oldHovered = m_hoveredWidget;
            m_hoveredWidget = hoveredWidget;
            if(oldHovered) {
                oldHovered->updateState(Fw::HoverState);
                oldHovered->onHoverChange(false);
            }
            if(hoveredWidget) {
                hoveredWidget->updateState(Fw::HoverState);
                hoveredWidget->onHoverChange(true);
            }
        }
    };

    if(now)
        func();
    else {
        m_hoverUpdateScheduled = true;
        g_dispatcher.addEvent(func);
    }
}

void UIManager::onWidgetAppear(const UIWidgetPtr& widget)
{
    if(widget->containsPoint(g_window.getMousePosition()))
        updateHoveredWidget();
}

void UIManager::onWidgetDisappear(const UIWidgetPtr& widget)
{
    if(widget->containsPoint(g_window.getMousePosition()))
        updateHoveredWidget();
}

void UIManager::onWidgetDestroy(const UIWidgetPtr& widget)
{
    // release input grabs
    if(m_keyboardReceiver == widget)
        resetKeyboardReceiver();

    if(m_mouseReceiver == widget)
        resetMouseReceiver();

    if(m_hoveredWidget == widget)
        updateHoveredWidget();

    if(m_pressedWidget == widget)
        updatePressedWidget(nullptr);

    if(m_draggingWidget == widget)
        updateDraggingWidget(nullptr);

#ifndef NDEBUG
    if(widget == m_rootWidget || !m_rootWidget)
        return;

    m_destroyedWidgets.push_back(widget);

    if(m_checkEvent && !m_checkEvent->isExecuted())
        return;

    m_checkEvent = g_dispatcher.scheduleEvent([this] {
        g_lua.collectGarbage();
        UIWidgetList backupList = m_destroyedWidgets;
        m_destroyedWidgets.clear();
        g_dispatcher.scheduleEvent([backupList] {
            g_lua.collectGarbage();
            for(const UIWidgetPtr& widget : backupList) {
                if(widget->ref_count() != 1)
                    g_logger.warning(stdext::format("widget '%s' destroyed but still have %d reference(s) left", widget->getId(), widget->getUseCount()-1));
            }
        }, 1);
    }, 1000);
#endif
}

void UIManager::clearStyles()
{
    m_styles.clear();
}

bool UIManager::importStyle(std::string file)
{
    try {
        file = g_resources.guessFilePath(file, "otui");

        OTMLDocumentPtr doc = OTMLDocument::parse(file);

        for(const OTMLNodePtr& styleNode : doc->children())
            importStyleFromOTML(styleNode);
        return true;
    } catch(stdext::exception& e) {
        g_logger.error(stdext::format("Failed to import UI styles from '%s': %s", file, e.what()));
        return false;
    }
}

void UIManager::importStyleFromOTML(const OTMLNodePtr& styleNode)
{
    std::string tag = styleNode->tag();
    std::vector<std::string> split = stdext::split(tag, "<");
    if(split.size() != 2)
        throw OTMLException(styleNode, "not a valid style declaration");

    std::string name = split[0];
    std::string base = split[1];
    bool unique = false;

    stdext::trim(name);
    stdext::trim(base);

    if(name[0] == '#') {
        name = name.substr(1);
        unique = true;

        styleNode->setTag(name);
        styleNode->writeAt("__unique", true);
    }

    OTMLNodePtr oldStyle = m_styles[name];

    // Warn about redefined styles
    /*
    if(!g_app.isRunning() && (oldStyle && !oldStyle->valueAt("__unique", false))) {
        auto it = m_styles.find(name);
        if(it != m_styles.end())
            g_logger.warning(stdext::format("style '%s' is being redefined", name));
    }
    */

    if(!oldStyle || !oldStyle->valueAt("__unique", false) || unique) {
        OTMLNodePtr originalStyle = getStyle(base);
        if(!originalStyle)
            stdext::throw_exception(stdext::format("base style '%s', is not defined", base));
        OTMLNodePtr style = originalStyle->clone();
        style->merge(styleNode);
        style->setTag(name);
        m_styles[name] = style;
    }
}

OTMLNodePtr UIManager::getStyle(const std::string& styleName)
{
    auto it = m_styles.find(styleName);
    if(it != m_styles.end())
        return m_styles[styleName];

    // styles starting with UI are automatically defined
    if(stdext::starts_with(styleName, "UI")) {
        OTMLNodePtr node = OTMLNode::create(styleName);
        node->writeAt("__class", styleName);
        m_styles[styleName] = node;
        return node;
    }

    return nullptr;
}

std::string UIManager::getStyleClass(const std::string& styleName)
{
    OTMLNodePtr style = getStyle(styleName);
    if(style && style->get("__class"))
        return style->valueAt("__class");
    return "";
}

UIWidgetPtr UIManager::loadUI(std::string file, const UIWidgetPtr& parent)
{
    try {
        file = g_resources.guessFilePath(file, "otui");

        OTMLDocumentPtr doc = OTMLDocument::parse(file);
        UIWidgetPtr widget;
        for(const OTMLNodePtr& node : doc->children()) {
            std::string tag = node->tag();

            // import styles in these files too
            if(tag.find("<") != std::string::npos)
                importStyleFromOTML(node);
            else {
                if(widget)
                    stdext::throw_exception("cannot have multiple main widgets in otui files");
                widget = createWidgetFromOTML(node, parent);
            }
        }

        return widget;
    } catch(stdext::exception& e) {
        g_logger.error(stdext::format("failed to load UI from '%s': %s", file, e.what()));
        return nullptr;
    }
}

UIWidgetPtr UIManager::createWidget(const std::string& styleName, const UIWidgetPtr& parent)
{
    OTMLNodePtr node = OTMLNode::create(styleName);
    try {
        return createWidgetFromOTML(node, parent);
    } catch(stdext::exception& e) {
        g_logger.error(stdext::format("failed to create widget from style '%s': %s", styleName, e.what()));
        return nullptr;
    }
}

UIWidgetPtr UIManager::createWidgetFromOTML(const OTMLNodePtr& widgetNode, const UIWidgetPtr& parent)
{
    OTMLNodePtr originalStyleNode = getStyle(widgetNode->tag());
    if(!originalStyleNode)
        stdext::throw_exception(stdext::format("'%s' is not a defined style", widgetNode->tag()));

    OTMLNodePtr styleNode = originalStyleNode->clone();
    styleNode->merge(widgetNode);

    std::string widgetType = styleNode->valueAt("__class");

    // call widget creation from lua
    UIWidgetPtr widget = g_lua.callGlobalField<UIWidgetPtr>(widgetType, "create");
    if(parent)
        parent->addChild(widget);

    if(widget) {
        widget->callLuaField("onCreate");

        widget->setStyleFromNode(styleNode);

        for(const OTMLNodePtr& childNode : styleNode->children()) {
            if(!childNode->isUnique()) {
                createWidgetFromOTML(childNode, widget);
                styleNode->removeChild(childNode);
            }
        }
    } else
        stdext::throw_exception(stdext::format("unable to create widget of type '%s'", widgetType));

    widget->callLuaField("onSetup");
    return widget;
}
