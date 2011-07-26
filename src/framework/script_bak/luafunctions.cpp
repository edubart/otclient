#include <global.h>
#include <script/luafunctions.h>
#include <script/luacontext.h>
#include <core/engine.h>
#include <core/resources.h>
#include <ui/ui.h>
#include <core/dispatcher.h>
#include "../../protocollogin.h"

#include <boost/algorithm/string.hpp>

int lua_App_exit() {
    g_engine.stop();
    return 0;
}

int lua_App_loadUI() {
    UIContainerPtr parent;
    if(g_lua.stackSize() > 1)
        parent = std::dynamic_pointer_cast<UIContainer>(g_lua.popLuaObjectInstance());
    else
        parent = UIContainer::getRoot();

    std::string uiFile = g_lua.popString();

    if(!boost::ends_with(uiFile, ".otml"))
        uiFile.append(".otml");

    UIElementPtr element;
    if(parent) {
        element = g_uiLoader.loadFromFile(uiFile.c_str(), parent);
        if(!element) {
            g_lua.reportFuncErrorWithTraceback("failed to load UI");
        }
    } else
        g_lua.reportFuncErrorWithTraceback("invalid parent container");

    g_lua.pushLuaObjectInstance(element);
    return 1;
}

int lua_App_getRootContainer() {
    UIContainerPtr rootContainer = UIContainer::getRoot();
    g_lua.pushLuaObjectInstance(rootContainer);
    return 1;
}

UIElementPtr lua_createUIElement(UI::ElementType elementType)
{
    std::string id;
    UIContainerPtr parent;

    if(g_lua.stackSize() > 1)
        parent = std::dynamic_pointer_cast<UIContainer>(g_lua.popLuaObjectInstance());

    if(g_lua.stackSize() > 0) {
        if(g_lua.isString())
            id = g_lua.popString();
        else
            parent = std::dynamic_pointer_cast<UIContainer>(g_lua.popLuaObjectInstance());
    }

    UIElementPtr element;
    switch(elementType) {
        case UI::Element:
            element = UIElementPtr(new UIElement);
            break;
        case UI::Container:
            element = UIElementPtr(new UIContainer);
            break;
        case UI::Label:
            element = UIElementPtr(new UILabel);
            break;
        case UI::Button:
            element = UIElementPtr(new UIButton);
            break;
        case UI::TextEdit:
            element = UIElementPtr(new UITextEdit);
            break;
        case UI::Window:
            element = UIElementPtr(new UIWindow);
            break;
        default:
            break;
    }

    if(element) {
        if(!id.empty())
            element->setId(id);

        if(parent)
            parent->addChild(element);

        element->applyDefaultSkin();
    }

    return element;
}

int lua_UIElement_setLocked() {
    bool locked = true;
    if(g_lua.stackSize() > 1)
        locked = g_lua.popBoolean();

    if(UIElementPtr element = std::dynamic_pointer_cast<UIElement>(g_lua.popLuaObjectInstance())) {
        if(UIContainerPtr container = element->getParent()) {
            if(locked)
                container->lockElement(element);
            else
                container->unlockElement(element);
        } else
            g_lua.reportFuncErrorWithTraceback("locking failed, element has no parent");
    }

    return 0;
}

int lua_UIElement_setMargin() {
    int top = 0, right = 0, bottom = 0, left = 0;
    if(g_lua.stackSize() == 5) {
        left = g_lua.popInteger();
        bottom = g_lua.popInteger();
        right = g_lua.popInteger();
        top = g_lua.popInteger();
    } else if(g_lua.stackSize() == 3) {
        left = right = g_lua.popInteger();
        top = bottom = g_lua.popInteger();
    } else if(g_lua.stackSize() == 2) {
        top = right = bottom = left = g_lua.popInteger();
    } else {
        g_lua.reportFuncErrorWithTraceback("invalid number of arguments");
    }

    if(UIElementPtr element = std::dynamic_pointer_cast<UIElement>(g_lua.popLuaObjectInstance()))
        element->setMargin(top, right, bottom, left);
    return 0;
}


int lua_UIElement_centerIn() {
    std::string targetId;
    if(g_lua.isString())
        targetId = g_lua.popString();
    else if(UIElementPtr target = std::dynamic_pointer_cast<UIElement>(g_lua.popLuaObjectInstance()))
        targetId = target->getId();

    if(UIElementPtr element = std::dynamic_pointer_cast<UIElement>(g_lua.popLuaObjectInstance()))
        element->centerIn(targetId);
    return 0;
}

int lua_UIElement_addAnchor() {
    AnchorPoint targetEdge = (AnchorPoint)g_lua.popInteger();

    std::string targetId;
    if(g_lua.isString())
        targetId = g_lua.popString();
    else if(UIElementPtr target = std::dynamic_pointer_cast<UIElement>(g_lua.popLuaObjectInstance()))
        targetId = target->getId();

    AnchorPoint anchoredEdge = (AnchorPoint)g_lua.popInteger();

    if(UIElementPtr element = std::dynamic_pointer_cast<UIElement>(g_lua.popLuaObjectInstance()))
        element->addAnchor(anchoredEdge, AnchorLine(targetId, targetEdge));
    return 0;
}


