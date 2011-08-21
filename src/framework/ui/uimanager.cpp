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
        m_rootWidget->resize(size);
}

void UIManager::inputEvent(const PlatformEvent& event)
{
    // translate input event to ui events
    if(m_rootWidget) {
        if(event.type & EventKeyboardAction) {
            int keyboardModifiers = KeyboardNoModifier;
            if(event.ctrl)
                keyboardModifiers |= KeyboardCtrlModifier;
            if(event.shift)
                keyboardModifiers |= KeyboardShiftModifier;
            if(event.alt)
                keyboardModifiers |= KeyboardAltModifier;

            UIKeyEvent e(event.keycode, event.keychar, keyboardModifiers);
            if(event.type == EventKeyDown)
                m_rootWidget->onKeyPress(e);
            else
                m_rootWidget->onKeyRelease(e);
        } else if(event.type & EventMouseAction) {
            if(event.type == EventMouseMove) {
                UIMouseEvent e(event.mousePos, event.mousePos);
                m_rootWidget->onMouseMove(e);
            }
            else if(event.type & EventMouseWheel) {
                MouseWheelDirection dir;
                if(event.type & EventDown)
                    dir = MouseWheelDown;
                else if(event.type & EventUp)
                    dir = MouseWheelUp;

                UIMouseEvent e(event.mousePos, dir);
                m_rootWidget->onMouseWheel(e);
            } else  {
                MouseButton button;
                if(event.type & EventMouseLeftButton)
                    button = MouseLeftButton;
                else if(event.type & EventMouseMidButton)
                    button = MouseMidButton;
                else if(event.type & EventMouseRightButton)
                    button = MouseRightButton;

                UIMouseEvent e(event.mousePos, button);
                if(event.type & EventDown)
                    m_rootWidget->onMousePress(e);
                else if(event.type & EventUp)
                    m_rootWidget->onMouseRelease(e);
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

    UIWidgetPtr widget;
    if(widgetType == "UIWidget")
        widget = UIWidgetPtr(new UIWidget);
    else if(widgetType == "UILabel")
        widget = UIWidgetPtr(new UILabel);
    else if(widgetType == "UIButton")
        widget = UIWidgetPtr(new UIButton);
    else if(widgetType == "UILineEdit")
        widget = UIWidgetPtr(new UILineEdit);
    else if(widgetType == "UIWindow")
        widget = UIWidgetPtr(new UIWindow);
    else
        throw OTMLException(styleNode, "cannot determine widget type");

    widget->loadStyleFromOTML(styleNode);
    widget->updateLayout();

    for(const OTMLNodePtr& childNode : widgetNode->children()) {
        if(!childNode->isUnique())
            widget->addChild(loadWidgetFromOTML(childNode));
    }

    return widget;
}

