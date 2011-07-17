#include <global.h>
#include <script/scriptfunctions.h>
#include <script/scriptcontext.h>
#include <core/engine.h>
#include <core/resources.h>
#include <ui/ui.h>
#include <core/dispatcher.h>
#include "../../protocollogin.h"

#include <boost/algorithm/string.hpp>


////////////////////////////////////////////////////////////////////////////////
// App

int lua_App_exit() {
    g_engine.stop();
    return 0;
}



////////////////////////////////////////////////////////////////////////////////
// UI

int lua_UI_load() {
    UIContainerPtr parent;
    if(g_lua.getStackSize() > 1)
        parent = boost::dynamic_pointer_cast<UIContainer>(g_lua.popClassInstance());
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

    g_lua.pushClassInstance(element);
    return 1;
}

int lua_UI_getRootContainer() {
    UIContainerPtr rootContainer = UIContainer::getRoot();
    g_lua.pushClassInstance(rootContainer);
    return 1;
}



////////////////////////////////////////////////////////////////////////////////
// UIElement

UIElementPtr lua_createUIElement(UI::ElementType elementType)
{
    std::string id;
    UIContainerPtr parent;

    if(g_lua.getStackSize() > 1)
        parent = boost::dynamic_pointer_cast<UIContainer>(g_lua.popClassInstance());

    if(g_lua.getStackSize() > 0) {
        if(g_lua.isString())
            id = g_lua.popString();
        else
            parent = boost::dynamic_pointer_cast<UIContainer>(g_lua.popClassInstance());
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

int lua_UIElement_new() {
    g_lua.pushClassInstance(lua_createUIElement(UI::Element));
    return 1;
}

int lua_UIElement_getId() {
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        g_lua.pushString(element->getId());
    else
        g_lua.pushNil();
    return 1;
}

int lua_UIElement_setId() {
    std::string id = g_lua.popString();
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        element->setId(id);
    return 0;
}

int lua_UIElement_isEnabled() {
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        g_lua.pushBoolean(element->isEnabled());
    else
        g_lua.pushNil();
    return 1;
}

int lua_UIElement_setEnabled() {
    bool enabled = g_lua.popBoolean();
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        element->setEnabled(enabled);
    return 0;
}

int lua_UIElement_isVisible() {
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        g_lua.pushBoolean(element->isVisible());
    else
        g_lua.pushNil();
    return 1;
}

int lua_UIElement_setVisible() {
    bool visible = g_lua.popBoolean();
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        element->setVisible(visible);
    return 0;
}

int lua_UIElement_isFocused() {
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        g_lua.pushBoolean(element->isFocused());
    else
        g_lua.pushNil();
    return 1;
}

int lua_UIElement_setFocused() {
    bool focused = g_lua.popBoolean();
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        element->setFocused(focused);
    return 0;
}

int lua_UIElement_getWidth() {
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        g_lua.pushInteger(element->getWidth());
    else
        g_lua.pushInteger(0);
    return 1;
}

int lua_UIElement_setWidth() {
    int width = g_lua.popInteger();
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        element->setWidth(width);
    return 0;
}

int lua_UIElement_getHeight() {
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        g_lua.pushInteger(element->getHeight());
    else
        g_lua.pushInteger(0);
    return 1;
}

int lua_UIElement_setHeight() {
    int height = g_lua.popInteger();
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        element->setHeight(height);
    return 0;
}

int lua_UIElement_getParent() {
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        g_lua.pushClassInstance(element->getParent());
    else
        g_lua.pushNil();
    return 1;
}

int lua_UIElement_setParent() {
    UIContainerPtr parent = boost::dynamic_pointer_cast<UIContainer>(g_lua.popClassInstance());
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        element->setParent(parent);
    return 0;
}

int lua_UIElement_setLocked() {
    bool locked = true;
    if(g_lua.getStackSize() > 1)
        locked = g_lua.popBoolean();

    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance())) {
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
    if(g_lua.getStackSize() == 5) {
        left = g_lua.popInteger();
        bottom = g_lua.popInteger();
        right = g_lua.popInteger();
        top = g_lua.popInteger();
    } else if(g_lua.getStackSize() == 3) {
        left = right = g_lua.popInteger();
        top = bottom = g_lua.popInteger();
    } else if(g_lua.getStackSize() == 2) {
        top = right = bottom = left = g_lua.popInteger();
    } else {
        g_lua.reportFuncErrorWithTraceback("invalid number of arguments");
    }

    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        element->setMargin(top, right, bottom, left);
    return 0;
}

int lua_UIElement_destroy() {
    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        element->destroyLater();
    g_dispatcher.addTask(boost::bind(&ScriptContext::collectGarbage, &g_lua));
    return 0;
}

int lua_UIElement_centerIn() {
    std::string targetId;
    if(g_lua.isString())
        targetId = g_lua.popString();
    else if(UIElementPtr target = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        targetId = target->getId();

    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        element->centerIn(targetId);
    return 0;
}

int lua_UIElement_addAnchor() {
    AnchorPoint targetEdge = (AnchorPoint)g_lua.popInteger();

    std::string targetId;
    if(g_lua.isString())
        targetId = g_lua.popString();
    else if(UIElementPtr target = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        targetId = target->getId();

    AnchorPoint anchoredEdge = (AnchorPoint)g_lua.popInteger();

    if(UIElementPtr element = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance()))
        element->addAnchor(anchoredEdge, AnchorLine(targetId, targetEdge));
    return 0;
}



////////////////////////////////////////////////////////////////////////////////
// UIContainer

int lua_UIContainer_new() {
    g_lua.pushClassInstance(lua_createUIElement(UI::Container));
    return 1;
}

int lua_UIContainer_child() {
    std::string id = g_lua.popString();
    if(UIContainerPtr container = boost::dynamic_pointer_cast<UIContainer>(g_lua.popClassInstance()))
        g_lua.pushClassInstance(container->getChildById(id));
    else
        g_lua.pushNil();
    return 1;
}

int lua_UIContainer_getChildren() {
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

int lua_UIContainer_addChild() {
    UIElementPtr child = boost::dynamic_pointer_cast<UIElement>(g_lua.popClassInstance());
    UIContainerPtr container = boost::dynamic_pointer_cast<UIContainer>(g_lua.popClassInstance());
    if(container && child)
        container->addChild(child);
    return 0;
}



////////////////////////////////////////////////////////////////////////////////
// UILabel

int lua_UILabel_new() {
    g_lua.pushClassInstance(lua_createUIElement(UI::Label));
    return 1;
}

int lua_UILabel_setText() {
    std::string text = g_lua.popString();
    if(UILabelPtr label = boost::dynamic_pointer_cast<UILabel>(g_lua.popClassInstance()))
        label->setText(text);
    return 0;
}

int lua_UILabel_getText() {
    if(UILabelPtr label = boost::dynamic_pointer_cast<UILabel>(g_lua.popClassInstance()))
        g_lua.pushString(label->getText());
    else
        g_lua.pushNil();
    return 1;
}



////////////////////////////////////////////////////////////////////////////////
// UIButton

int lua_UIButton_new() {
    g_lua.pushClassInstance(lua_createUIElement(UI::Button));
    return 1;
}

int lua_UIButton_setText() {
    std::string text = g_lua.popString();
    if(UIButtonPtr button = boost::dynamic_pointer_cast<UIButton>(g_lua.popClassInstance()))
        button->setText(text);
    return 0;
}

int lua_UIButton_getText() {
    if(UIButtonPtr button = boost::dynamic_pointer_cast<UIButton>(g_lua.popClassInstance()))
        g_lua.pushString(button->getText());
    else
        g_lua.pushNil();
    return 1;
}



////////////////////////////////////////////////////////////////////////////////
// UITextEdit

int lua_UITextEdit_new() {
    g_lua.pushClassInstance(lua_createUIElement(UI::TextEdit));
    return 1;
}

int lua_UITextEdit_setText() {
    std::string text = g_lua.popString();
    if(UITextEditPtr textEdit = boost::dynamic_pointer_cast<UITextEdit>(g_lua.popClassInstance()))
        textEdit->setText(text);
    return 0;
}

int lua_UITextEdit_getText() {
    if(UITextEditPtr textEdit = boost::dynamic_pointer_cast<UITextEdit>(g_lua.popClassInstance()))
        g_lua.pushString(textEdit->getText());
    else
        g_lua.pushNil();
    return 1;
}



////////////////////////////////////////////////////////////////////////////////
// UIWindow

int lua_UIWindow_new() {
    g_lua.pushClassInstance(lua_createUIElement(UI::Window));
    return 1;
}

int lua_UIWindow_setTitle() {
    std::string title = g_lua.popString();
    if(UIWindowPtr window = boost::dynamic_pointer_cast<UIWindow>(g_lua.popClassInstance()))
        window->setTitle(title);
    return 0;
}

int lua_UIWindow_getTitle() {
    if(UIWindowPtr window = boost::dynamic_pointer_cast<UIWindow>(g_lua.popClassInstance()))
        g_lua.pushString(window->getTitle());
    else
        g_lua.pushNil();
    return 1;
}



void registerScriptFunctions()
{
    ////////////////////////////////////////////////////////////////////////////////
    // App
    g_lua.registerModule("App");
    // App.exit()
    g_lua.registerMemberFunction("exit", &lua_App_exit);


    ////////////////////////////////////////////////////////////////////////////////
    // UI
    g_lua.registerClass("UI");
    // UI.load(file, [parent])
    g_lua.registerMemberFunction("load", &lua_UI_load);
    // UI.getRootContainer()
    g_lua.registerMemberFunction("getRootContainer", &lua_UI_getRootContainer);


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
        g_lua.pushClassInstance(ScriptObjectPtr(new ProtocolLogin));
        return 1;
    });
    g_lua.registerMemberFunction("login", []{
        std::string accountPassword = g_lua.popString();
        std::string accountName = g_lua.popString();
        if(ProtocolLoginPtr protocolLogin = boost::dynamic_pointer_cast<ProtocolLogin>(g_lua.popClassInstance()))
            protocolLogin->login(accountName, accountPassword);
        return 0;
    });
    g_lua.registerMemberFunction("cancel", []{
        //TODO: this func
        return 0;
    });
}


