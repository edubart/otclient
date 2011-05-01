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
#include <core/resources.h>
#include <ui/ui.h>
#include <ui/uiloader.h>
#include <script/luascript.h>
#include <script/luafunctions.h>

UIElementPtr UILoader::createElementFromId(const std::string& id)
{
    UIElementPtr element;

    std::vector<std::string> split;
    boost::split(split, id, boost::is_any_of(std::string("#")));
    if(split.size() != 2)
        return element;

    std::string elementType = split[0];
    std::string elementId = split[1];

    if(elementType == "panel") {
        element = UIElementPtr(new UIContainer(UI::Panel));
    } else if(elementType == "button") {
        element = UIElementPtr(new UIButton);
    } else if(elementType == "label") {
        element = UIElementPtr(new UILabel);
    } else if(elementType == "window") {
        element = UIElementPtr(new UIWindow);
    } else if(elementType == "textEdit") {
        element = UIElementPtr(new UITextEdit);
    } else if(elementType == "lineDecoration") {
        element = UIElementPtr(new UIElement(UI::LineDecoration));
    } else if(elementType == "checkBox") {
        element = UIElementPtr(new UICheckBox);
    }

    if(element) {
        element->setId(elementId);
    }

    return element;
}

UIElementPtr UILoader::loadFile(const std::string& file, const UIContainerPtr& parent)
{
    std::string fileContents = g_resources.loadTextFile(file);
    if(!fileContents.size()) {
        flogError("ERROR: Could not load ui file \"%s",  file.c_str());
        return UIElementPtr();
    }

    std::istringstream fin(fileContents);

    try {
        YAML::Parser parser(fin);

        YAML::Node doc;
        parser.GetNextDocument(doc);

        // get element id
        std::string elementId;
        doc.begin().first() >> elementId;

        // first we should populate all elements
        // only after that we can load anchors

        // create element interpreting it's id
        UIElementPtr element = createElementFromId(elementId);
        if(!element)
            throw YAML::Exception(doc.begin().first().GetMark(), "invalid element type");
        parent->addChild(element);

        // populete it
        if(element->asUIContainer())
            populateContainer(element->asUIContainer(), doc.begin().second());

        // now do the real load
        loadElements(element, doc.begin().second());

        // report onLoad events
        element->onLoad();
        return element;
    } catch (YAML::Exception& e) {
        flogError("ERROR: Failed to load ui file \"%s\":\n  %s", file.c_str() % e.what());
    }

    return UIElementPtr();
}

void UILoader::populateContainer(const UIContainerPtr& parent, const YAML::Node& node)
{
    for(auto it = node.begin(); it != node.end(); ++it) {
        std::string id;
        it.first() >> id;

        // check if it's and element id
        if(id.find("#") != std::string::npos) {
            UIElementPtr element = createElementFromId(id);
            if(!element) {
                logError(YAML::Exception(it.first().GetMark(), "invalid element type").what());
                continue;
            }
            parent->addChild(element);

            // also populate this element if it's a parent
            if(element->asUIContainer())
                populateContainer(element->asUIContainer(), it.second());
        }
    }
}

void UILoader::loadElements(const UIElementPtr& parent, const YAML::Node& node)
{
    loadElement(parent, node);

    if(parent->asUIContainer()) {
        UIContainerPtr container = parent->asUIContainer();
        for(auto it = node.begin(); it != node.end(); ++it) {
            std::string id;
            it.first() >> id;

            // check if it's and element id
            if(id.find("#") != std::string::npos) {
                std::vector<std::string> split;
                boost::split(split, id, boost::is_any_of(std::string("#")));
                loadElements(container->getChildById(split[1]), it.second());
            }
        }
    }
}

