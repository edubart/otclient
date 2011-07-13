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


#include <global.h>
#include <core/resources.h>
#include <graphics/font.h>
#include <graphics/textures.h>
#include <graphics/graphics.h>
#include <otml/otml.h>

void Font::calculateGlyphsWidthsAutomatically(const Size& glyphSize)
{
    int numHorizontalGlyphs = m_texture->getSize().width() / glyphSize.width();
    uchar *texturePixels = m_texture->getPixels();

    // small AI to auto calculate pixels widths
    for(int glyph = m_firstGlyph; glyph< 256; ++glyph) {
        Rect glyphCoords(((glyph - m_firstGlyph) % numHorizontalGlyphs) * glyphSize.width(),
                         ((glyph - m_firstGlyph) / numHorizontalGlyphs) * glyphSize.height(),
                            glyphSize.width(),
                            m_glyphHeight);
        int width = glyphSize.width();
        int lastColumnFilledPixels = 0;
        for(int x = glyphCoords.left() + 1; x <= glyphCoords.right(); ++x) {
            int columnFilledPixels = 0;

            // check if all vertical pixels are alpha
            for(int y = glyphCoords.top(); y <= glyphCoords.bottom(); ++y) {
                if(texturePixels[(y * m_texture->getSize().width() * 4) + (x*4) + 3] != 0)
                    columnFilledPixels++;
            }

            // if all pixels were alpha we found the width
            if(columnFilledPixels == 0) {
                width = x - glyphCoords.left();
                if(m_glyphHeight >= 16 && lastColumnFilledPixels >= m_glyphHeight/3)
                    width += 1;
                break;
            }
            lastColumnFilledPixels = columnFilledPixels;
        }
        // store glyph size
        m_glyphsSize[glyph].setSize(width, m_glyphHeight);
    }

    delete[] texturePixels;
}

bool Font::load(const std::string& file)
{
    std::stringstream fin;
    if(!g_resources.loadFile(file, fin)) {
        error("ERROR: Coult not load font file '",file,"'");
        return false;
    }

    std::string textureName;
    Size glyphSize;

    try {
        OTMLParser parser(fin, file);
        OTMLNode* doc = parser.getDocument();

        // required values
        textureName = doc->valueAt("image");
        glyphSize = doc->readAt("image glyph size", Size(16, 16));
        m_glyphHeight = doc->readAt("glyph height", 11);
        m_firstGlyph = doc->readAt("first glyph", 32);
        m_topMargin = doc->readAt("top margin", 0);
        m_glyphSpacing = doc->readAt("glyph spacing", Size(0,0));

        // load texture
        m_texture = g_textures.get(textureName);
        if(!m_texture) {
            error("ERROR: Failed to load image for font file '",file,"'");
            return false;
        }

        // auto calculate widths
        calculateGlyphsWidthsAutomatically(glyphSize);

        // read custom widths
        if(doc->hasNode("glyph widths")) {
            std::map<int, int> glyphWidths;
            doc->readAt("glyph widths", &glyphWidths);
            foreach(const auto& pair, glyphWidths)
                m_glyphsSize[pair.first].setWidth(pair.second);
        }

        // calculate glyphs texture coords
        int numHorizontalGlyphs = m_texture->getSize().width() / glyphSize.width();
        for(int glyph = m_firstGlyph; glyph< 256; ++glyph) {
            m_glyphsTextureCoords[glyph].setRect(((glyph - m_firstGlyph) % numHorizontalGlyphs) * glyphSize.width(),
                                                 ((glyph - m_firstGlyph) / numHorizontalGlyphs) * glyphSize.height(),
                                                 m_glyphsSize[glyph].width(),
                                                 m_glyphHeight);
        }
    } catch(OTMLException e) {
        error("ERROR: Malformed font file \"", file.c_str(), "\":\n  ", e.what());
        return false;
    }

    return true;
}

void Font::renderText(const std::string& text,
                      const Point& startPos,
                      const Color& color)
{
    Size boxSize = g_graphics.getScreenSize() - startPos.toSize();
    Rect screenCoords(startPos, boxSize);
    renderText(text, screenCoords, AlignTopLeft, color);
}


