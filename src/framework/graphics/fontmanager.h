#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include "font.h"

class FontManager
{
public:
    /// Release fonts references, thus making possible to destruct them
    void releaseFonts();

    /// Import a font from .otfont file
    bool importFont(std::string fontFile);

    bool fontExists(const std::string& fontName);
    FontPtr getFont(const std::string& fontName);
    FontPtr getDefaultFont();

    void setDefaultFont(const std::string& fontName) { m_defaultFont = getFont(fontName); }

private:
    std::vector<FontPtr> m_fonts;
    FontPtr m_defaultFont;
};

extern FontManager g_fonts;

#endif
