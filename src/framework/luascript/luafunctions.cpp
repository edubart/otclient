#include "luainterface.h"
#include <framework/graphics/fontmanager.h>
#include <framework/ui/ui.h>
#include <framework/net/protocol.h>
#include <framework/core/eventdispatcher.h>
#include <framework/core/configs.h>

void LuaInterface::registerFunctions()
{
    // UIWidget
    g_lua.registerClass<UIWidget>();
    g_lua.bindClassStaticFunction<UIWidget>("create", &UIWidget::create);
    g_lua.bindClassMemberFunction<UIWidget>("getId", &UIWidget::getId);
    g_lua.bindClassMemberFunction<UIWidget>("setId", &UIWidget::setId);
    g_lua.bindClassMemberFunction<UIWidget>("isEnabled", &UIWidget::isEnabled);
    g_lua.bindClassMemberFunction<UIWidget>("setEnabled", &UIWidget::setEnabled);
    g_lua.bindClassMemberFunction<UIWidget>("isExplicitlyVisible", &UIWidget::isExplicitlyVisible);
    g_lua.bindClassMemberFunction<UIWidget>("setVisible", &UIWidget::setVisible);
    g_lua.bindClassMemberFunction<UIWidget>("getWidth", &UIWidget::getWidth);
    g_lua.bindClassMemberFunction<UIWidget>("setWidth", &UIWidget::setWidth);
    g_lua.bindClassMemberFunction<UIWidget>("getHeight", &UIWidget::getHeight);
    g_lua.bindClassMemberFunction<UIWidget>("setHeight", &UIWidget::setHeight);
    g_lua.bindClassMemberFunction<UIWidget>("getParent", &UIWidget::getParent);
    g_lua.bindClassMemberFunction<UIWidget>("setParent", &UIWidget::setParent);
    g_lua.bindClassMemberFunction<UIWidget>("getBackgroundColor", &UIWidget::getBackgroundColor);
    g_lua.bindClassMemberFunction<UIWidget>("setBackgroundColor", &UIWidget::setBackgroundColor);
    g_lua.bindClassMemberFunction<UIWidget>("getForegroundColor", &UIWidget::getForegroundColor);
    g_lua.bindClassMemberFunction<UIWidget>("setForegroundColor", &UIWidget::setForegroundColor);
    g_lua.bindClassMemberFunction<UIWidget>("getOpacity", &UIWidget::getOpacity);
    g_lua.bindClassMemberFunction<UIWidget>("setOpacity", &UIWidget::setOpacity);
    g_lua.bindClassMemberFunction<UIWidget>("setStyle", &UIWidget::applyStyle);
    g_lua.bindClassMemberFunction<UIWidget>("getMarginTop", &UIWidget::getMarginTop);
    g_lua.bindClassMemberFunction<UIWidget>("setMarginTop", &UIWidget::setMarginTop);
    g_lua.bindClassMemberFunction<UIWidget>("getMarginBottom", &UIWidget::getMarginBottom);
    g_lua.bindClassMemberFunction<UIWidget>("setMarginBottom", &UIWidget::setMarginBottom);
    g_lua.bindClassMemberFunction<UIWidget>("getMarginLeft", &UIWidget::getMarginLeft);
    g_lua.bindClassMemberFunction<UIWidget>("setMarginLeft", &UIWidget::setMarginLeft);
    g_lua.bindClassMemberFunction<UIWidget>("getMarginRight", &UIWidget::getMarginRight);
    g_lua.bindClassMemberFunction<UIWidget>("setMarginRight", &UIWidget::setMarginRight);
    g_lua.bindClassMemberFunction<UIWidget>("hide", &UIWidget::hide);
    g_lua.bindClassMemberFunction<UIWidget>("show", &UIWidget::show);
    g_lua.bindClassMemberFunction<UIWidget>("fill", &UIWidget::fill);
    g_lua.bindClassMemberFunction<UIWidget>("centerIn", &UIWidget::centerIn);
    g_lua.bindClassMemberFunction<UIWidget>("addAnchor", &UIWidget::addAnchor);
    g_lua.bindClassMemberFunction<UIWidget>("getChildById", &UIWidget::getChildById);
    g_lua.bindClassMemberFunction<UIWidget>("getChildByIndex", &UIWidget::getChildByIndex);
    g_lua.bindClassMemberFunction<UIWidget>("getChildCount", &UIWidget::getChildCount);
    g_lua.bindClassMemberFunction<UIWidget>("insertChild", &UIWidget::insertChild);
    g_lua.bindClassMemberFunction<UIWidget>("removeChild", &UIWidget::removeChild);
    g_lua.bindClassMemberFunction<UIWidget>("addChild", &UIWidget::addChild);
    g_lua.bindClassMemberFunction<UIWidget>("lockChild", &UIWidget::lockChild);
    g_lua.bindClassMemberFunction<UIWidget>("destroy", &UIWidget::destroy);


    // UILabel
    g_lua.registerClass<UILabel, UIWidget>();
    g_lua.bindClassStaticFunction<UILabel>("create", &UILabel::create);
    g_lua.bindClassMemberFunction<UILabel>("getText", &UILabel::getText);
    g_lua.bindClassMemberFunction<UILabel>("setText", &UILabel::setText);
    g_lua.bindClassMemberFunction("resizeToText", &UILabel::resizeToText);

    // UIButton
    g_lua.registerClass<UIButton, UIWidget>();
    g_lua.bindClassStaticFunction<UIButton>("create", &UIButton::create);
    g_lua.bindClassMemberFunction<UIButton>("getText", &UIButton::getText);
    g_lua.bindClassMemberFunction<UIButton>("setText", &UIButton::setText);

    // UILineEdit
    g_lua.registerClass<UILineEdit, UIWidget>();
    g_lua.bindClassStaticFunction<UILineEdit>("create", &UILineEdit::create);
    g_lua.bindClassMemberFunction<UILineEdit>("getText", &UILineEdit::getText);
    g_lua.bindClassMemberFunction<UILineEdit>("setText", &UILineEdit::setText);
    g_lua.bindClassMemberFunction<UILineEdit>("clearText", &UILineEdit::clearText);

    // UIWindow
    g_lua.registerClass<UIWindow, UIWidget>();
    g_lua.bindClassStaticFunction<UIWindow>("create", &UIWindow::create);
    g_lua.bindClassMemberFunction<UIWindow>("getTitle", &UIWindow::getTitle);
    g_lua.bindClassMemberFunction<UIWindow>("setTitle", &UIWindow::setTitle);

    // Protocol
    g_lua.registerClass<Protocol>();

    // ConfigManager
    g_lua.registerClass<Configs>();
    g_lua.bindClassStaticFunction<Configs>("set", std::bind(&Configs::set, &g_configs, _1, _2));
    g_lua.bindClassStaticFunction<Configs>("get", std::bind(&Configs::get, &g_configs, _1));

    // Logger
    g_lua.registerClass<Logger>();
    g_lua.bindClassStaticFunction<Logger>("log", std::bind(&Logger::log, &g_logger, _1, _2));
    g_lua.bindClassStaticFunction<Logger>("fireOldMessages", std::bind(&Logger::fireOldMessages, &g_logger));
    g_lua.bindClassStaticFunction<Logger>("setOnLog", std::bind(&Logger::setOnLog, &g_logger, _1));

    // global functions
    g_lua.bindGlobalFunction("importFont", std::bind(&FontManager::importFont, &g_fonts, _1));
    g_lua.bindGlobalFunction("importStyles", std::bind(&UIManager::importStyles, &g_ui, _1));
    g_lua.bindGlobalFunction("setDefaultFont", std::bind(&FontManager::setDefaultFont, &g_fonts, _1));
    g_lua.bindGlobalFunction("loadUI", std::bind(&UIManager::loadUI, &g_ui, _1));
    g_lua.bindGlobalFunction("getRootWidget", std::bind(&UIManager::getRootWidget, &g_ui));
    g_lua.bindGlobalFunction("addEvent", std::bind(&EventDispatcher::addEvent, &g_dispatcher, _1, false));
    g_lua.bindGlobalFunction("scheduleEvent", std::bind(&EventDispatcher::scheduleEvent, &g_dispatcher, _1, _2));
}
