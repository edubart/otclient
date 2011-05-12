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
#include "uianchorlayout.h"

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
    // try to find the file
    std::string filePath = "modules/" + file;
    if(!g_resources.fileExists(filePath))
        filePath = "addons/" + file;
    if(!g_resources.fileExists(filePath))
        filePath = file;
    if(!g_resources.fileExists(filePath)) {
        flogError("ERROR: Could not load ui file \"%s",  file.c_str());
        return UIElementPtr();
    }

    std::istringstream fin(g_resources.loadTextFile(filePath));

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
            yamlThrowError(doc.begin().first(), "invalid element type");
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
    // order nodes
    std::map<int, std::string> orderedNodes;
    for(auto it = node.begin(); it != node.end(); ++it) {
        std::string id;
        it.first() >> id;

        // check if it's an element id
        if(id.find("#") != std::string::npos)
            orderedNodes[it.first().GetMark().pos] = id;
    }

    // populate ordered elements
    foreach(auto pair, orderedNodes) {
        std::string id = pair.second;
        const YAML::Node& cnode = node[id];

        UIElementPtr element = createElementFromId(id);
        if(!element) {
            logError(yamlErrorDesc(cnode, "invalid element type"));
            continue;
        }
        parent->addChild(element);

        // also populate this element if it's a parent
        if(element->asUIContainer())
            populateContainer(element->asUIContainer(), cnode);
    }
}

void UILoader::loadElements(const UIElementPtr& parent, const YAML::Node& node)
{
    loadElement(parent, node);

    if(UIContainerPtr container = parent->asUIContainer()) {
        foreach(const UIElementPtr& element, container->getChildren()) {
            for(auto it = node.begin(); it != node.end(); ++it) {
                // node found, load it
                if(boost::ends_with(it.first().Read<std::string>(), "#" + element->getId())) {
                    loadElements(element, it.second());
                    break;
                }
            }
        }
    }
}

void UILoader::loadElement(const UIElementPtr& element, const YAML::Node& node)
{
    // set element skin
    if(yamlHasValue(node, "skin")) {
        const YAML::Node& cnode = node["skin"];
        if(cnode.GetType() == YAML::CT_SCALAR) {
            element->setSkin(g_uiSkins.getElementSkin(element->getElementType(), cnode));
        } else {
            UIElementSkinPtr skin = UIElementSkinPtr(new UIElementSkin());
            skin->load(cnode);
            element->setSkin(skin);
        }
    } else // apply default skin
        element->setSkin(g_uiSkins.getElementSkin(element->getElementType(), "default"));

    // load elements common proprieties
    if(yamlHasValue(node, "size"))
        element->setSize(yamlRead<Size>(node, "size"));
    element->setMarginLeft(yamlRead(node, "margin.left", 0));
    element->setMarginRight(yamlRead(node, "margin.right", 0));
    element->setMarginTop(yamlRead(node, "margin.top", 0));
    element->setMarginBottom(yamlRead(node, "margin.bottom", 0));

    if(yamlHasValue(node, "anchors.left"))
        loadElementAnchor(element, UI::AnchorLeft, node["anchors.left"]);

    if(yamlHasValue(node, "anchors.right"))
        loadElementAnchor(element, UI::AnchorRight, node["anchors.right"]);

    if(yamlHasValue(node, "anchors.top"))
        loadElementAnchor(element, UI::AnchorTop, node["anchors.top"]);

    if(yamlHasValue(node, "anchors.bottom"))
        loadElementAnchor(element, UI::AnchorBottom, node["anchors.bottom"]);

    if(yamlHasValue(node, "anchors.horizontalCenter"))
        loadElementAnchor(element, UI::AnchorHorizontalCenter, node["anchors.horizontalCenter"]);

    if(yamlHasValue(node, "anchors.verticalCenter"))
        loadElementAnchor(element, UI::AnchorVerticalCenter, node["anchors.verticalCenter"]);

    // load events
    if(yamlHasValue(node, "onLoad")) {
        const YAML::Node& cnode = node["onLoad"];
        if(g_lua.loadBufferAsFunction(yamlRead<std::string>(cnode), element->getId() + ":onLoad"))
            g_lua.setScriptableField(element, "onLoad");
        else
            logError(yamlErrorDesc(cnode, "failed to parse inline lua script"));
    }

    if(yamlHasValue(node, "onDestroy")) {
        const YAML::Node& cnode = node["onDestroy"];
        if(g_lua.loadBufferAsFunction(yamlRead<std::string>(cnode), element->getId() + ":onDestroy"))
            g_lua.setScriptableField(element, "onDestroy");
        else
            logError(yamlErrorDesc(cnode, "failed to parse inline lua script"));
    }

    // load specific element type
    if(element->getElementType() == UI::Button)
        loadButton(boost::static_pointer_cast<UIButton>(element), node);
    else if(element->getElementType() == UI::Window) {
        UIWindowPtr window = boost::static_pointer_cast<UIWindow>(element);
        window->setTitle(yamlRead(node, "title", std::string()));
    }
    else if(element->getElementType() == UI::Label) {
        UILabelPtr label = boost::static_pointer_cast<UILabel>(element);
        label->setText(yamlRead(node, "text", std::string()));
        label->setAlign(parseAlignment(yamlRead(node, "align", std::string("left"))));
    }
}

void UILoader::loadElementAnchor(const UIElementPtr& anchoredElement, UI::AnchorPoint anchoredEdge, const YAML::Node& node)
{
    UIAnchorLayoutPtr layout = boost::dynamic_pointer_cast<UIAnchorLayout>(anchoredElement->getLayout());
    if(!layout) {
        logError(yamlErrorDesc(node, "could not add anchor, because this element does not participate of an anchor layout"));
        return;
    }

    std::string anchorDescription;
    node >> anchorDescription;

    std::vector<std::string> split;
    boost::split(split, anchorDescription, boost::is_any_of(std::string(".")));
    if(split.size() != 2) {
        logError(yamlErrorDesc(node, "invalid anchor"));
        return;
    }

    std::string anchorLineElementId = split[0];
    UI::AnchorPoint anchorLineEdge = UIAnchorLayout::parseAnchorPoint(split[1]);

    if(anchorLineEdge == UI::AnchorNone) {
        logError(yamlErrorDesc(node, "invalid anchor type"));
        return;
    }

    if(!layout->addAnchor(anchoredElement, anchoredEdge, AnchorLine(anchorLineElementId, anchorLineEdge)))
        logError(yamlErrorDesc(node, "anchoring failed"));
}

void UILoader::loadButton(const UIButtonPtr& button, const YAML::Node& node)
{
    button->setText(node["text"].Read<std::string>());

    // set on click event
    if(yamlHasValue(node, "onClick")) {
        const YAML::Node& cnode = node["onClick"];
        if(g_lua.loadBufferAsFunction(yamlRead<std::string>(cnode), button->getId() + ":onClick"))
            g_lua.setScriptableField(button, "onClick");
        else
            logError(yamlErrorDesc(cnode, "failed to parse inline lua script"));
    }
}
