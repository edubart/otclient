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

Image::Image()
{
    m_fixedRatio = false;
    m_repeated = false;
}

void Image::loadFromOTML(const OTMLNodePtr& imageNode)
{
    // load configs from otml node
    std::string source = imageNode->hasValue() ? imageNode->value() : imageNode->valueAt("source");
    bool smooth = imageNode->valueAt("smooth", false);
    m_fixedRatio = imageNode->valueAt("fixed ratio", false);
    m_repeated = imageNode->valueAt("repeated", false);

    // load texture
    m_texture = g_textures.getTexture(source);
    m_textureCoords = imageNode->valueAt("coords", Rect(Point(0,0),m_texture->getSize()));

    // enable texture bilinear filter
    if(smooth)
        m_texture->enableBilinearFilter();
}

void Image::draw(const Rect& screenCoords)
{
    if(m_texture) {
        if(m_fixedRatio) {
            const Size& texSize = m_texture->getSize();
            Size texCoordsSize = screenCoords.size();
            texCoordsSize.scale(texSize, Fw::KeepAspectRatio);
            Point texCoordsOffset;
            if(texSize.height() > texCoordsSize.height())
                texCoordsOffset.y = (texSize.height() - texCoordsSize.height())/2;
            else if(texSize.width() > texCoordsSize.width())
                texCoordsOffset.x = (texSize.width() - texCoordsSize.width())/2;

            g_painter.drawTexturedRect(screenCoords, m_texture, Rect(texCoordsOffset, texCoordsSize));
        } else {
            if(m_repeated)
                g_painter.drawRepeatedTexturedRect(screenCoords, m_texture, m_textureCoords);
            else
                g_painter.drawTexturedRect(screenCoords, m_texture, m_textureCoords);
        }
    }
}
