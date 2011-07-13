/* The MIT License
 *
 * Copyright (c) 2010 OTClient, https://github.com/edubart/otclient
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


#include <global.h>
#include <script/luafunctions.h>
#include <script/luascript.h>
#include <core/engine.h>
#include <core/resources.h>
#include <ui/ui.h>
#include <core/dispatcher.h>
#include "../../protocollogin.h"

void registerLuaFunctions()
{
    // App
    g_lua.registerModule("App");
    g_lua.registerMemberFunction("exit", &lua_App_exit);

    // UI
    g_lua.registerClass("UI");
    g_lua.registerMemberFunction("load", &lua_UI_load);
    g_lua.registerMemberFunction("getRootContainer", &lua_UI_getRootContainer);

    // UIElement
    g_lua.registerClass("UIElement");
    g_lua.registerMemberField("id", &lua_UIElement_getId, &lua_UIElement_setId);
    g_lua.registerMemberField("enabled", &lua_UIElement_isEnabled, &lua_UIElement_setEnabled);
    g_lua.registerMemberField("visible", &lua_UIElement_isVisible, &lua_UIElement_setVisible);
    g_lua.registerMemberField("focused", &lua_UIElement_isFocused, &lua_UIElement_setFocused);
    g_lua.registerMemberField("parent", &lua_UIElement_getParent, &lua_UIElement_setParent);
    g_lua.registerMemberField("locked", NULL, &lua_UIElement_setLocked);
    g_lua.registerMemberFunction("destroy", &lua_UIElement_destroy);

    // UIContainer
    g_lua.registerClass("UIContainer", "UIElement");
    g_lua.registerMemberFunction("child", &lua_UIContainer_getChild);
    g_lua.registerMemberFunction("children", &lua_UIContainer_getChildren);

    // UILabel
    g_lua.registerClass("UILabel", "UIElement");
    g_lua.registerMemberField("text", &lua_UILabel_getText, &lua_UILabel_setText);

    // UIButton
    g_lua.registerClass("UIButton", "UIElement");

    // UITextEdit
    g_lua.registerClass("UITextEdit", "UIElement");
    g_lua.registerMemberField("text", &lua_UITextEdit_getText, &lua_UITextEdit_setText);

    // UIWindow
    g_lua.registerClass("UIWindow", "UIContainer");
    g_lua.registerMemberField("title", &lua_UIWindow_getTitle, &lua_UIWindow_setTitle);

    // Protocol
    g_lua.registerClass("ProtocolLogin");
    g_lua.registerMemberFunction("new", []{
        ProtocolLoginPtr protocolLogin(new ProtocolLogin);
        g_lua.pushClassInstance(protocolLogin);
        return 1;
    });
    g_lua.registerMemberFunction("login", []{
        std::string accountPassword = g_lua.popString();
        std::string accountName = g_lua.popString();
        if(ProtocolLoginPtr protocolLogin = boost::dynamic_pointer_cast<ProtocolLogin>(g_lua.popClassInstance()))
            protocolLogin->login(accountName, accountPassword);
        return 0;
    });

}


////////////////////////////////////////////////////////////////////////////////
// App module

int lua_App_exit()
{
    g_engine.stop();
    return 1;
}


////////////////////////////////////////////////////////////////////////////////
// UI module

int lua_UI_load()
{
    UIContainerPtr parent;
    if(g_lua.getStackSize() > 1) {
        parent = boost::dynamic_pointer_cast<UIContainer>(g_lua.popClassInstance());
    } else {
        parent = UIContainer::getRoot();
    }

    std::string uiFile = g_lua.popString();

    UIElementPtr element;
    if(parent)
        element = g_uiLoader.loadFromFile(uiFile.c_str(), parent);
    else
        g_lua.reportErrorWithTraceback("invalid parent container");

    g_lua.pushClassInstance(element);
    return 1;
}

int lua_UI_getRootContainer()
{
    UIContainerPtr rootContainer = UIContainer::getRoot();
    g_lua.pushClassInstance(rootContainer);
    return 1;
}


////////////////////////////////////////////////////////////////////////////////
// UIElement

int lua_UIElement_getId()
{
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        g_lua.pushString(element->getId());
    else
        g_lua.pushNil();
    return 1;
}

int lua_UIElement_setId()
{
    std::string id = g_lua.popString();
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        element->setId(id);
    return 1;
}

int lua_UIElement_isEnabled()
{
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        g_lua.pushBoolean(element->isEnabled());
    else
        g_lua.pushNil();
    return 1;
}

int lua_UIElement_setEnabled()
{
    bool enabled = g_lua.popBoolean();
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        element->setEnabled(enabled);
    return 1;
}

int lua_UIElement_isVisible()
{
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        g_lua.pushBoolean(element->isVisible());
    else
        g_lua.pushNil();
    return 1;
}

int lua_UIElement_setVisible()
{
    bool visible = g_lua.popBoolean();
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        element->setVisible(visible);
    return 1;
}

int lua_UIElement_isFocused()
{
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        g_lua.pushBoolean(element->isFocused());
    else
        g_lua.pushNil();
    return 1;
}

int lua_UIElement_setFocused()
{
    bool focused = g_lua.popBoolean();
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        element->setFocused(focused);
    return 1;
}

int lua_UIElement_getParent()
{
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        g_lua.pushClassInstance(element->getParent());
    else
        g_lua.pushNil();
    return 1;
}

int lua_UIElement_setParent()
{
    UIContainerPtr parent = boost::dynamic_pointer_cast<UIContainer>(g_lua.popClassInstance());
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        element->setParent(parent);
    return 1;
}

int lua_UIElement_setLocked()
{
    bool locked = g_lua.popBoolean();
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance())) {
        if(UIContainerPtr container = element->getParent()) {
            if(locked)
                container->lockElement(element);
            else
                container->unlockElement(element);
        }
    } else {
        g_lua.reportFuncErrorWithTraceback("invalid element");
    }
    return 1;
}

int lua_UIElement_destroy()
{
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        element->destroyLater();
    g_dispatcher.addTask(boost::bind(&LuaScript::collectGarbage, &g_lua));
    return 1;
}


////////////////////////////////////////////////////////////////////////////////
// UIContainer

int lua_UIContainer_getChild()
{
    std::string id = g_lua.popString();
    if(UIContainerPtr container = boost::dynamic_pointer_cast<UIContainer>(g_lua.popClassInstance()))
        g_lua.pushClassInstance(container->getChildById(id));
    else
        g_lua.pushNil();
    return 1;
}

int lua_UIContainer_getChildren()
{
    if(UIContainerPtr container = boost::dynamic_pointer_cast<UIContainer>(g_lua.popClassInstance())) {
        g_lua.newTable();
        foreach(const UIElementPtr& child, container->getChildren()) {
            g_lua.pushClassInstance(child);
            if(child->getId().length())
                g_lua.setField(child->getId());
        }
    } else
        g_lua.pushNil();
    return 1;
}

////////////////////////////////////////////////////////////////////////////////
// UILabel

int lua_UILabel_setText()
{
    std::string text = g_lua.popString();
    if(UILabelPtr label = boost::dynamic_pointer_cast<UILabel>(g_lua.popClassInstance()))
        label->setText(text);
    return 1;
}

int lua_UILabel_getText()
{
    if(UILabelPtr label = boost::dynamic_pointer_cast<UILabel>(g_lua.popClassInstance()))
        g_lua.pushString(label->getText());
    else
        g_lua.pushNil();
    return 1;
}


////////////////////////////////////////////////////////////////////////////////
// UILabel

int lua_UITextEdit_setText()
{
    std::string text = g_lua.popString();
    if(UITextEditPtr textEdit = boost::dynamic_pointer_cast<UITextEdit>(g_lua.popClassInstance()))
        textEdit->setText(text);
    return 1;
}

int lua_UITextEdit_getText()
{
    if(UITextEditPtr textEdit = boost::dynamic_pointer_cast<UITextEdit>(g_lua.popClassInstance()))
        g_lua.pushString(textEdit->getText());
    else
        g_lua.pushNil();
    return 1;
}

////////////////////////////////////////////////////////////////////////////////
// UIWindow

int lua_UIWindow_setTitle()
{
    std::string title = g_lua.popString();
    if(UIWindowPtr window = boost::dynamic_pointer_cast<UIWindow>(g_lua.popClassInstance()))
        window->setTitle(title);
    return 1;
}

int lua_UIWindow_getTitle()
{
    if(UIWindowPtr window = boost::dynamic_pointer_cast<UIWindow>(g_lua.popClassInstance()))
        g_lua.pushString(window->getTitle());
    else
        g_lua.pushNil();
    return 1;
}