void Font::renderText(const std::string& text,
                    const Rect& screenCoords,
                    AlignmentFlag align,
                    const Color& color)
{
    // prevent glitches from invalid rects
    if(!screenCoords.isValid())
        return;

    int textLenght = text.length();

    // map glyphs positions
    Size textBoxSize;
    const std::vector<Point>& glyphsPositions = calculateGlyphsPositions(text, align, &textBoxSize);

    for(int i = 0; i < textLenght; ++i) {
        int glyph = (uchar)text[i];

        // skip invalid glyphs
        if(glyph < 32)
            continue;

        // calculate initial glyph rect and texture coords
        Rect glyphScreenCoords(glyphsPositions[i], m_glyphsSize[glyph]);
        Rect glyphTextureCoords = m_glyphsTextureCoords[glyph];

        // first translate to align position
        if(align & AlignBottom) {
            glyphScreenCoords.translate(0, screenCoords.height() - textBoxSize.height());
        } else if(align & AlignVerticalCenter) {
            glyphScreenCoords.translate(0, (screenCoords.height() - textBoxSize.height()) / 2);
        } else { // AlignTop
            // nothing to do
        }

        if(align & AlignRight) {
            glyphScreenCoords.translate(screenCoords.width() - textBoxSize.width(), 0);
        } else if(align & AlignHorizontalCenter) {
            glyphScreenCoords.translate((screenCoords.width() - textBoxSize.width()) / 2, 0);
        } else { // AlignLeft
            // nothing to do
        }

        // only render glyphs that are after 0, 0
        if(glyphScreenCoords.bottom() < 0 || glyphScreenCoords.right() < 0)
            continue;

        // bound glyph topLeft to 0,0 if needed
        if(glyphScreenCoords.top() < 0) {
            glyphTextureCoords.setTop(glyphTextureCoords.top() - glyphScreenCoords.top());
            glyphScreenCoords.setTop(0);
        }
        if(glyphScreenCoords.left() < 0) {
            glyphTextureCoords.setLeft(glyphTextureCoords.left() - glyphScreenCoords.left());
            glyphScreenCoords.setLeft(0);
        }

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
        g_graphics.drawTexturedRect(glyphScreenCoords, m_texture, glyphTextureCoords, color);
    }
}

const std::vector<Point>& Font::calculateGlyphsPositions(const std::string& text, AlignmentFlag align, Size *textBoxSize) const
{
    // for performance reasons we use statics vectors that are allocated on demand
    static std::vector<Point> glyphsPositions(1);
    static std::vector<int> lineWidths(1);

    int textLength = text.length();
    int maxLineWidth = 0;
    int lines = 0;
    int glyph;
    int i;

    // return if there is no text
    if(textLength == 0) {
        if(textBoxSize)
            textBoxSize->setSize(0,m_glyphHeight);
        return glyphsPositions;
    }

    // resize glyphsPositions vector when needed
    if(textLength > (int)glyphsPositions.size())
        glyphsPositions.resize(textLength);

    // calculate lines width
    if((align & AlignRight || align & AlignHorizontalCenter) || textBoxSize) {
        lineWidths[0] = 0;
        for(i = 0; i< textLength; ++i) {
            glyph = (uchar)text[i];

            if(glyph == (uchar)'\n') {
                lines++;
                if(lines+1 > (int)lineWidths.size())
                    lineWidths.resize(lines+1);
                lineWidths[lines] = 0;
            } else if(glyph >= 32) {
                lineWidths[lines] += m_glyphsSize[glyph].width() + m_glyphSpacing.width();
                maxLineWidth = std::max(maxLineWidth, lineWidths[lines]);
            }
        }
    }

    Point virtualPos(0, m_topMargin);
    lines = 0;
    for(i = 0; i < textLength; ++i) {
        glyph = (uchar)text[i];

        // new line or first glyph
        if(glyph == (uchar)'\n' || i == 0) {
            if(glyph == (uchar)'\n') {
                virtualPos.y += m_glyphHeight + m_glyphSpacing.height();
                lines++;
            }

            // calculate start x pos
            if(align & AlignRight) {
                virtualPos.x = (maxLineWidth - lineWidths[lines]);
            } else if(align & AlignHorizontalCenter) {
                virtualPos.x = (maxLineWidth - lineWidths[lines]) / 2;
            } else { // AlignLeft
                virtualPos.x = 0;
            }
        }

        // store current glyph topLeft
        glyphsPositions[i] = virtualPos;

        // render only if the glyph is valid
        if(glyph >= 32 && glyph != (uchar)'\n') {
            virtualPos.x += m_glyphsSize[glyph].width() + m_glyphSpacing.width();
        }
    }

    if(textBoxSize) {
        textBoxSize->setWidth(maxLineWidth);
        textBoxSize->setHeight(virtualPos.y + m_glyphHeight);
    }

    return glyphsPositions;
}

Size Font::calculateTextRectSize(const std::string& text)
{
    Size size;
    calculateGlyphsPositions(text, AlignTopLeft, &size);
    return size;
}