void UILoader::loadElement(const UIElementPtr& element, const YAML::Node& node)
{
    // set element skin
    if(node.FindValue("skin")) {
        if(node["skin"].GetType() == YAML::CT_SCALAR) {
            element->setSkin(g_uiSkins.getElementSkin(element->getElementType(), node["skin"]));
        } else {
            UIElementSkinPtr skin = UIElementSkinPtr(new UIElementSkin());
            skin->load(node["skin"]);
            element->setSkin(skin);
        }
    } else // apply default skin
        element->setSkin(g_uiSkins.getElementSkin(element->getElementType(), "default"));

    // load elements common proprieties
    if(node.FindValue("size")) {
        Size size;
        node["size"] >> size;
        element->setSize(size);
    }

    int margin;
    if(node.FindValue("margin.left")) {
        node["margin.left"] >> margin;
        element->setMarginLeft(margin);
    }

    if(node.FindValue("margin.right")) {
        node["margin.right"] >> margin;
        element->setMarginRight(margin);
    }

    if(node.FindValue("margin.top")) {
        node["margin.top"] >> margin;
        element->setMarginTop(margin);
    }

    if(node.FindValue("margin.bottom")) {
        node["margin.bottom"] >> margin;
        element->setMarginBottom(margin);
    }

    if(node.FindValue("anchors.left"))
        loadElementAnchor(element, ANCHOR_LEFT, node["anchors.left"]);

    if(node.FindValue("anchors.right"))
        loadElementAnchor(element, ANCHOR_RIGHT, node["anchors.right"]);

    if(node.FindValue("anchors.top"))
        loadElementAnchor(element, ANCHOR_TOP, node["anchors.top"]);

    if(node.FindValue("anchors.bottom"))
        loadElementAnchor(element, ANCHOR_BOTTOM, node["anchors.bottom"]);

    if(node.FindValue("anchors.horizontalCenter"))
        loadElementAnchor(element, ANCHOR_HORIZONTAL_CENTER, node["anchors.horizontalCenter"]);

    if(node.FindValue("anchors.verticalCenter"))
        loadElementAnchor(element, ANCHOR_VERTICAL_CENTER, node["anchors.verticalCenter"]);

    // load events
    if(node.FindValue("onLoad")) {
        const YAML::Node& cnode = node["onLoad"];
        int funcRef = g_lua.loadBufferAsFunction(cnode.Read<std::string>());
        if(funcRef != LUA_REFNIL) {
            g_lua.pushClassInstance(element);
            g_lua.pushFunction(funcRef);
            lua_UIElement_setOnLoad();
        } else
            logError(YAML::Exception(cnode.GetMark(), "failed to parse inline lua script").what());
    }

    if(node.FindValue("onDestroy")) {
        const YAML::Node& cnode = node["onDestroy"];
        int funcRef = g_lua.loadBufferAsFunction(cnode.Read<std::string>());
        if(funcRef != LUA_REFNIL) {
            g_lua.pushClassInstance(element);
            g_lua.pushFunction(funcRef);
            lua_UIElement_setOnDestroy();
        } else
            logError(YAML::Exception(cnode.GetMark(), "failed to parse inline lua script").what());
    }

    // load specific element type
    if(element->getElementType() == UI::Button)
        loadButton(boost::static_pointer_cast<UIButton>(element), node);
    else if(element->getElementType() == UI::Window) {
        UIWindowPtr window = boost::static_pointer_cast<UIWindow>(element);
        if(node.FindValue("title"))
            window->setTitle(node["title"].Read<std::string>());
    }
    else if(element->getElementType() == UI::Label) {
        UILabelPtr label = boost::static_pointer_cast<UILabel>(element);
        if(node.FindValue("text"))
            label->setText(node["text"].Read<std::string>());
        if(node.FindValue("align")) {
            std::string alignDesc;
            node["align"] >> alignDesc;
            if(alignDesc == "center")
                label->setAlign(ALIGN_CENTER);
        }
    }
}

void UILoader::loadElementAnchor(const UIElementPtr& element, EAnchorType type, const YAML::Node& node)
{
    std::string anchorDescription;
    node >> anchorDescription;

    std::vector<std::string> split;
    boost::split(split, anchorDescription, boost::is_any_of(std::string(".")));
    if(split.size() != 2) {
        logError(YAML::Exception(node.GetMark(), "invalid anchor").what());
        return;
    }

    std::string relativeElementId = split[0];
    std::string relativeAnchorTypeId = split[1];
    EAnchorType relativeAnchorType;

    if(relativeAnchorTypeId == "left")
        relativeAnchorType = ANCHOR_LEFT;
    else if(relativeAnchorTypeId == "right")
        relativeAnchorType = ANCHOR_RIGHT;
    else if(relativeAnchorTypeId == "top")
        relativeAnchorType = ANCHOR_TOP;
    else if(relativeAnchorTypeId == "bottom")
        relativeAnchorType = ANCHOR_BOTTOM;
    else if(relativeAnchorTypeId == "horizontalCenter")
        relativeAnchorType = ANCHOR_HORIZONTAL_CENTER;
    else if(relativeAnchorTypeId == "verticalCenter")
        relativeAnchorType = ANCHOR_VERTICAL_CENTER;
    else {
        logError(YAML::Exception(node.GetMark(), "invalid anchor type").what());
        return;
    }

    UILayoutPtr relativeElement;
    if(relativeElementId == "parent" && element->getParent()) {
        relativeElement = element->getParent()->asUILayout();
    } else if(relativeElementId == "root") {
        relativeElement = UIContainer::getRoot();
    } else {
        UIElementPtr tmp = element->backwardsGetElementById(relativeElementId);
        if(tmp)
            relativeElement = tmp->asUILayout();
    }

    if(relativeElement) {
        if(!element->addAnchor(type, AnchorLine(relativeElement, relativeAnchorType)))
            logError(YAML::Exception(node.GetMark(), "anchoring failed").what());
    } else {
        logError(YAML::Exception(node.GetMark(), "anchoring failed, does the relative element really exists?").what());
    }
}

void UILoader::loadButton(const UIButtonPtr& button, const YAML::Node& node)
{
    button->setText(node["text"].Read<std::string>());

    // set on click event
    if(node.FindValue("onClick")) {
        int funcRef = g_lua.loadBufferAsFunction(node["onClick"].Read<std::string>());
        if(funcRef != LUA_REFNIL) {
            g_lua.pushClassInstance(button);
            g_lua.pushFunction(funcRef);
            lua_UIButton_setOnClick();
        } else {
            logError(YAML::Exception(node["onClick"].GetMark(), "failed to parse inline lua script").what());
        }
    }
}
