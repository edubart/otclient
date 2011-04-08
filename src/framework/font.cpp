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

void Font::renderText(const std::string& text,
                      const Point& startPos)
{
    Size boxSize = g_graphics.getScreenSize() - startPos.toSize();
    Rect screenCoords(startPos, boxSize);
    Font::renderText(text, screenCoords);
}

void Font::renderText(const std::string& text,
                    const Rect& screenCoords,
                    int align,
                    const Point& startInternalPos,
                    bool debug)
{
    // prevent glitches from invalid rects
    if(!screenCoords.isValid())
        return;

    // begin texture rendering
    g_graphics.setColor(m_color);
    g_graphics._beginTextureRender(m_texture.get());

    const Size& textureSize = m_texture->getSize();
    int textLenght = text.length();

    // map glyphs positions
    Size textBoxSize;
    Point *glyphsPositions = calculateGlyphsPositions(text, align, &textBoxSize);

    for(int i = 0; i < textLenght; ++i) {
        int glyph = (int)text[i];

        // skip invalid glyphs
        if(glyph < 32)
            continue;

        // calculate initial glyph rect and texture coords
        Rect glyphScreenCoords(glyphsPositions[i], m_glyphsSize[glyph]);
        Rect glyphTextureCoords = m_glyphsTextureCoords[glyph];

        // first translate to align position
        if(align & ALIGN_BOTTOM) {
            glyphScreenCoords.translate(0, screenCoords.height() - textBoxSize.height());
        } else if(align & ALIGN_VERTICAL_CENTER) {
            glyphScreenCoords.translate(0, (screenCoords.height() - textBoxSize.height()) / 2);
        } else { // ALIGN_TOP
            // nothing to do
        }

        if(align & ALIGN_RIGHT) {
            glyphScreenCoords.translate(screenCoords.width() - textBoxSize.width(), 0);
        } else if(align & ALIGN_HORIZONTAL_CENTER) {
            glyphScreenCoords.translate((screenCoords.width() - textBoxSize.width()) / 2, 0);
        } else { // ALIGN_TOP
            // nothing to do
        }

        // only render glyphs that are after startRenderPosition
        if(glyphScreenCoords.bottom() < startInternalPos.y || glyphScreenCoords.right() < startInternalPos.x)
            continue;

        // bound glyph topLeft to startRenderPosition
        if(glyphScreenCoords.top() < startInternalPos.y) {
            glyphTextureCoords.setTop(glyphTextureCoords.top() + (startInternalPos.y - glyphScreenCoords.top()));
            glyphScreenCoords.setTop(startInternalPos.y);
        }
        if(glyphScreenCoords.left() < startInternalPos.x) {
            glyphTextureCoords.setLeft(glyphTextureCoords.left() + (startInternalPos.x - glyphScreenCoords.left()));
            glyphScreenCoords.setLeft(startInternalPos.x);
        }

        // subtract startInternalPos
        glyphScreenCoords.translate(-startInternalPos);

        // translate rect to screen coords
        glyphScreenCoords.translate(screenCoords.topLeft());

        // only render if glyph rect is visible on screenCoords
        if(!screenCoords.intersects(glyphScreenCoords))
            continue;

        // bound glyph bottomRight to screenCoords bottomRight
        if(glyphScreenCoords.bottom() > screenCoords.bottom()) {
            glyphTextureCoords.setBottom(glyphTextureCoords.bottom() + (screenCoords.bottom() - glyphScreenCoords.bottom()));
            glyphScreenCoords.setBottom(screenCoords.bottom());
        }
        if(glyphScreenCoords.right() > screenCoords.right()) {
            glyphTextureCoords.setRight(glyphTextureCoords.right() + (screenCoords.right() - glyphScreenCoords.right()));
            glyphScreenCoords.setRight(screenCoords.right());
        }

        // render glyph
        g_graphics._drawTexturedRect(glyphScreenCoords, glyphTextureCoords, textureSize);
    }

    // end texture redering
    g_graphics._endTextureRender();
    g_graphics.resetColor();

    if(debug)
        g_graphics.drawBoundingRect(screenCoords.expanded(1), Color(0xFF00FF00), 1);
}

Point* Font::calculateGlyphsPositions(const std::string& text, int align, Size *textBoxSize)
{
    static Point glyphsPositions[8192];
    static int lineWidths[512];
    int maxLineWidth = 0;
    int lines = 0;
    int glyph;
    int i;

    // protect buffer overflow on glyphsPostions
    int numGlyphs = text.length();
    if(numGlyphs > 8192)
        logFatal("A text was too long to render!");

    // calculate lines width
    if((align & ALIGN_RIGHT || align & ALIGN_HORIZONTAL_CENTER) || textBoxSize) {
        lineWidths[0] = 0;
        for(i = 0; i< numGlyphs; ++i) {
            glyph = (int)text[i];

            if(glyph == (uchar)'\n') {
                lineWidths[++lines] = 0;
            } else if(glyph >= 32) {
                lineWidths[lines] += m_glyphsSize[glyph].width();
                maxLineWidth = std::max(maxLineWidth, lineWidths[lines]);
            }
        }
    }

    Point virtualPos;
    lines = 0;
    for(i = 0; i < numGlyphs; ++i) {
        glyph = (int)text[i];

        // store current glyph topLeft
        glyphsPositions[i] = virtualPos;

        // new line or first glyph
        if(glyph == (uchar)'\n' || i == 0) {
            if(glyph == (uchar)'\n') {
                virtualPos.y += m_lineHeight;
                lines++;
            }

            // calculate start x pos
            if(align & ALIGN_RIGHT) {
                virtualPos.x = (maxLineWidth - lineWidths[lines]);
            } else if(align & ALIGN_HORIZONTAL_CENTER) {
                virtualPos.x = (maxLineWidth - lineWidths[lines]) / 2;
            } else { // ALIGN_LEFT
                virtualPos.x = 0;
            }
        }

        // render only if the glyph is valid
        if(glyph >= 32 && glyph != (uchar)'\n') {
            virtualPos.x += m_glyphsSize[glyph].width();
        }
    }

    if(textBoxSize) {
        textBoxSize->setWidth(maxLineWidth);
        textBoxSize->setHeight(virtualPos.y + m_lineHeight);
    }

    return (Point *)glyphsPositions;
}


Size Font::calculateTextRectSize(const std::string& text)
{
    Size size;
    calculateGlyphsPositions(text, ALIGN_TOP_LEFT, &size);
    return size;
}
