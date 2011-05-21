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

UILoader g_uiLoader;

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

UIElementPtr UILoader::loadFromFile(std::string filePath, const UIContainerPtr& parent)
{
    std::stringstream fin;
    if(!g_resources.loadFile(filePath, fin)) {
        flogError("ERROR: Could not load ui file \"%s", filePath.c_str());
        return UIElementPtr();
    }

    m_currentFile = filePath;

    FML::Parser parser(fin);
    if(!parser.hasError()) {
        FML::Node* doc = parser.getDocument();

        // get element id
        std::string elementId = doc->front()->tag();

        // first we should populate all elements
        // only after that we can load anchors

        // create element interpreting it's id
        UIElementPtr element = createElementFromId(elementId);
        if(!element) {
            logError(doc->front()->generateErrorMessage("invalid root element type"));
            return element;
        }
        parent->addChild(element);

        // populete it
        if(element->asUIContainer())
            populateContainer(element->asUIContainer(), doc->front());

        // now do the real load
        loadElements(element, doc->front());

        // report onLoad events
        element->onLoad();
        return element;
    } else {
        flogError("ERROR: Failed to load ui file \"%s\":\n  %s", filePath.c_str() % parser.getErrorMessage());
    }

    return UIElementPtr();
}

void UILoader::populateContainer(const UIContainerPtr& parent, FML::Node* node)
{
    // populate ordered elements
    foreach(FML::Node* cnode, *node) {
        std::string id = cnode->tag();
        if(id.find("#") != std::string::npos) {
            UIElementPtr element = createElementFromId(id);
            if(!element) {
                logError(cnode->generateErrorMessage("invalid element type"));
                continue;
            }
            parent->addChild(element);

            // also populate this element if it's a parent
            if(element->asUIContainer())
                populateContainer(element->asUIContainer(), cnode);
        }
    }
}

void UILoader::loadElements(const UIElementPtr& parent, FML::Node* node)
{
    loadElement(parent, node);

    if(UIContainerPtr container = parent->asUIContainer()) {
        foreach(const UIElementPtr& element, container->getChildren()) {
            foreach(FML::Node* cnode, *node) {
                // node found, load it
                if(boost::ends_with(cnode->tag(), "#" + element->getId())) {
                    loadElements(element, cnode);
                    break;
                }
            }
        }
    }
}

void UILoader::loadElement(const UIElementPtr& element, FML::Node* node)
{
    // set element skin
    if(FML::Node* cnode = node->at("skin")) {
        if(cnode->hasValue()) {
            element->setSkin(g_uiSkins.getElementSkin(element->getElementType(), cnode->value()));
        } else {
            UIElementSkinPtr skin = UIElementSkinPtr(new UIElementSkin());
            skin->load(cnode);
            element->setSkin(skin);
        }
    } else // apply default skin
        element->setSkin(g_uiSkins.getElementSkin(element->getElementType(), "default"));

    // load elements common proprieties
    if(node->hasNode("size"))
        element->setSize(node->readAt<Size>("size"));

    element->setMarginLeft(node->readAt("margin.left", 0));
    element->setMarginRight(node->readAt("margin.right", 0));
    element->setMarginTop(node->readAt("margin.top", 0));
    element->setMarginBottom(node->readAt("margin.bottom", 0));

    if(node->hasNode("anchors.left"))
        loadElementAnchor(element, UI::AnchorLeft, node->at("anchors.left"));

    if(node->hasNode("anchors.right"))
        loadElementAnchor(element, UI::AnchorRight, node->at("anchors.right"));

    if(node->hasNode("anchors.top"))
        loadElementAnchor(element, UI::AnchorTop, node->at("anchors.top"));

    if(node->hasNode("anchors.bottom"))
        loadElementAnchor(element, UI::AnchorBottom, node->at("anchors.bottom"));

    if(node->hasNode("anchors.horizontalCenter"))
        loadElementAnchor(element, UI::AnchorHorizontalCenter, node->at("anchors.horizontalCenter"));

    if(node->hasNode("anchors.verticalCenter"))
        loadElementAnchor(element, UI::AnchorVerticalCenter, node->at("anchors.verticalCenter"));

    // load events
    if(FML::Node* cnode = node->at("onLoad")) {
        if(g_lua.loadBufferAsFunction(cnode->value(), getElementSourceDesc(element, "onLoad")))
            g_lua.setScriptableField(element, "onLoad");
        else
            logError(cnode->generateErrorMessage("failed to parse inline lua script"));
    }

    if(FML::Node* cnode = node->at("onDestroy")) {
        if(g_lua.loadBufferAsFunction(cnode->value(), getElementSourceDesc(element, "onDestroy")))
            g_lua.setScriptableField(element, "onDestroy");
        else
            logError(cnode->generateErrorMessage("failed to parse inline lua script"));
    }

    // load specific element type
    if(element->getElementType() == UI::Button)
        loadButton(boost::static_pointer_cast<UIButton>(element), node);
    else if(element->getElementType() == UI::Window) {
        UIWindowPtr window = boost::static_pointer_cast<UIWindow>(element);
        window->setTitle(node->readAt("title", std::string()));
    }
    else if(element->getElementType() == UI::Label) {
        UILabelPtr label = boost::static_pointer_cast<UILabel>(element);
        label->setText(node->readAt("text", std::string()));
        label->setAlign(parseAlignment(node->readAt("align", std::string("left"))));
    }
}

void UILoader::loadElementAnchor(const UIElementPtr& anchoredElement, UI::AnchorPoint anchoredEdge, FML::Node* node)
{
    UIAnchorLayoutPtr layout = boost::dynamic_pointer_cast<UIAnchorLayout>(anchoredElement->getLayout());
    if(!layout) {
        logError(node->generateErrorMessage("could not add anchor, because this element does not participate of an anchor layout"));
        return;
    }

    std::string anchorDescription = node->value();

    std::vector<std::string> split;
    boost::split(split, anchorDescription, boost::is_any_of(std::string(".")));
    if(split.size() != 2) {
        logError(node->generateErrorMessage("invalid anchor"));
        return;
    }

    std::string anchorLineElementId = split[0];
    UI::AnchorPoint anchorLineEdge = UIAnchorLayout::parseAnchorPoint(split[1]);

    if(anchorLineEdge == UI::AnchorNone) {
        logError(node->generateErrorMessage("invalid anchor type"));
        return;
    }

    if(!layout->addAnchor(anchoredElement, anchoredEdge, AnchorLine(anchorLineElementId, anchorLineEdge)))
        logError(node->generateErrorMessage("anchoring failed"));
}

void UILoader::loadButton(const UIButtonPtr& button, FML::Node* node)
{
    button->setText(node->valueAt("text"));

    // set on click event
    if(FML::Node* cnode = node->at("onClick")) {
        if(g_lua.loadBufferAsFunction(cnode->value(), getElementSourceDesc(button, "onClick")))
            g_lua.setScriptableField(button, "onClick");
        else
            logError(cnode->generateErrorMessage("failed to parse inline lua script"));
    }
}

std::string UILoader::getElementSourceDesc(const UIElementPtr& element, const std::string& key)
{
    std::string desc;
    desc += g_resources.resolvePath(m_currentFile) + ":" + element->getId();
    if(key.length() > 0)
        desc += "[" + key + "]";
    return desc;
}

