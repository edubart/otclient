/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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
#include "texture.h"

#include <framework/core/resourcemanager.h>
#include <framework/otml/otml.h>

FontManager g_fonts;

FontManager::FontManager()
{
    m_defaultFont = BitmapFontPtr(new BitmapFont("emptyfont"));
}

void FontManager::terminate()
{
    m_fonts.clear();
    m_defaultFont = nullptr;
}

void FontManager::clearFonts()
{
    m_fonts.clear();
    m_defaultFont = BitmapFontPtr(new BitmapFont("emptyfont"));
}

bool FontManager::importFont(std::string file)
{
    try {
        file = g_resources.guessFilePath(file, "otfont");

        OTMLDocumentPtr doc = OTMLDocument::parse(file);
        OTMLNodePtr fontNode = doc->at("Font");

        std::string name = fontNode->valueAt("name");

        // remove any font with the same name
        for(auto it = m_fonts.begin(); it != m_fonts.end(); ++it) {
            if((*it)->getName() == name) {
                m_fonts.erase(it);
                break;
            }
        }

        BitmapFontPtr font(new BitmapFont(name));
        font->load(fontNode);
        m_fonts.push_back(font);

        // set as default if needed
        if(!m_defaultFont || fontNode->valueAt<bool>("default", false))
            m_defaultFont = font;

        return true;
    } catch(stdext::exception& e) {
        g_logger.error(stdext::format("Unable to load font from file '%s': %s", file, e.what()));
        return false;
    }
}

bool FontManager::fontExists(const std::string& fontName)
{
    for(const BitmapFontPtr& font : m_fonts) {
        if(font->getName() == fontName)
            return true;
    }
    return false;
}

BitmapFontPtr FontManager::getFont(const std::string& fontName)
{
    // find font by name
    for(const BitmapFontPtr& font : m_fonts) {
        if(font->getName() == fontName)
            return font;
    }

    // when not found, fallback to default font
    g_logger.error(stdext::format("font '%s' not found", fontName));
    return getDefaultFont();
}
