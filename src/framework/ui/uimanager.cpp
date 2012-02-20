/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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
}

void UIManager::render()
{
    m_rootWidget->draw();
}

void UIManager::resize(const Size& size)
{
    m_rootWidget->setSize(g_window.getSize());
}

void UIManager::inputEvent(const InputEvent& event)
{
    m_isOnInputEvent = true;
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
            m_mouseReceiver->propagateOnMousePress(event.mousePos, event.mouseButton);
            if(event.mouseButton == Fw::MouseLeftButton && m_mouseReceiver->isVisible()) {
                UIWidgetPtr pressedWidget = m_mouseReceiver->recursiveGetChildByPos(event.mousePos);
                if(pressedWidget && !pressedWidget->isEnabled())
                    pressedWidget = nullptr;
                updatePressedWidget(pressedWidget, event.mousePos);
            }
            break;
        case Fw::MouseReleaseInputEvent:
            m_mouseReceiver->propagateOnMouseRelease(event.mousePos, event.mouseButton);
            if(event.mouseButton == Fw::MouseLeftButton) {
                // release pressed widget
                if(m_pressedWidget)
                    updatePressedWidget(nullptr, event.mousePos);

                // release dragging widget
                if(m_draggingWidget)
                    updateDraggingWidget(nullptr, event.mousePos);
            }
            break;
        case Fw::MouseMoveInputEvent: {
            // start dragging when moves a pressed widget
            if(m_pressedWidget && m_pressedWidget->isDragable() && m_draggingWidget != m_pressedWidget)
                updateDraggingWidget(m_pressedWidget, event.mousePos - event.mouseMoved);

            if(m_draggingWidget)
                m_draggingWidget->onDragMove(event.mousePos, event.mouseMoved);

            updateHoveredWidget();
            m_mouseReceiver->propagateOnMouseMove(event.mousePos, event.mouseMoved);
            break;
        }
        case Fw::MouseWheelInputEvent:
            m_mouseReceiver->propagateOnMouseWheel(event.mousePos, event.wheelDirection);
            break;
        default:
            break;
    };
    m_isOnInputEvent = false;
}

void UIManager::updatePressedWidget(const UIWidgetPtr& newPressedWidget, const Point& clickedPos)
{
    UIWidgetPtr oldPressedWidget = m_pressedWidget;
    m_pressedWidget = newPressedWidget;

    if(newPressedWidget)
        newPressedWidget->updateState(Fw::PressedState);

    if(oldPressedWidget) {
        oldPressedWidget->updateState(Fw::PressedState);

        if(oldPressedWidget->isEnabled()) {
            // when releasing mouse inside pressed widget area send onClick event
            if(!clickedPos.isNull() && oldPressedWidget->containsPoint(clickedPos)) {
                // onMouseRelease will be already fired by mouseReceiver
                oldPressedWidget->onClick(clickedPos);
            // must send mouse events even if the mouse is outside widget area when releasing a pressed widget
            } else
                oldPressedWidget->onMouseRelease(clickedPos, Fw::MouseLeftButton);
        }
    }
}

void UIManager::updateHoveredWidget()
{
    if(m_hoverUpdateScheduled)
        return;

    g_dispatcher.addEvent([this] {
        if(!m_rootWidget)
            return;

        m_hoverUpdateScheduled = false;
        UIWidgetPtr hoveredWidget = m_rootWidget->recursiveGetChildByPos(g_window.getMousePosition());
        if(hoveredWidget && !hoveredWidget->isEnabled())
            hoveredWidget = nullptr;

        if(hoveredWidget != m_hoveredWidget) {
            UIWidgetPtr oldHovered = m_hoveredWidget;
            m_hoveredWidget = hoveredWidget;
            if(oldHovered)
                oldHovered->updateState(Fw::HoverState);
            if(hoveredWidget)
                hoveredWidget->updateState(Fw::HoverState);
        }
    });
    m_hoverUpdateScheduled = true;
}

