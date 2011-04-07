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

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

Font::Font() :
    m_lineHeight(14),
    m_cursorSize(14),
    m_color(Color::white)
{
    bzero(m_glyphWidths, sizeof(m_glyphWidths));
}

bool Font::load(const std::string& file)
{
    if(!g_resources.fileExists(file)) {
        error("Font file %s does not exists", file.c_str());
        return false;
    }

    std::string fileContents = g_resources.loadTextFile(file);
    if(!fileContents.size()) {
        error("Empty font file \"%s",  file.c_str());
        return false;
    }

    std::istringstream fin(fileContents);

    std::string textureName;

    try {
        YAML::Parser parser(fin);

        YAML::Node doc;
        parser.GetNextDocument(doc);

        doc["name"] >> m_name;
        doc["line height"] >> m_lineHeight;
        doc["cursor size"] >> m_cursorSize;
        doc["color"] >> m_color;
        doc["first glyph"] >> m_firstGlyph;
        doc["image"] >> textureName;
        doc["image glyph size"] >> m_glyphSize;

        const YAML::Node& widthsNode = doc["widths"];
        for(auto it = widthsNode.begin(); it != widthsNode.end(); ++it) {
            int id, width;
            it.first() >> id;
            it.second() >> width;
            m_glyphWidths[id] = width;
        }
    } catch (YAML::ParserException& e) {
        error("Malformed font file \"%s\"", file.c_str());
        return false;
    }

    m_texture = g_textures.get("fonts/" + textureName);
    m_numHorizontalGlyphs = m_texture->getSize().width() / m_glyphSize.width();
    m_numVerticalGlyphs = m_texture->getSize().height() / m_glyphSize.height();

    if(!m_texture) {
        error("Failed to load image for font \"%s\"", file.c_str());
        return false;
    }

    return true;
}

void Font::renderText(const Point& pos, const std::string& text)
{
    // bind font texture
    glBindTexture(GL_TEXTURE_2D, m_texture->getTextureId());

    // set font color
    glColor4ubv(m_color.rgbaPtr());

    // begin render
    glBegin(GL_QUADS);

    Point currentPos = pos;
    const Size& screenSize = g_graphics.getScreenSize();
    int textLenght = text.length();

    for(int i = 0; i < textLenght; ++i) {
        int c = (int)text[i];

        // check if is visible
        if(currentPos.x >= screenSize.width())
            continue;
        if(currentPos.y >= screenSize.height())
            break;

        // new line
        if(c == '\n') {
            currentPos.y += m_lineHeight;
            currentPos.x = pos.x;
        }
        // text eof
        else if(c == '\0') {
            break;
        }
        // normal glyph
        else if(c >= 32 && c <= 255) {
            currentPos.x += renderGlyph(currentPos, c);
        }
    }

    // end font render
    glEnd();
}

int Font::renderGlyph(const Point& pos, int glyph)
{
    // get glyph width
    int glyphWidth = m_glyphWidths[glyph];

    // calculate glyph coords on texture font
    const Size& textureSize = m_texture->getSize();
    int glyphTexCoordX = ((glyph - m_firstGlyph) % m_numHorizontalGlyphs) * m_glyphSize.width();
    int glyphTexCoordY = ((glyph - m_firstGlyph) / m_numVerticalGlyphs) * m_glyphSize.height();
    float textureRight = (float)(glyphTexCoordX + glyphWidth) / textureSize.width();
    float textureBottom = (float)(glyphTexCoordY + m_glyphSize.height()) / textureSize.height();
    float textureTop = (float)(glyphTexCoordY) / textureSize.height();
    float textureLeft = (float)(glyphTexCoordX) / textureSize.width();

    // calculate glyph coords on screen
    int right = pos.x + glyphWidth;
    int bottom = pos.y + m_glyphSize.height();
    int top = pos.y;
    int left = pos.x;

    // render glyph
    glTexCoord2f(textureLeft,  textureTop);    glVertex2i(left,  top);
    glTexCoord2f(textureLeft,  textureBottom); glVertex2i(left,  bottom);
    glTexCoord2f(textureRight, textureBottom); glVertex2i(right, bottom);
    glTexCoord2f(textureRight, textureTop);    glVertex2i(right, top);

    return glyphWidth;
}
