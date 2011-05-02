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


#include <prerequisites.h>
#include <script/luafunctions.h>
#include <script/luascript.h>
#include <core/engine.h>
#include <core/resources.h>
#include <ui/ui.h>
#include <core/dispatcher.h>

void registerLuaFunctions()
{
    // App
    g_lua.registerModule("App");
    g_lua.registerMemberFunction("App", "exit", &lua_App_exit);
    g_lua.registerMemberFunction("App", "setOnClose", &lua_App_setOnClose);

    // UI
    g_lua.registerModule("UI");
    g_lua.registerMemberFunction("UI", "load", &lua_UI_load);
    g_lua.registerMemberFunction("UI", "getRootContainer", &lua_UI_getRootContainer);

    // UILayout
    g_lua.registerClass("UILayout");

    // UIElement
    g_lua.registerClass("UIElement", "UILayout");
    g_lua.registerMemberField("UIElement", "onLoad", NULL, &lua_UIElement_setOnLoad);
    g_lua.registerMemberField("UIElement", "onDestroy", NULL,  &lua_UIElement_setOnDestroy);
    g_lua.registerMemberField("UIElement", "parent", &lua_UIElement_getParent);
    g_lua.registerMemberFunction("UIElement", "destroy", &lua_UIElement_destroy);

    // UIContainer
    g_lua.registerClass("UIContainer", "UIElement");
    g_lua.registerMemberFunction("UIContainer", "getChildById", &lua_UIContainer_getChildById);
    g_lua.registerMemberField("UIContainer", "locked", NULL, &lua_UIContainer_setLocked);

    // UILabel
    g_lua.registerClass("UILabel", "UIElement");
    g_lua.registerMemberField("UILabel", "text", &lua_UILabel_getText, &lua_UILabel_setText);

    // UIButton
    g_lua.registerClass("UIButton", "UIElement");
    g_lua.registerMemberField("UIButton", "onClick", NULL, &lua_UIButton_setOnClick);

    // UIWindow
    g_lua.registerClass("UIWindow", "UIContainer");
    g_lua.registerMemberField("UIWindow", "title", &lua_UIWindow_getTitle, &lua_UIWindow_setTitle);
}


////////////////////////////////////////////////////////////////////////////////
// App module

int lua_App_exit()
{
    g_engine.stop();
    return 1;
}

int lua_App_setOnClose()
{
    g_engine.setOnClose(g_lua.createSimpleFuncCallback(g_lua.popFunction()));
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
        element = UILoader::loadFile(uiFile.c_str(), parent);
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

int lua_UIElement_setOnLoad()
{
    g_lua.insert(-2);
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance())) {
        int funcRef = g_lua.popFunction();
        element->associateLuaRef(__FUNCTION__, funcRef);
        element->setOnLoad(g_lua.createScriptableSelfFuncCallback(funcRef));
    } else
        g_lua.pop();
    return 1;
}

int lua_UIElement_setOnDestroy()
{
    g_lua.insert(-2);
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance())) {
        int funcRef = g_lua.popFunction();
        element->associateLuaRef(__FUNCTION__, funcRef);
        element->setOnDestroy(g_lua.createScriptableSelfFuncCallback(funcRef));
    }
    else
        g_lua.pop();
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

int lua_UIElement_destroy()
{
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        element->destroy();
    g_dispatcher.addTask(boost::bind(&LuaScript::collectGarbage, &g_lua));
    return 1;
}


////////////////////////////////////////////////////////////////////////////////
// UIContainer

int lua_UIContainer_getChildById()
{
    std::string id = g_lua.popString();
    if(UIContainerPtr container = boost::dynamic_pointer_cast<UIContainer>(g_lua.popClassInstance()))
        g_lua.pushClassInstance(container->getChildById(id));
    else
        g_lua.pushNil();
    return 1;
}

int lua_UIContainer_setLocked()
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
// UIButton

int lua_UIButton_setOnClick()
{
    g_lua.insert(-2);
    if(UIButtonPtr button = boost::dynamic_pointer_cast<UIButton>(g_lua.popClassInstance())) {
        int funcRef = g_lua.popFunction();
        button->associateLuaRef(__FUNCTION__, funcRef);
        button->setOnClick(g_lua.createScriptableSelfFuncCallback(funcRef));
    } else
        g_lua.pop();
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
