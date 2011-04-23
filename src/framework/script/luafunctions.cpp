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
#include <script/luascript.h>
#include <core/engine.h>
#include <core/resources.h>
#include <ui/ui.h>

void LuaScript::registerFunctions()
{
    registerGlobalFunction("exitGame", &LuaScript::lua_exitGame);
    registerGlobalFunction("loadUI", &LuaScript::lua_loadUI);
    registerGlobalFunction("getUIRootContainer", &LuaScript::lua_getUIRootContainer);
    registerGlobalFunction("setOnApplicationClose", &LuaScript::lua_setOnApplicationClose);

    registerClass("UILayout");
    registerClass("UIElement",   "UILayout");
    registerClass("UIButton",    "UIElement");
    registerClass("UITextEdit",  "UIElement");
    registerClass("UICheckBox",  "UIElement");
    registerClass("UIContainer", "UIElement");
    registerClass("UIWindow",    "UIContainer");

    registerMemberFunction("UIElement", "setOnLoad", &LuaScript::lua_UIElement_setOnLoad);
    registerMemberFunction("UIElement", "setOnDestroy", &LuaScript::lua_UIElement_setOnDestroy);
    registerMemberFunction("UIElement", "getParent", &LuaScript::lua_UIElement_getParent);
    registerMemberFunction("UIElement", "destroy", &LuaScript::lua_UIElement_destroy);
    registerMemberFunction("UIContainer", "getChildByID", &LuaScript::lua_UIContainer_getChildByID);
    registerMemberFunction("UIContainer", "lock", &LuaScript::lua_UIContainer_lock);
    registerMemberFunction("UIContainer", "unlock", &LuaScript::lua_UIContainer_unlock);
    registerMemberFunction("UIButton", "setOnClick", &LuaScript::lua_UIButton_setOnClick);
}

int LuaScript::lua_exitGame()
{
    g_engine.stop();
    return 1;
}

int LuaScript::lua_loadUI()
{
    UIContainerPtr parent;
    if(getStackSize() > 1) {
        parent = boost::dynamic_pointer_cast<UIContainer>(popClassInstance());
    } else {
        parent = UIContainer::getRootContainer();
    }

    std::string uiFile = popString();

    UIElementPtr element;
    if(parent)
        element = UILoader::loadFile(uiFile.c_str(), parent);
    else
        reportErrorWithTraceback("invalid parent container");

    pushClassInstance(element);
    return 1;
}

int LuaScript::lua_getUIRootContainer()
{
    UIContainerPtr rootContainer = UIContainer::getRootContainer();
    pushClassInstance(rootContainer);
    return 1;
}

int LuaScript::lua_setOnApplicationClose()
{
    int funcRef = popFunction();
    g_engine.setOnClose([this, funcRef] {
        pushFunction(funcRef);
        callFunction();
    });
    return 1;
}

int LuaScript::lua_UIElement_setOnLoad()
{
    moveTop(-2);
    UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(popClassInstance());
    if(element) {
        int funcRef = popFunction();
        element->setOnLoad([this, funcRef](UIElementPtr element) {
            pushFunction(funcRef);
            setLocal(element, "self");
            callFunction();
        });
    } else {
        pop();
    }
    return 1;
}

int LuaScript::lua_UIElement_setOnDestroy()
{
    moveTop(-2);
    UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(popClassInstance());
    if(element) {
        int funcRef = popFunction();
        element->setOnDestroy([this, funcRef](UIElementPtr element) {
            pushFunction(funcRef);
            setLocal(element, "self");
            callFunction();
        });
    } else {
        pop();
    }
    return 1;
}

int LuaScript::lua_UIElement_destroy()
{
    UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(popClassInstance());
    if(element)
        element->destroy();
    else
        pushNil();
    return 1;
}

int LuaScript::lua_UIElement_getParent()
{
    UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(popClassInstance());
    if(element)
        pushClassInstance(element->getParent());
    else
        pushNil();
    return 1;
}

int LuaScript::lua_UIButton_setOnClick()
{
    moveTop(-2);
    UIButtonPtr button = boost::dynamic_pointer_cast<UIButton>(popClassInstance());
    if(button) {
        int funcRef = popFunction();
        button->setOnClick([this, funcRef](UIElementPtr button) {
            pushFunction(funcRef);
            setLocal(button, "self");
            callFunction();
        });
    } else {
        pop();
    }
    return 1;
}

int LuaScript::lua_UIContainer_getChildByID()
{
    std::string id = popString();
    UIContainerPtr container = boost::dynamic_pointer_cast<UIContainer>(popClassInstance());
    if(container)
        pushClassInstance(container->getChildById(id));
    else
        pushNil();
    return 1;
}

int LuaScript::lua_UIContainer_lock()
{
    UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(popClassInstance());
    UIContainerPtr container = boost::dynamic_pointer_cast<UIContainer>(popClassInstance());
    if(!element) {
        reportFuncErrorWithTraceback("invalid lock element");
        return 1;
    }

    if(container) {
        container->lockElement(element);
    }
    return 1;
}

int LuaScript::lua_UIContainer_unlock()
{
    UIContainerPtr container = boost::dynamic_pointer_cast<UIContainer>(popClassInstance());
    if(container) {
        container->unlockElement();
    }
    return 1;
}
