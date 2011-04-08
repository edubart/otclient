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
#include "../borderedimage.h"
#include "../textures.h"
#include "uiskins.h"

void UIElementSkin::draw(UIElement *element)
{
    const ImagePtr& image = m_stateImages.front();
    if(image) {
        image->draw(element->getRect());
    }
}

void UIElementSkin::load(const YAML::Node& node)
{
    if(node.FindValue("default size"))
        node["default size"] >> m_defaultSize;

    if(node.FindValue("bordered image")) {
        const YAML::Node& child = node["bordered image"];
        Rect left, right, top, bottom, topLeft, topRight, bottomLeft, bottomRight, center;
        child["left border"] >> left;
        child["right border"] >> right;
        child["top border"] >> top;
        child["bottom border"] >> bottom;
        child["top left corner"] >> topLeft;
        child["top right corner"] >> topRight;
        child["bottom left corner"] >> bottomLeft;
        child["bottom right corner"] >> bottomRight;
        child["center"] >> center;

        TexturePtr texture;
        if(child.FindValue("image")) {
            std::string textureName;
            child["image"] >> textureName;
            texture = g_textures.get(textureName);
        } else {
            texture = g_uiSkins.getDefaultTexture();
        }

        ImagePtr image = ImagePtr(new BorderedImage(texture,
                                           left,
                                           right,
                                           top,
                                           bottom,
                                           topLeft,
                                           topRight,
                                           bottomLeft,
                                           bottomRight,
                                           center));
        m_stateImages.push_back(image);
    }
}
