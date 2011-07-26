#include "luainterface.h"
#include <core/engine.h>
#include <ui/ui.h>
#include "../../protocollogin.h"

void LuaInterface::registerFunctions()
{
    g_lua.bindGlobalFunction("exit", std::bind(&Engine::stop, &g_engine));
    g_lua.bindGlobalFunction("loadUI", std::bind(&UILoader::loadFromFile, &g_uiLoader, std::placeholders::_1, std::placeholders::_2));
    g_lua.setGlobal("rootUI", safe_to_luavalue(UIContainer::getRoot()));

    g_lua.registerClass("UIElement");
    g_lua.bindClassStaticFunction("UIElement", "new", &UIElement::create);
    g_lua.bindClassMemberField("UIElement", "id", &UIElement::getId, &UIElement::setId);
    g_lua.bindClassMemberField("UIElement", "enabled", &UIElement::isEnabled, &UIElement::setEnabled);
    g_lua.bindClassMemberField("UIElement", "visible", &UIElement::isVisible, &UIElement::setVisible);
    g_lua.bindClassMemberField("UIElement", "focused", &UIElement::isFocused, &UIElement::setFocused);
    g_lua.bindClassMemberField("UIElement", "width", &UIElement::getWidth, &UIElement::setWidth);
    g_lua.bindClassMemberField("UIElement", "height", &UIElement::getHeight, &UIElement::setHeight);
    g_lua.bindClassMemberField("UIElement", "parent", &UIElement::getParent, &UIElement::setParent);
    g_lua.bindClassMemberFunction("UIElement", "setLocked", &UIElement::setLocked);
    //g_lua.bindClassMemberFunction("UIElement", "setMargin", &UIElement::luaSetMargin);
    g_lua.bindClassMemberFunction("UIElement", "destroy", &UIElement::destroyLater);
    g_lua.bindClassMemberFunction("UIElement", "centerIn", &UIElement::centerIn);
    g_lua.bindClassMemberFunction("UIElement", "addAnchor", &UIElement::addAnchor);

    g_lua.registerClass("UIContainer", "UIElement");
    g_lua.bindClassStaticFunction("UIContainer", "create", &UIContainer::create);
    g_lua.bindClassMemberFunction("UIContainer", "getChild", &UIContainer::getChildById);
    //g_lua.bindClassMemberFunction("UIContainer", "children", &UIContainer::getChildren);
    g_lua.bindClassMemberFunction("UIContainer", "addChild", &UIContainer::addChild);

    g_lua.registerClass("UILabel", "UIElement");
    g_lua.bindClassStaticFunction("UILabel", "create", &UILabel::create);
    g_lua.bindClassMemberField("UILabel", "text", &UILabel::getText, &UILabel::setText);

    g_lua.registerClass("UIButton", "UIElement");
    g_lua.bindClassStaticFunction("UIButton", "create", &UIButton::create);
    g_lua.bindClassMemberField("UIButton", "text", &UIButton::getText, &UIButton::setText);

    g_lua.registerClass("UITextEdit", "UIElement");
    g_lua.bindClassStaticFunction("UITextEdit", "create", &UITextEdit::create);
    g_lua.bindClassMemberField("UITextEdit", "text", &UITextEdit::getText, &UITextEdit::setText);

    g_lua.registerClass("UIWindow", "UIContainer");
    g_lua.bindClassStaticFunction("UIWindow", "create", &UIWindow::create);
    g_lua.bindClassMemberField("UIWindow", "title", &UIWindow::getTitle, &UIWindow::setTitle);

    g_lua.registerClass("Protocol");
    g_lua.registerClass("ProtocolLogin", "Protocol");
    g_lua.bindClassStaticFunction("ProtocolLogin", "create", &ProtocolLogin::create);
    g_lua.bindClassMemberFunction("ProtocolLogin", "login", &ProtocolLogin::login);
    g_lua.bindClassMemberFunction("ProtocolLogin", "cancel", &ProtocolLogin::cancel);
}
