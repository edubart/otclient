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


#include "uicontainer.h"
#include "../resources.h"
#include "uibutton.h"
#include "uipanel.h"
#include "uilabel.h"
#include "uitextedit.h"
#include "uiwindow.h"

UIContainerPtr g_ui(new UIContainer);

UIElementPtr createElementFromDescription(std::string elementId)
{
    UIElementPtr element;

    std::vector<std::string> split;
    boost::split(split, elementId, boost::is_any_of("-"));
    if(split.size() != 2) {
        logError("incorrect element id format: %s", elementId.c_str());
        return element;
    }

    std::string elementType = split[1];
    if(elementType == "panel") {
        element = UIElementPtr(new UIPanel);
    } else if(elementType == "button") {
        element = UIElementPtr(new UIButton);
    } else if(elementType == "label") {
        element = UIElementPtr(new UILabel);
    } else if(elementType == "window") {
        element = UIElementPtr(new UIWindow);
    } else if(elementType == "textEdit") {
        element = UIElementPtr(new UITextEdit);
    }

    if(element)
        element->setId(elementId);

    return element;
}

void UIContainer::load(const YAML::Node& node)
{
    UIElement::load(node);

    for(auto it = node.begin(); it != node.end(); ++it) {
        std::string elementDesc;
        it.first() >> elementDesc;

        if(elementDesc.find("-") != std::string::npos) {
            UIElementPtr element = createElementFromDescription(elementDesc);
            if(element) {
                addChild(element);
                element->load(it.second());
            }
        }
    }
}

UIContainerPtr UIContainer::load(const std::string& file)
{
    //TODO: handle errors
    //TODO: display errors in which file and line
    UIContainerPtr container;

    std::string fileContents = g_resources.loadTextFile(file);
    if(!fileContents.size()) {
        logFatal("could not load ui file \"%s",  file.c_str());
        return UIContainerPtr();
    }

    std::istringstream fin(fileContents);

    try {
        YAML::Parser parser(fin);

        YAML::Node doc;
        parser.GetNextDocument(doc);

        std::string elementDesc;
        doc.begin().first() >> elementDesc;
        UIElementPtr element = createElementFromDescription(elementDesc);
        if(element) {
            g_ui->addChild(element);
            element->load(doc.begin().second());
            return element->asUIContainer();
        }
    } catch (YAML::ParserException& e) {
        logError("Malformed ui file \"%s\": %s", file.c_str(), e.what());
        return UIContainerPtr();
    }

    return container;
}

void UIContainer::addChild(UIElementPtr child)
{
    m_children.push_back(child);
    child->setParent(asUIContainer());
}

void UIContainer::removeChild(UIElementPtr child)
{
    if(m_activeElement == child)
        setActiveElement(UIElementPtr());
    m_children.remove(child);
    if(child->getParent() == shared_from_this())
        child->setParent(UIContainerPtr());
}

UIElementPtr UIContainer::getChildById(const std::string& id)
{
    if(getId() == id)
        return asUIElement();
    for(auto it = m_children.begin(); it != m_children.end(); ++it) {
        if((*it)->getId() == id) {
            return (*it);
        }
    }
    return UIElementPtr();
}

UIElementPtr UIContainer::recursiveGetChildById(const std::string& id)
{
    if(getId() == id)
        return asUIElement();

    UIElementPtr element;
    for(auto it = m_children.begin(); it != m_children.end(); ++it) {
        element = (*it);
        if(element->getId() == id) {
            return element;
        } else {
            UIContainerPtr container = element->asUIContainer();
            if(container) {
                element = container->recursiveGetChildById(id);
                if(element)
                    return element;
            }
        }
    }
    return UIElementPtr();
}

void UIContainer::render()
{
    UIElement::render();
    for(auto it = m_children.begin(); it != m_children.end(); ++it) {
        const UIElementPtr& child = (*it);
        if(child->isVisible())
            child->render();
    }
}

bool UIContainer::onInputEvent(const InputEvent& event)
{
    bool ret = false;
    for(auto it = m_children.begin(); it != m_children.end(); ++it) {
        const UIElementPtr& child = (*it);
        if(child->isEnabled() && child->isVisible())
            ret = child->onInputEvent(event) || ret;
    }
    return ret;
}

void UIContainer::setActiveElement(UIElementPtr activeElement)
{

}
