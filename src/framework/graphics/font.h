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


#ifndef FONT_H
#define FONT_H

#include <prerequisites.h>
#include <graphics/texture.h>

class Font
{
public:
    Font(const std::string& name) :
        m_name(name) { }

    /// Load font from file
    bool load(const std::string &file);

    /// Simple text render starting at startPos
    void renderText(const std::string& text,
                    const Point& startPos,
                    const Color& color = Color::white);

    /// Advanced text render
    void renderText(const std::string& text,
                    const Rect& screenCoords,
                    AlignmentFlag align = AlignTopLeft,
                    const Color& color = Color::white);

    /// Calculate glyphs positions to use on render, also calculates textBoxSize if wanted
    const std::vector<Point>& calculateGlyphsPositions(const std::string& text, AlignmentFlag align = AlignTopLeft, Size *textBoxSize = NULL) const;

    /// Simulate render and calculate text size
    Size calculateTextRectSize(const std::string& text);

    std::string getName() const { return m_name; }
    int getGlyphHeight() const { return m_glyphHeight; }
    const Rect *getGlyphsTextureCoords() const { return m_glyphsTextureCoords; }
    const Size *getGlyphsSize() const { return m_glyphsSize; }
    const TexturePtr& getTexture() const { return m_texture; }
    int getTopMargin() const { return m_topMargin; }
    Size getGlyphSpacing() const { return m_glyphSpacing; }

private:
    void calculateGlyphsWidthsAutomatically(const Size& glyphSize);

    std::string m_name;
    int m_glyphHeight;
    int m_firstGlyph;
    int m_topMargin;
    Size m_glyphSpacing;
    TexturePtr m_texture;
    Rect m_glyphsTextureCoords[256];
    Size m_glyphsSize[256];
};

typedef boost::shared_ptr<Font> FontPtr;

#endif // FONT_H
