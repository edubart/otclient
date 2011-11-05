/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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

UIManager g_ui;

void UIManager::init()
{
    // creates root widget
    m_rootWidget = UIWidget::create<UIWidget>();
    m_rootWidget->setup();
    m_rootWidget->setId("root");
    m_rootWidget->resize(g_graphics.getScreenSize());
}

void UIManager::terminate()
{
    // destroy root widget and its children'
    m_rootWidget->destroy();
    m_rootWidget.reset();
}

void UIManager::render()
{
    m_rootWidget->render();
}

void UIManager::resize(const Size& size)
{
    if(m_rootWidget)
        m_rootWidget->resize(g_graphics.getScreenSize());
}

void UIManager::inputEvent(const PlatformEvent& event)
{
    // translate input event to ui events
    if(m_rootWidget) {
        if(event.type & EventKeyboardAction) {
            int keyboardModifiers = Fw::KeyboardNoModifier;
            if(event.ctrl)
                keyboardModifiers |= Fw::KeyboardCtrlModifier;
            if(event.shift)
                keyboardModifiers |= Fw::KeyboardShiftModifier;
            if(event.alt)
                keyboardModifiers |= Fw::KeyboardAltModifier;

            if(event.type == EventKeyDown)
                m_rootWidget->onKeyPress(event.keycode, event.keychar, keyboardModifiers);
            else
                m_rootWidget->onKeyRelease(event.keycode, event.keychar, keyboardModifiers);
        } else if(event.type & EventMouseAction) {
            if(event.type == EventMouseMove) {
                m_rootWidget->updateState(Fw::HoverState);
                m_rootWidget->onMouseMove(event.mousePos, event.mouseMoved);
            }
            else if(event.type & EventMouseWheel) {
                Fw::MouseWheelDirection dir = Fw::MouseNoWheel;
                if(event.type & EventDown)
                    dir = Fw::MouseWheelDown;
                else if(event.type & EventUp)
                    dir = Fw::MouseWheelUp;

                m_rootWidget->onMouseWheel(event.mousePos, dir);
            } else  {
                Fw::MouseButton button = Fw::MouseNoButton;
                if(event.type & EventMouseLeftButton)
                    button = Fw::MouseLeftButton;
                else if(event.type & EventMouseMidButton)
                    button = Fw::MouseMidButton;
                else if(event.type & EventMouseRightButton)
                    button = Fw::MouseRightButton;

                if(event.type & EventDown)
                    m_rootWidget->onMousePress(event.mousePos, button);
                else if(event.type & EventUp)
                    m_rootWidget->onMouseRelease(event.mousePos, button);
            }
        }
    }
}

bool UIManager::importStyles(const std::string& file)
{
    try {
        OTMLDocumentPtr doc = OTMLDocument::parse(file);

        for(const OTMLNodePtr& styleNode : doc->children())
            importStyleFromOTML(styleNode);
        return true;
    } catch(std::exception& e) {
        logError("failed to import ui styles from '", file, "':\n", e.what());
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
    // disable this warning because many ppl was complening about it
    /*
    auto it = m_styles.find(name);
    if(it != m_styles.end())
        logWarning("style '", name, "' is being redefined");
    */

    OTMLNodePtr style = getStyle(base)->clone();
    style->merge(styleNode);
    style->setTag(name);
    m_styles[name] = style;
}

OTMLNodePtr UIManager::getStyle(const std::string& styleName)
{
    if(boost::starts_with(styleName, "UI")) {
        OTMLNodePtr node = OTMLNode::create();
        node->writeAt("__widgetType", styleName);
        return node;
    }

    auto it = m_styles.find(styleName);
    if(it == m_styles.end())
        throw std::runtime_error(Fw::mkstr("style '", styleName, "' is not a defined style"));
    return m_styles[styleName];
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
                    throw std::runtime_error("cannot have multiple main widgets in .otui files");
                widget = loadWidgetFromOTML(node, parent);
            }
        }

        return widget;
    } catch(std::exception& e) {
        logError("failed to load ui from '", file, "':\n", e.what());
        return nullptr;
    }
}

UIWidgetPtr UIManager::loadWidgetFromOTML(const OTMLNodePtr& widgetNode, const UIWidgetPtr& parent)
{
    OTMLNodePtr styleNode = getStyle(widgetNode->tag())->clone();
    styleNode->merge(widgetNode);

    std::string widgetType = styleNode->valueAt("__widgetType");

    // call widget creation from lua
    UIWidgetPtr widget = g_lua.callGlobalField<UIWidgetPtr>(widgetType, "create");
    if(parent)
        parent->addChild(widget);

    widget->setStyleFromNode(styleNode);

    for(const OTMLNodePtr& childNode : widgetNode->children()) {
        if(!childNode->isUnique())
            loadWidgetFromOTML(childNode, widget);
    }

    return widget;
}

