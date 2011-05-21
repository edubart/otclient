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

void UIElementSkin::load(FML::Node* node)
{
    m_defaultSize = node->readAt("default size", Size());
    m_defaultImage = loadImage(node);
    m_font = loadFont(node);
    m_fontColor = node->readAt("font color", g_uiSkins.getDefaultFontColor());
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

ImagePtr UIElementSkin::loadImage(FML::Node* node)
{
    ImagePtr image;
    TexturePtr texture;

    if(FML::Node* cnode = node->at("bordered image")) {
        Rect left = cnode->readAt("left border", Rect());
        Rect right = cnode->readAt("right border", Rect());
        Rect top = cnode->readAt("top border", Rect());
        Rect bottom = cnode->readAt("bottom border", Rect());
        Rect topLeft = cnode->readAt("top left corner", Rect());
        Rect topRight = cnode->readAt("top right corner", Rect());
        Rect bottomLeft = cnode->readAt("bottom left corner", Rect());
        Rect bottomRight = cnode->readAt("bottom right corner", Rect());
        Rect center = cnode->readAt("center", Rect());
        std::string textureName = cnode->readAt("source", std::string());

        if(!textureName.empty())
            texture = g_textures.get(textureName);
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
            logError(node->generateErrorMessage("failed to load bordered image"));
    } else if(FML::Node* cnode = node->at("image")) {
        texture = g_textures.get(cnode->value());
        if(texture)
            image = ImagePtr(new Image(texture));
        if(!m_defaultSize.isValid())
            m_defaultSize = texture->getSize();

        if(!image)
            logError(cnode->generateErrorMessage("failed to load image"));
    }

    if(texture) {
        bool antialised = node->readAt("antialised", false);
        if(antialised)
            texture->enableBilinearFilter();
    }

    return image;
}

FontPtr UIElementSkin::loadFont(FML::Node* node)
{
    FontPtr font;
    if(FML::Node* cnode = node->at("font"))
        font = g_fonts.get(cnode->value());
    if(!font)
        font = g_uiSkins.getDefaultFont();
    return font;
}


