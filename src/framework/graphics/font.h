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

#include "prerequisites.h"
#include "texture.h"

enum EAlign {
    ALIGN_TOP = 1 << 0,
    ALIGN_BOTTOM = 1 << 1,
    ALIGN_LEFT = 1 << 2,
    ALIGN_RIGHT = 1 << 3,
    ALIGN_HORIZONTAL_CENTER = 1 << 4,
    ALIGN_VERTICAL_CENTER = 1 << 5,
    ALIGN_CENTER = ALIGN_HORIZONTAL_CENTER | ALIGN_VERTICAL_CENTER,
    ALIGN_TOP_RIGHT = ALIGN_TOP | ALIGN_RIGHT,
    ALIGN_TOP_LEFT = ALIGN_TOP | ALIGN_LEFT,
    ALIGN_BOTTOM_RIGHT = ALIGN_BOTTOM | ALIGN_RIGHT,
    ALIGN_BOTTOM_LEFT = ALIGN_BOTTOM | ALIGN_LEFT
};

class Font
{
public:
    Font(const std::string& name) :
        m_name(name),
        m_glyphHeight(10),
        m_topMargin(0) { }

    /// Load font from file
    bool load(const std::string &file);

    /// Simple text render starting at pos
    void renderText(const std::string& text,
                    const Point& startPos);

    /** Advanced text render
     * screenCoords is the rect that will be filled on the screen
     * startRenderPosition is the postion to start rendering relative to the text rect
     */
    void renderText(const std::string& text,
                    const Rect& screenCoords,
                    int align = ALIGN_TOP_LEFT,
                    const Color& color = Color::white,
                    const Point& startInternalPos = Point(),
                    int cursorPos = -1,
                    const Color& cursorColor = Color::white);


    /// Calculate glyphs positions to use on render, also calculates textBoxSize if wanted
    Point *calculateGlyphsPositions(const std::string& text, int align = ALIGN_TOP_LEFT, Size *textBoxSize = NULL);

    /// Simulate render and calculate text size
    Size calculateTextRectSize(const std::string& text);

    const std::string& getName() const { return m_name; }
    int getGlyphHeight() const { return m_glyphHeight; }
 
private:
    void calculateGlyphsWidthsAutomatically(const Size& glyphSize);

    std::string m_name;
    int m_glyphHeight;
    int m_topMargin;
    Size m_glyphSpacing;
    TexturePtr m_texture;
    Rect m_glyphsTextureCoords[256];
    Size m_glyphsSize[256];
};

typedef std::shared_ptr<Font> FontPtr;

#endif // FONT_H
