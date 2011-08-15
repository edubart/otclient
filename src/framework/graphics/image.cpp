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
    std::string source = imageNode->hasValue() ? imageNode->read<std::string>() : imageNode->readAt<std::string>("source");
    bool smooth = imageNode->readAt("smooth", false);
    Rect textureCoords = imageNode->readAt("coords", Rect());

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
