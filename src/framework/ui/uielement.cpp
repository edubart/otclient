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


#include "uielement.h"
#include "uiskins.h"
#include "uielementskin.h"

UIElement::UIElement(UI::EElementType type) :
    AnchorLayout(),
    m_type(type),
    m_skin(NULL),
    m_visible(true),
    m_enabled(true)
{
    // set default skin
    if(type > UI::Container)
        setSkin(g_uiSkins.getElementSkin(type));
}

void UIElement::load(const YAML::Node& node)
{
    if(node.FindValue("skin"))
        setSkin(g_uiSkins.getElementSkin(m_type, node["skin"]));

    if(node.FindValue("size")) {
        Size size;
        node["size"] >> size;
        setSize(size);
    }

    int margin;
    if(node.FindValue("margin.left")) {
        node["margin.left"] >> margin;
        setMarginLeft(margin);
    }

    if(node.FindValue("margin.right")) {
        node["margin.right"] >> margin;
        setMarginRight(margin);
    }

    if(node.FindValue("margin.top")) {
        node["margin.top"] >> margin;
        setMarginTop(margin);
    }

    if(node.FindValue("margin.bottom")) {
        node["margin.bottom"] >> margin;
        setMarginBottom(margin);
    }

    if(node.FindValue("anchors.left"))
        loadAnchor(ANCHOR_LEFT, node["anchors.left"]);

    if(node.FindValue("anchors.right"))
        loadAnchor(ANCHOR_RIGHT, node["anchors.right"]);

    if(node.FindValue("anchors.top"))
        loadAnchor(ANCHOR_TOP, node["anchors.top"]);

    if(node.FindValue("anchors.bottom"))
        loadAnchor(ANCHOR_BOTTOM, node["anchors.bottom"]);

    if(node.FindValue("anchors.horizontalCenter"))
        loadAnchor(ANCHOR_HORIZONTAL_CENTER, node["anchors.horizontalCenter"]);

    if(node.FindValue("anchors.verticalCenter"))
        loadAnchor(ANCHOR_VERTICAL_CENTER, node["anchors.verticalCenter"]);
}

void UIElement::loadAnchor(EAnchorType type, const YAML::Node& node)
{
    std::string anchorDescription;
    node >> anchorDescription;

    std::vector<std::string> split;
    boost::split(split, anchorDescription, boost::is_any_of("."));
    if(split.size() != 2) {
        logError("wrong anchors description: %s", anchorDescription.c_str());
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
        logError("wrong anchors description: %s", anchorDescription.c_str());
        return;
    }

    AnchorLayoutPtr relativeElement;
    if(relativeElementId == "parent" && getParent()) {
        relativeElement = getParent()->asAnchorLayout();
    } else {
        UIElementPtr element = g_ui->recursiveGetChildById(relativeElementId);
        if(element)
            relativeElement = element->asAnchorLayout();
    }

    if(relativeElement) {
        addAnchor(type, AnchorLine(relativeElement, relativeAnchorType));
    } else {
        logError("anchoring has failed: %s", anchorDescription.c_str());
        return;
    }
}

bool UIElement::setSkin(const std::string& skinName)
{
    setSkin(g_uiSkins.getElementSkin(m_type, skinName));
    return m_skin != NULL;
}

void UIElement::setSkin(UIElementSkin* skin)
{
    if(skin && !getRect().isValid()) {
        setSize(skin->getDefaultSize());
    }
    m_skin = skin;
}

void UIElement::render()
{
    if(m_skin)
        m_skin->draw(this);
}
