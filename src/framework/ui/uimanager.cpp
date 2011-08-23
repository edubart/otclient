#include "uimanager.h"
#include "ui.h"

#include <framework/otml/otml.h>
#include <framework/graphics/graphics.h>

UIManager g_ui;

void UIManager::init()
{
    // creates root widget
    m_rootWidget = UIWidgetPtr(new UIWidget);
    m_rootWidget->setId("root");
    m_rootWidget->setHovered(true);
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
            int keyboardModifiers = UI::KeyboardNoModifier;
            if(event.ctrl)
                keyboardModifiers |= UI::KeyboardCtrlModifier;
            if(event.shift)
                keyboardModifiers |= UI::KeyboardShiftModifier;
            if(event.alt)
                keyboardModifiers |= UI::KeyboardAltModifier;

            if(event.type == EventKeyDown)
                m_rootWidget->onKeyPress(event.keycode, event.keychar, keyboardModifiers);
            else
                m_rootWidget->onKeyRelease(event.keycode, event.keychar, keyboardModifiers);
        } else if(event.type & EventMouseAction) {
            if(event.type == EventMouseMove) {
                m_rootWidget->onMouseMove(event.mousePos, event.mouseMoved);
            }
            else if(event.type & EventMouseWheel) {
                UI::MouseWheelDirection dir = UI::MouseNoWheel;
                if(event.type & EventDown)
                    dir = UI::MouseWheelDown;
                else if(event.type & EventUp)
                    dir = UI::MouseWheelUp;

                m_rootWidget->onMouseWheel(event.mousePos, dir);
            } else  {
                UI::MouseButton button = UI::MouseNoButton;
                if(event.type & EventMouseLeftButton)
                    button = UI::MouseLeftButton;
                else if(event.type & EventMouseMidButton)
                    button = UI::MouseMidButton;
                else if(event.type & EventMouseRightButton)
                    button = UI::MouseRightButton;

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

    auto it = m_styles.find(name);
    if(it != m_styles.end())
        logWarning("style '", name, "' is being redefined");

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
        throw std::runtime_error(fw::mkstr("style '", styleName, "' is not a defined style"));
    return m_styles[styleName];
}

UIWidgetPtr UIManager::loadUI(const std::string& file)
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
                widget = loadWidgetFromOTML(node);
            }
        }

        return widget;
    } catch(std::exception& e) {
        logError("failed to load ui from '", file, "':\n", e.what());
        return nullptr;
    }
}

UIWidgetPtr UIManager::loadWidgetFromOTML(const OTMLNodePtr& widgetNode)
{
    OTMLNodePtr styleNode = getStyle(widgetNode->tag())->clone();
    styleNode->merge(widgetNode);

    std::string widgetType = styleNode->valueAt("__widgetType");

    // call widget creation from lua
    //g_lua.getGlobalField(widgetType, "create");
    g_lua.getGlobal(widgetType);
    g_lua.getField("create");
    g_lua.remove(-2);
    g_lua.protectedCall(0, 1);
    UIWidgetPtr widget = g_lua.polymorphicPop<UIWidgetPtr>();

    widget->onStyleApply(styleNode);
    widget->updateLayout();

    for(const OTMLNodePtr& childNode : widgetNode->children()) {
        if(!childNode->isUnique())
            widget->addChild(loadWidgetFromOTML(childNode));
    }

    return widget;
}

