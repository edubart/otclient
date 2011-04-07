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
#include "color.h"
#include "texture.h"
#include "rect.h"

class Font
{
public:
    Font();
    virtual ~Font() { }

    /// Load font from file
    bool load(const std::string &file);

    /// Simple text render starting at pos
    void renderText(const Point& pos, const std::string& text);

    /// Render text delimited by screenCoords rect
    void renderText(const Rect& screenCoords, const std::string& text);

    /** Advanced text render
     * screenCoords is the rect that will be filled on the screen
     * startRenderPosition is the postion to start rendering relative to the text rect
     */
    void renderText(const Rect& screenCoords, const Point& startRenderPosition, const std::string& text);

    /// Simulate render and calculate text size
    Size calculateTextSize(const std::string& text);

    /*
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
    /// Render a text inside a rect
    void renderText(const Rect& screenCoords, EAlign align, const std::string& text);
    */

private:
    int m_lineHeight;
    int m_cursorSize;
    Color m_color;
    TexturePtr m_texture;
    Rect m_glyphsTextureCoords[256];
    Size m_glyphsSize[256];
};

#endif // FONT_H
