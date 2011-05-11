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
#include <ui/uiskins.h>
#include <ui/uielement.h>
#include <ui/uielementskin.h>
#include <graphics/borderedimage.h>
#include <graphics/textures.h>
#include <graphics/fonts.h>

void UIElementSkin::load(const YAML::Node& node)
{
    m_defaultSize = yamlRead(node, "default size", Size());
    m_defaultImage = loadImage(node);
    m_font = loadFont(node);
    m_fontColor = yamlRead(node, "font color", g_uiSkins.getDefaultFontColor());
}

void UIElementSkin::apply(UIElement* element)
{
    if(!element->getRect().isValid() && m_defaultSize.isValid())
        element->setSize(m_defaultSize);
}

void UIElementSkin::draw(UIElement *element)
{
    if(m_defaultImage)
        m_defaultImage->draw(element->getRect());
}

ImagePtr UIElementSkin::loadImage(const YAML::Node& node)
{
    ImagePtr image;
    TexturePtr texture;

    if(yamlHasValue(node, "bordered image")) {
        const YAML::Node& cnode = node["bordered image"];
        Rect left = yamlRead(cnode, "left border", Rect());
        Rect right = yamlRead(cnode, "right border", Rect());
        Rect top = yamlRead(cnode, "top border", Rect());
        Rect bottom = yamlRead(cnode, "bottom border", Rect());
        Rect topLeft = yamlRead(cnode, "top left corner", Rect());
        Rect topRight = yamlRead(cnode, "top right corner", Rect());
        Rect bottomLeft = yamlRead(cnode, "bottom left corner", Rect());
        Rect bottomRight = yamlRead(cnode, "bottom right corner", Rect());
        Rect center = yamlRead(cnode, "center", Rect());
        std::string textureName = yamlRead(cnode, "source", std::string());

        if(!textureName.empty())
            texture = g_textures.get("skins/" + textureName);
        else
            texture = g_uiSkins.getDefaultTexture();

        if(texture) {
            image = ImagePtr(new BorderedImage(texture,
                                            left,
                                            right,
                                            top,
                                            bottom,
                                            topLeft,
                                            topRight,
                                            bottomLeft,
                                            bottomRight,
                                            center));
        }

        if(!image)
            logError(yamlErrorDesc(cnode, "failed to load bordered image"));
    } else if(yamlHasValue(node, "image")) {
        texture = g_textures.get("skins/" + yamlRead<std::string>(node, "image"));
        if(texture)
            image = ImagePtr(new Image(texture));

        if(!image)
            logError(yamlErrorDesc(node["image"], "failed to load image"));
    }

    if(texture) {
        bool antialised = yamlRead(node, "antialised", false);
        if(antialised)
            texture->enableBilinearFilter();
    }

    return image;
}

FontPtr UIElementSkin::loadFont(const YAML::Node& node)
{
    FontPtr font;
    if(yamlHasValue(node, "font"))
        font = g_fonts.get(yamlRead<std::string>(node, "font"));
    if(!font)
        font = g_uiSkins.getDefaultFont();
    return font;
}


