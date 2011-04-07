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


#include "font.h"
#include "resources.h"
#include "textures.h"
#include "graphics.h"

Font::Font() :
    m_lineHeight(14),
    m_cursorSize(14),
    m_color(0xFFFFFFFF)
{
}

bool Font::load(const std::string& file)
{
    if(!g_resources.fileExists(file)) {
        logError("Font file %s does not exists", file.c_str());
        return false;
    }

    std::string fileContents = g_resources.loadTextFile(file);
    if(!fileContents.size()) {
        logError("Empty font file \"%s",  file.c_str());
        return false;
    }

    std::istringstream fin(fileContents);

    std::string textureName;
    int numHorizontalGlyphs;
    int firstGlyph;
    Size glyphSize;
    Size textureSize;

    try {
        YAML::Parser parser(fin);

        YAML::Node doc;
        parser.GetNextDocument(doc);

        doc["line height"] >> m_lineHeight;
        doc["cursor size"] >> m_cursorSize;
        doc["color"] >> m_color;
        doc["first glyph"] >> firstGlyph;
        doc["image glyph size"] >> glyphSize;
        doc["image"] >> textureName;

        m_texture = g_textures.get("fonts/" + textureName);
        if(!m_texture) {
            logError("Failed to load image for font \"%s\"", file.c_str());
            return false;
        }

        textureSize = m_texture->getSize();
        numHorizontalGlyphs = textureSize.width() / glyphSize.width();

        const YAML::Node& widthsNode = doc["glyph widths"];
        for(auto it = widthsNode.begin(); it != widthsNode.end(); ++it) {
            int glyph, glyphWidth;
            it.first() >> glyph;
            it.second() >> glyphWidth;

            // calculate glyph texture coords
            m_glyphsTextureCoords[glyph].setRect(((glyph - firstGlyph) % numHorizontalGlyphs) * glyphSize.width(),
                                                 ((glyph - firstGlyph) / numHorizontalGlyphs) * glyphSize.height(),
                                                 glyphWidth,
                                                 glyphSize.height());

            // store glyph size
            m_glyphsSize[glyph].setHeight(glyphSize.height());
            m_glyphsSize[glyph].setWidth(glyphWidth);
        }
    } catch (YAML::ParserException& e) {
        logError("Malformed font file \"%s\"", file.c_str());
        return false;
    }

    return true;
}

void Font::renderText(const Point& pos, const std::string& text)
{
    // begin texture rendering
    g_graphics.setColor(m_color);
    g_graphics._beginTextureRender(m_texture.get());

    Point currentPos = pos;
    const Size& screenSize = g_graphics.getScreenSize();
    const Size& textureSize = m_texture->getSize();
    int textLenght = text.length();

    for(int i = 0; i < textLenght; ++i) {
        int glyph = (int)text[i];

        // break rendering if the Y pos is below the screen
        if(currentPos.y >= screenSize.height())
            break;

        // new line
        if(glyph == (uchar)'\n') {
            currentPos.y += m_lineHeight;
            currentPos.x = pos.x;
        }
        // render only if the glyph is valid and visible
        else if(glyph >= 32 && currentPos.x < screenSize.width()) {
            g_graphics._drawTexturedRect(Rect(currentPos, m_glyphsSize[glyph]),
                                            m_glyphsTextureCoords[glyph],
                                            textureSize);
            currentPos.x += m_glyphsSize[glyph].width();
        }
    }

    // end texture redering
    g_graphics._endTextureRender();
    g_graphics.resetColor();
}

Size Font::calculateTextSize(const std::string& text)
{
    int textLenght = text.length();
    Size size;
    Point currentPos;

    for(int i = 0; i < textLenght; ++i) {
        int glyph = (int)text[i];

        if(glyph == (uchar)'\n') {
            currentPos.y += m_lineHeight;
            size.expandedTo(currentPos.toSize());
            currentPos.x = 0;
        }
        else if(glyph > 32) {
            currentPos.x += m_glyphsSize[glyph].width();
        }
    }
    return size;
}