void registerScriptFunctions()
{
    ////////////////////////////////////////////////////////////////////////////////
    // App
    g_lua.registerModule("App");
    // App.exit()
    g_lua.registerMemberFunction("exit", &lua_App_exit);
    // App.loadUI(file, [parent])
    g_lua.registerMemberFunction("loadUI", &lua_App_loadUI);
    // App.getRootContainer()
    g_lua.registerMemberFunction("getRootContainer", &lua_App_getRootContainer);


    ////////////////////////////////////////////////////////////////////////////////
    // UIElement
    g_lua.registerClass("UIElement");
    // (UIElement) UIElement.new([UIContainer parent], [string id])
    g_lua.registerMemberFunction("new", &lua_UIElement_new);
    // (string) UIElement.id
    g_lua.registerMemberField("id", &lua_UIElement_getId, &lua_UIElement_setId);
    // (boolean) UIElement.enabled
    g_lua.registerMemberField("enabled", &lua_UIElement_isEnabled, &lua_UIElement_setEnabled);
    // (boolean UIElement.visible
    g_lua.registerMemberField("visible", &lua_UIElement_isVisible, &lua_UIElement_setVisible);
    // (boolean UIElement.focused
    g_lua.registerMemberField("focused", &lua_UIElement_isFocused, &lua_UIElement_setFocused);
    // (integer) UIElement.width
    g_lua.registerMemberField("width", &lua_UIElement_getWidth, &lua_UIElement_setWidth);
    // (integer) UIElement.height
    g_lua.registerMemberField("height", &lua_UIElement_getHeight, &lua_UIElement_setHeight);
    // (UIContainer UIElement.parent
    g_lua.registerMemberField("parent", &lua_UIElement_getParent, &lua_UIElement_setParent);
    // element:setLocked(boolean locked)
    g_lua.registerMemberFunction("setLocked", &lua_UIElement_setLocked);
    // element:setMargin(int all)
    // element:setMargin(int vertical, int horizontal)
    // element:setMargin(int top, right, bottom, left)
    g_lua.registerMemberFunction("setMargin", &lua_UIElement_setMargin);
    // element:destroy()
    g_lua.registerMemberFunction("destroy", &lua_UIElement_destroy);
    // element:centerIn(UIContainer target)
    g_lua.registerMemberFunction("centerIn", &lua_UIElement_centerIn);
    // element:addAnchor(AnchorPoint anchoredEdge, UIElement target, AnchorPoint targetEdge)
    // element:addAnchor(AnchorPoint anchoredEdge, string targetId, AnchorPoint targetEdge)
    g_lua.registerMemberFunction("addAnchor", &lua_UIElement_addAnchor);


    ////////////////////////////////////////////////////////////////////////////////
    // UIContainer
    g_lua.registerClass("UIContainer", "UIElement");
    // UIContainer.new([UIContainer parent], [string id])
    g_lua.registerMemberFunction("new", &lua_UIContainer_new);
    // container:child(string id)
    g_lua.registerMemberFunction("child", &lua_UIContainer_child);
    // (table) container:children()
    g_lua.registerMemberFunction("children", &lua_UIContainer_getChildren);
    // container:addChild(UIElement child)
    g_lua.registerMemberFunction("addChild", &lua_UIContainer_addChild);


    ////////////////////////////////////////////////////////////////////////////////
    // UILabel
    g_lua.registerClass("UILabel", "UIElement");
    // UILabel.new([UIContainer parent], [string id])
    g_lua.registerMemberFunction("new", &lua_UILabel_new);
    // label.text
    g_lua.registerMemberField("text", &lua_UILabel_getText, &lua_UILabel_setText);


    ////////////////////////////////////////////////////////////////////////////////
    // UIButton
    g_lua.registerClass("UIButton", "UIElement");
    // UIButton.new([UIContainer parent], [string id])
    g_lua.registerMemberFunction("new", &lua_UIButton_new);
    // button.text
    g_lua.registerMemberField("text", &lua_UIButton_getText, &lua_UIButton_setText);


    ////////////////////////////////////////////////////////////////////////////////
    // UITextEdit
    g_lua.registerClass("UITextEdit", "UIElement");
    // UITextEdit.new([UIContainer parent], [string id])
    g_lua.registerMemberFunction("new", &lua_UITextEdit_new);
    // textEdit.text
    g_lua.registerMemberField("text", &lua_UITextEdit_getText, &lua_UITextEdit_setText);


    ////////////////////////////////////////////////////////////////////////////////
    // UIWindow
    g_lua.registerClass("UIWindow", "UIContainer");
    // UIWindow.new([UIContainer parent], [string id])
    g_lua.registerMemberFunction("new", &lua_UIWindow_new);
    // window.title
    g_lua.registerMemberField("title", &lua_UIWindow_getTitle, &lua_UIWindow_setTitle);


    ////////////////////////////////////////////////////////////////////////////////
    // Protocol
    g_lua.registerClass("ProtocolLogin");
    g_lua.registerMemberFunction("new", []{
        g_lua.pushLuaObjectInstance(LuaObjectPtr(new ProtocolLogin));
        return 1;
    });
    g_lua.registerMemberFunction("login", []{
        std::string accountPassword = g_lua.popString();
        std::string accountName = g_lua.popString();
        if(ProtocolLoginPtr protocolLogin = std::dynamic_pointer_cast<ProtocolLogin>(g_lua.popLuaObjectInstance()))
            protocolLogin->login(accountName, accountPassword);
        return 0;
    });
    g_lua.registerMemberFunction("cancel", []{
        //TODO: this func
        return 0;
    });
}


