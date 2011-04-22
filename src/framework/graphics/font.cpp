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
#include <graphics/font.h>
#include <graphics/textures.h>
#include <graphics/graphics.h>

void Font::calculateGlyphsWidthsAutomatically(const Size& glyphSize)
{
    int numHorizontalGlyphs = m_texture->getSize().width() / glyphSize.width();
    uchar *texturePixels = m_texture->getPixels();

    // small AI to auto calculate pixels widths
    for(int glyph = 32; glyph< 256; ++glyph) {
        Rect glyphCoords(((glyph - 32) % numHorizontalGlyphs) * glyphSize.width(),
                            ((glyph - 32) / numHorizontalGlyphs) * glyphSize.height(),
                            glyphSize.width(),
                            m_glyphHeight);
        int width = glyphSize.width();
        for(int x = glyphCoords.left() + 2; x <= glyphCoords.right(); ++x) {
            bool allAlpha = true;

            // check if all vertical pixels are alpha
            for(int y = glyphCoords.top(); y <= glyphCoords.bottom(); ++y) {
                if(texturePixels[(y * m_texture->getSize().width() * 4) + (x*4) + 3] != 0) {
                    allAlpha = false;
                    break;
                }
            }

            // if all pixels were alpha we found the width
            if(allAlpha) {
                width = x - glyphCoords.left();
                break;
            }
        }
        // store glyph size
        m_glyphsSize[glyph].setSize(width, m_glyphHeight);
    }

    delete[] texturePixels;
}

bool Font::load(const std::string& file)
{
    std::string fileContents = g_resources.loadTextFile(file);
    if(!fileContents.size()) {
        flogError("ERROR: Coult not load font file \"%s",  file.c_str());
        return false;
    }

    std::istringstream fin(fileContents);
    std::string textureName;
    Size glyphSize;

    try {
        YAML::Parser parser(fin);

        YAML::Node doc;
        parser.GetNextDocument(doc);

        // required values
        doc["glyph height"] >> m_glyphHeight;
        doc["image glyph size"] >> glyphSize;
        doc["image"] >> textureName;

        // optional values
        if(doc.FindValue("glyph spacing"))
            doc["glyph spacing"] >> m_glyphSpacing;
        if(doc.FindValue("top margin"))
            doc["top margin"] >> m_topMargin;

        // load texture
        m_texture = g_textures.get("fonts/" + textureName);
        if(!m_texture) {
            flogError("ERROR: Failed to load image for font file \"%s\"", file.c_str());
            return false;
        }

        // auto calculate widths
        calculateGlyphsWidthsAutomatically(glyphSize);

        // read custom widths
        if(doc.FindValue("glyph widths")) {
            const YAML::Node& widthsNode = doc["glyph widths"];
            for(auto it = widthsNode.begin(); it != widthsNode.end(); ++it) {
                int glyph, glyphWidth;
                it.first() >> glyph;
                it.second() >> glyphWidth;
                m_glyphsSize[glyph].setWidth(glyphWidth);
            }
        }

        // calculate glyphs texture coords
        int numHorizontalGlyphs = m_texture->getSize().width() / glyphSize.width();
        for(int glyph = 32; glyph< 256; ++glyph) {
            m_glyphsTextureCoords[glyph].setRect(((glyph - 32) % numHorizontalGlyphs) * glyphSize.width(),
                                                 ((glyph - 32) / numHorizontalGlyphs) * glyphSize.height(),
                                                 m_glyphsSize[glyph].width(),
                                                 m_glyphHeight);
        }
    } catch (YAML::Exception& e) {
        flogError("ERROR: Malformed font file \"%s\":\n  %s", file.c_str() % e.what());
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
    renderText(text, screenCoords, ALIGN_TOP_LEFT, color);
}


void Font::renderText(const std::string& text,
                    const Rect& screenCoords,
                    int align,
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

const std::vector<Point>& Font::calculateGlyphsPositions(const std::string& text, int align, Size *textBoxSize) const
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
            textBoxSize->setSize(0,0);
        return glyphsPositions;
    }

    // resize glyphsPositions vector when needed
    if(textLength > (int)glyphsPositions.size())
        glyphsPositions.resize(textLength);

    // calculate lines width
    if((align & ALIGN_RIGHT || align & ALIGN_HORIZONTAL_CENTER) || textBoxSize) {
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
            if(align & ALIGN_RIGHT) {
                virtualPos.x = (maxLineWidth - lineWidths[lines]);
            } else if(align & ALIGN_HORIZONTAL_CENTER) {
                virtualPos.x = (maxLineWidth - lineWidths[lines]) / 2;
            } else { // ALIGN_LEFT
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
    calculateGlyphsPositions(text, ALIGN_TOP_LEFT, &size);
    return size;
}

