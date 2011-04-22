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


#include <prerequisites.h>
#include <core/resources.h>
#include <graphics/fonts.h>

Fonts g_fonts;

void Fonts::init(const std::string& defaultFontName)
{
    // load all fonts
    std::list<std::string> files = g_resources.getDirectoryFiles("fonts");
    foreach(const std::string& file, files) {
        if(boost::ends_with(file, ".yml")) {
            std::string name = file;
            boost::erase_first(name, ".yml");
            FontPtr font(new Font(name));
            if(font->load("fonts/" + file)) {
                m_fonts.push_back(font);

                if(name == defaultFontName)
                    m_defaultFont = font;
            }
        }
    }

    if(!m_defaultFont)
        flogFatal("FATAL ERROR: Could not load the default font \"%s\"\n", defaultFontName.c_str());
}

Font* Fonts::get(const std::string& fontName)
{
    // find font by name
    for(auto it = m_fonts.begin(); it != m_fonts.end(); ++it) {
        if((*it)->getName() == fontName)
            return (*it).get();
    }

    flogError("ERROR: Font \"%s\" not found, returing the default one", fontName.c_str());
    return m_defaultFont.get();
}
