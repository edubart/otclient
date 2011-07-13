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


#include <global.h>
#include <core/resources.h>
#include <graphics/textures.h>
#include <ui/uiskins.h>
#include <ui/uielementskin.h>
#include <ui/uibuttonskin.h>
#include <ui/uiwindowskin.h>
#include <ui/uitexteditskin.h>
#include <ui/uilabelskin.h>
#include <graphics/fonts.h>
#include <otml/otml.h>

UISkins g_uiSkins;

void UISkins::load(const std::string& skinName)
{
    g_resources.pushCurrentPath("skins");

    std::stringstream fin;
    if(!g_resources.loadFile(skinName + ".yml", fin))
        fatal("FATAL ERROR: Could not load skin '",skinName,"'");

    try {
        OTMLParser parser(fin, skinName);
        OTMLNode* doc = parser.getDocument();

        m_defaultFont = g_fonts.get(doc->valueAt("default font"));
        if(!m_defaultFont)
            fatal("FATAL ERROR: Could not load skin default font");

        m_defaultFontColor = doc->readAt("default font color", Color::white);

        std::string defaultTextureName = doc->readAt("default texture", std::string());
        if(!defaultTextureName.empty())
            m_defaultTexture = g_textures.get(defaultTextureName);

        foreach(OTMLNode* node, *doc) {
            UIElementSkinPtr skin;
            foreach(OTMLNode* cnode, *node) {
                if(node->tag() == "buttons")
                    skin = UIElementSkinPtr(new UIButtonSkin(cnode->tag()));
                else if(node->tag() == "panels")
                    skin = UIElementSkinPtr(new UIElementSkin(cnode->tag(), UI::Panel));
                else if(node->tag() == "windows")
                    skin = UIElementSkinPtr(new UIWindowSkin(cnode->tag()));
                else if(node->tag() == "labels")
                    skin = UIElementSkinPtr(new UILabelSkin(cnode->tag()));
                else if(node->tag() == "text edits")
                    skin = UIElementSkinPtr(new UITextEditSkin(cnode->tag()));
                else if(node->tag() == "line decorations")
                    skin = UIElementSkinPtr(new UIElementSkin(cnode->tag(), UI::LineDecoration));
                else {
                    break;
                }
                skin->load(cnode);
                m_elementSkins.push_back(skin);
            }
        }
    } catch(OTMLException e) {
        fatal("FATAL ERROR: Malformed skin file '",skinName,"':\n  ",e.what());
    }

    g_resources.popCurrentPath();
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
    warning("Element skin '",name,"' not found");
    return UIElementSkinPtr();
}
