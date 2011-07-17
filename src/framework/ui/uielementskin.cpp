#include <global.h>
#include <ui/uiskins.h>
#include <ui/uielement.h>
#include <ui/uielementskin.h>
#include <graphics/borderedimage.h>
#include <graphics/textures.h>
#include <graphics/fonts.h>

void UIElementSkin::load(OTMLNode* node)
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

ImagePtr UIElementSkin::loadImage(OTMLNode* node)
{
    ImagePtr image;
    TexturePtr texture;

    if(OTMLNode* cnode = node->at("bordered image")) {
        image = BorderedImage::loadFromOTMLNode(cnode, g_uiSkins.getDefaultTexture());
        if(!image)
            logError(node->generateErrorMessage("failed to load bordered image"));
    } else if(OTMLNode* cnode = node->at("image")) {
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

FontPtr UIElementSkin::loadFont(OTMLNode* node)
{
    FontPtr font;
    if(OTMLNode* cnode = node->at("font"))
        font = g_fonts.get(cnode->value());
    if(!font)
        font = g_uiSkins.getDefaultFont();
    return font;
}


