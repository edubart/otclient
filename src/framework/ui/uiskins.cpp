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
#include <graphics/textures.h>
#include <ui/uiskins.h>
#include <ui/uielementskin.h>
#include <ui/uibuttonskin.h>
#include <ui/uiwindowskin.h>
#include <ui/uitexteditskin.h>
#include <ui/uilabelskin.h>
#include <graphics/fonts.h>

UISkins g_uiSkins;

void UISkins::load(const std::string& skinsFile)
{
    std::string fileContents = g_resources.loadTextFile(skinsFile);
    if(!fileContents.size())
        flogFatal("FATAL ERROR: Could not load skin file \"%s",  skinsFile.c_str());

    std::istringstream fin(fileContents);

    try {
        YAML::Parser parser(fin);

        YAML::Node doc;
        parser.GetNextDocument(doc);

        m_defaultFont = g_fonts.get(yamlRead<std::string>(doc, "default font"));
        if(!m_defaultFont)
            logFatal("FATAL ERROR: Could not load skin default font");

        m_defaultFontColor = yamlRead(doc, "default font color", Color::white);

        std::string defaultTextureName = yamlRead(doc, "default texture", std::string());
        if(!defaultTextureName.empty())
            m_defaultTexture = g_textures.get("skins/" + defaultTextureName);

        {
            const YAML::Node& node = doc["buttons"];
            for(auto it = node.begin(); it != node.end(); ++it) {
                std::string name;
                it.first() >> name;

                UIElementSkinPtr skin = UIElementSkinPtr(new UIButtonSkin(name));
                skin->load(it.second());
                m_elementSkins.push_back(skin);
            }
        }

        {
            const YAML::Node& node = doc["panels"];
            for(auto it = node.begin(); it != node.end(); ++it) {
                std::string name;
                it.first() >> name;

                UIElementSkinPtr skin = UIElementSkinPtr(new UIElementSkin(name, UI::Panel));
                skin->load(it.second());
                m_elementSkins.push_back(skin);
            }
        }

        {
            const YAML::Node& node = doc["windows"];
            for(auto it = node.begin(); it != node.end(); ++it) {
                std::string name;
                it.first() >> name;

                UIElementSkinPtr skin = UIElementSkinPtr(new UIWindowSkin(name));
                skin->load(it.second());
                m_elementSkins.push_back(skin);
            }
        }

        {
            const YAML::Node& node = doc["labels"];
            for(auto it = node.begin(); it != node.end(); ++it) {
                std::string name;
                it.first() >> name;

                UIElementSkinPtr skin = UIElementSkinPtr(new UILabelSkin(name));
                skin->load(it.second());
                m_elementSkins.push_back(skin);
            }
        }


        {
            const YAML::Node& node = doc["text edits"];
            for(auto it = node.begin(); it != node.end(); ++it) {
                std::string name;
                it.first() >> name;

                UIElementSkinPtr skin = UIElementSkinPtr(new UITextEditSkin(name));
                skin->load(it.second());
                m_elementSkins.push_back(skin);
            }
        }


        {
            const YAML::Node& node = doc["line decorations"];
            for(auto it = node.begin(); it != node.end(); ++it) {
                std::string name;
                it.first() >> name;

                UIElementSkinPtr skin = UIElementSkinPtr(new UIElementSkin(name, UI::LineDecoration));
                skin->load(it.second());
                m_elementSkins.push_back(skin);
            }
        }
    } catch (YAML::Exception& e) {
        flogFatal("FATAL ERROR: Malformed skin file \"%s\":\n  %s", skinsFile.c_str() % e.what());
    }
}

void UISkins::terminate()
{

}


UIElementSkinPtr UISkins::getElementSkin(UI::ElementType elementType, const std::string& name)
{
    for(auto it = m_elementSkins.begin(); it != m_elementSkins.end(); ++it) {
        const UIElementSkinPtr& skin = (*it);
        if(elementType == skin->getElementType() && name == skin->getName())
            return skin;
    }
    flogWarning("Element skin '%s' not found", name.c_str());
    return UIElementSkinPtr();
}
