/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
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

#ifndef BITMAPFONT_H
#define BITMAPFONT_H

#include "declarations.h"
#include "texture.h"

#include <framework/otml/declarations.h>
#include <framework/graphics/coordsbuffer.h>

class BitmapFont : public stdext::shared_object
{
public:
    BitmapFont(const std::string& name) : m_name(name) { }

    /// Load font from otml node
    void load(const OTMLNodePtr& fontNode);

    /// Simple text render starting at startPos
    void drawText(const std::string& text, const Point& startPos);

    /// Advanced text render delimited by a screen region and alignment
    void drawText(const std::string& text, const Rect& screenCoords, Fw::AlignmentFlag align = Fw::AlignTopLeft);

    void calculateDrawTextCoords(CoordsBuffer& coordsBuffer, const std::string& text, const Rect& screenCoords, Fw::AlignmentFlag align = Fw::AlignTopLeft);

    /// Calculate glyphs positions to use on render, also calculates textBoxSize if wanted
    const std::vector<Point>& calculateGlyphsPositions(const std::string& text,
                                                       Fw::AlignmentFlag align = Fw::AlignTopLeft,
                                                       Size* textBoxSize = NULL);

    /// Simulate render and calculate text size
    Size calculateTextRectSize(const std::string& text);

    std::string wrapText(const std::string& text, int maxWidth);

    std::string getName() { return m_name; }
    int getGlyphHeight() { return m_glyphHeight; }
    const Rect* getGlyphsTextureCoords() { return m_glyphsTextureCoords; }
    const Size* getGlyphsSize() { return m_glyphsSize; }
    const TexturePtr& getTexture() { return m_texture; }
    int getYOffset() { return m_yOffset; }
    Size getGlyphSpacing() { return m_glyphSpacing; }

private:
    /// Calculates each font character by inspecting font bitmap
    void calculateGlyphsWidthsAutomatically(const ImagePtr& image, const Size& glyphSize);

    std::string m_name;
    int m_glyphHeight;
    int m_firstGlyph;
    int m_yOffset;
    Size m_glyphSpacing;
    TexturePtr m_texture;
    Rect m_glyphsTextureCoords[256];
    Size m_glyphsSize[256];
};


#endif

