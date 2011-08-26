#include "fontmanager.h"

#include <framework/core/resourcemanager.h>
#include <framework/otml/otml.h>

FontManager g_fonts;

void FontManager::releaseFonts()
{
    m_defaultFont.reset();
    m_fonts.clear();
}

bool FontManager::importFont(std::string fontFile)
{
    try {
        if(!boost::ends_with(fontFile, ".otfont"))
            fontFile += ".otfont";

        OTMLDocumentPtr doc = OTMLDocument::parse(fontFile);
        OTMLNodePtr fontNode = doc->at("Font");

        std::string name = fontNode->valueAt("name");
        if(fontExists(name))
            throw std::runtime_error("a font with the same name is already imported, did you duplicate font names?");

        FontPtr font(new Font(name));
        font->load(fontNode);
        m_fonts.push_back(font);

        // set as default if needed
        if(!m_defaultFont)
            m_defaultFont = font;

        return true;
    } catch(std::exception& e) {
        logError("could not load font from '", fontFile, "': ", e.what());
        return false;
    }
}

bool FontManager::fontExists(const std::string& fontName)
{
    for(const FontPtr& font : m_fonts) {
        if(font->getName() == fontName)
            return true;
    }
    return false;
}

FontPtr FontManager::getFont(const std::string& fontName)
{
    // find font by name
    for(const FontPtr& font : m_fonts) {
        if(font->getName() == fontName)
            return font;
    }

    // when not found, fallback to default font
    return getDefaultFont();
}

