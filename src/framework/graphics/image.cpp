/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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

#include "image.h"
#include "texture.h"
#include "graphics.h"
#include "texturemanager.h"

#include <framework/otml/otml.h>

Image::Image(TexturePtr texture, Rect textureCoords)
{
    m_texture = texture;
    if(!textureCoords.isValid())
        m_textureCoords = Rect(0, 0, m_texture->getSize());
    else
        m_textureCoords = textureCoords;
}

ImagePtr Image::loadFromOTML(const OTMLNodePtr& imageNode)
{
    // load configs from otml node
    std::string source = imageNode->hasValue() ? imageNode->value() : imageNode->valueAt("source");
    bool smooth = imageNode->valueAt("smooth", false);
    Rect textureCoords = imageNode->valueAt("coords", Rect());

    // load texture
    TexturePtr texture = g_textures.getTexture(source);
    if(!texture)
        throw OTMLException(imageNode, "could not load image texture");

    // enable texture bilinear filter
    if(smooth)
        texture->enableBilinearFilter();

    // create image
    return ImagePtr(new Image(texture, textureCoords));
}

void Image::draw(const Rect& screenCoords)
{
    if(m_texture)
        g_graphics.drawTexturedRect(screenCoords, m_texture, m_textureCoords);
}
