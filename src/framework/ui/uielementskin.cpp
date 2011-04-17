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


#include "uielementskin.h"
#include "uielement.h"
#include "graphics/borderedimage.h"
#include "graphics/textures.h"
#include "uiskins.h"

void UIElementSkin::load(const YAML::Node& node)
{
    if(node.FindValue("default size"))
        node["default size"] >> m_defaultSize;
    m_defaultImage = loadImage(node);
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
    if(node.FindValue("bordered image")) {
        const YAML::Node& child = node["bordered image"];
        Rect left, right, top, bottom, topLeft, topRight, bottomLeft, bottomRight, center;
        if(child.FindValue("left border"))
            child["left border"] >> left;
        if(child.FindValue("right border"))
            child["right border"] >> right;
        if(child.FindValue("top border"))
            child["top border"] >> top;
        if(child.FindValue("bottom border"))
            child["bottom border"] >> bottom;
        if(child.FindValue("top left corner"))
            child["top left corner"] >> topLeft;
        if(child.FindValue("top right corner"))
            child["top right corner"] >> topRight;
        if(child.FindValue("bottom left corner"))
            child["bottom left corner"] >> bottomLeft;
        if(child.FindValue("bottom right corner"))
            child["bottom right corner"] >> bottomRight;
        if(child.FindValue("center"))
            child["center"] >> center;

        TexturePtr texture;
        if(child.FindValue("image")) {
            std::string textureName;
            child["image"] >> textureName;
            texture = g_textures.get(textureName);
        } else {
            texture = g_uiSkins.getDefaultTexture();
        }

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
    return image;
}

