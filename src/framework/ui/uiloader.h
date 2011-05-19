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


#ifndef UILOADER_H
#define UILOADER_H

#include <prerequisites.h>
#include <ui/uiconstants.h>
#include <ui/uicontainer.h>
#include <ui/uibutton.h>

class UILoader
{
public:
    /// Loads an UIElement and it's children from a YAML file
    UIElementPtr loadFromYAML(std::string filePath, const UIContainerPtr& parent = UIContainer::getRoot());

private:
    /// Detect element type and create it
    UIElementPtr createElementFromId(const std::string& id);

    /// Populate container children from a YAML node
    void populateContainer(const UIContainerPtr& parent, const YAML::Node& node);

    /// Load element and its children from a YAML node
    void loadElements(const UIElementPtr& parent, const YAML::Node& node);

    /// Load element proprieties from a YAML node
    void loadElement(const UIElementPtr& element, const YAML::Node& node);

    /// Load anchor from a YAML node
    void loadElementAnchor(const UIElementPtr& anchoredElement, UI::AnchorPoint anchoredEdge, const YAML::Node& node);

    // specific elements loading
    void loadButton(const UIButtonPtr& button, const YAML::Node& node);

    std::string getElementSourceDesc(const UIElementPtr& element, const std::string& key = "");

    std::string m_currentFile;
};

extern UILoader g_uiLoader;

#endif // UILOADER_H
