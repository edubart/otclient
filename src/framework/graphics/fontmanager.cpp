/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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

#include "fontmanager.h"

#include <framework/core/resourcemanager.h>
#include <framework/otml/otml.h>

FontManager g_fonts;

FontManager::FontManager()
{
    m_defaultFont = FontPtr(new Font("emptyfont"));
}

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

        // remove any font with the same name
        for(auto it = m_fonts.begin(); it != m_fonts.end(); ++it) {
            if((*it)->getName() == name) {
                m_fonts.erase(it);
                break;
            }
        }

        FontPtr font(new Font(name));
        font->load(fontNode);
        m_fonts.push_back(font);

        // set as default if needed
        if(!m_defaultFont)
            m_defaultFont = font;

        return true;
    } catch(stdext::exception& e) {
        logError("Unable to load font from file '%s': %s", fontFile, e.what());
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
    logError("font '%s' not found", fontName);
    return getDefaultFont();
}