void UIManager::updateDraggingWidget(const UIWidgetPtr& draggingWidget, const Point& clickedPos)
{
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

        oldDraggingWidget->onDragLeave(droppedWidget, clickedPos);
        oldDraggingWidget->updateState(Fw::DraggingState);
    }

    if(draggingWidget) {
        if(draggingWidget->onDragEnter(clickedPos)) {
            m_draggingWidget = draggingWidget;
            draggingWidget->updateState(Fw::DraggingState);
        }
    }
}

void UIManager::onWidgetAppear(const UIWidgetPtr& widget)
{
    updateHoveredWidget();
}

void UIManager::onWidgetDisappear(const UIWidgetPtr& widget)
{
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
}

bool UIManager::importStyle(const std::string& file)
{
    try {
        OTMLDocumentPtr doc = OTMLDocument::parse(file);

        for(const OTMLNodePtr& styleNode : doc->children())
            importStyleFromOTML(styleNode);
        return true;
    } catch(Exception& e) {
        logError("Failed to import UI styles from '", file, "': ", e.what());
        return false;
    }
}

void UIManager::importStyleFromOTML(const OTMLNodePtr& styleNode)
{
    std::string tag = styleNode->tag();
    std::vector<std::string> split;
    boost::split(split, tag, boost::is_any_of(std::string("<")));
    if(split.size() != 2)
        throw OTMLException(styleNode, "not a valid style declaration");

    std::string name = split[0];
    std::string base = split[1];

    boost::trim(name);
    boost::trim(base);

    // TODO: styles must be searched by widget scopes, in that way this warning could be fixed
    // this warning is disabled because many ppl was complening about it
    /*
    auto it = m_styles.find(name);
    if(it != m_styles.end())
        logWarning("style '", name, "' is being redefined");
    */

    OTMLNodePtr originalStyle = getStyle(base);
    if(!originalStyle)
        Fw::throwException("base style '", base, "' is not defined");
    OTMLNodePtr style = originalStyle->clone();
    style->merge(styleNode);
    style->setTag(name);
    m_styles[name] = style;
}

OTMLNodePtr UIManager::getStyle(const std::string& styleName)
{
    auto it = m_styles.find(styleName);
    if(it != m_styles.end())
        return m_styles[styleName];

    // styles starting with UI are automatically defined
    if(boost::starts_with(styleName, "UI")) {
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

UIWidgetPtr UIManager::loadUI(const std::string& file, const UIWidgetPtr& parent)
{
    try {
        OTMLDocumentPtr doc = OTMLDocument::parse(file);
        UIWidgetPtr widget;
        for(const OTMLNodePtr& node : doc->children()) {
            std::string tag = node->tag();

            // import styles in these files too
            if(tag.find("<") != std::string::npos)
                importStyleFromOTML(node);
            else {
                if(widget)
                    Fw::throwException("cannot have multiple main widgets in otui files");
                widget = loadWidgetFromOTML(node, parent);
            }
        }

        return widget;
    } catch(Exception& e) {
        logError("Failed to load UI from '", file, "': ", e.what());
        return nullptr;
    }
}

UIWidgetPtr UIManager::loadWidgetFromOTML(const OTMLNodePtr& widgetNode, const UIWidgetPtr& parent)
{
    OTMLNodePtr originalStyleNode = getStyle(widgetNode->tag());
    if(!originalStyleNode)
        Fw::throwException("'", widgetNode->tag(), "' is not a defined style");

    OTMLNodePtr styleNode = originalStyleNode->clone();
    styleNode->merge(widgetNode);

    std::string widgetType = styleNode->valueAt("__class");

    // call widget creation from lua
    UIWidgetPtr widget = g_lua.callGlobalField<UIWidgetPtr>(widgetType, "create");
    if(parent)
        parent->addChild(widget);

    if(widget) {
        widget->setStyleFromNode(styleNode);

        for(const OTMLNodePtr& childNode : widgetNode->children()) {
            if(!childNode->isUnique())
                loadWidgetFromOTML(childNode, widget);
        }
    } else
        Fw::throwException("unable to create widget of type '", widgetType, "'");

    return widget;
}

