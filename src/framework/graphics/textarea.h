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


#ifndef TEXTAREA_H
#define TEXTAREA_H

#include <prerequisites.h>
#include "font.h"

class TextArea
{
public:
    TextArea();
    TextArea(FontPtr font,
             const std::string& text,
             const Rect& screenCoords,
             AlignmentFlag align = AlignTopLeft,
             const Color& color = Color::white);

    void draw();

    void setFont(FontPtr font);
    void setText(const std::string& text);
    void setScreenCoords(const Rect& screenCoords);
    void setAlign(AlignmentFlag align);
    void setColor(const Color& color) { m_color = color; }
    void setCursorPos(int pos);
    void enableCursor(bool enable = true);
    void setCursorVisible(bool visible = true) { m_cursorVisible = visible; }

    void moveCursor(bool right);
    void appendCharacter(char c);
    void removeCharacter(bool right);

    std::string getText() const { return m_text; }

    FontPtr getFont() const { return m_font; }
    int getTextPos(Point pos);

private:
    void recalculate();

    FontPtr m_font;
    std::string m_text;
    Rect m_screenCoords;
    Rect m_drawArea;
    AlignmentFlag m_align;
    Color m_color;
    int m_cursorPos;
    Point m_startInternalPos;
    int m_startRenderPos;
    int m_cursorTicks;
    bool m_cursorVisible;

    std::vector<Rect> m_glyphsCoords;
    std::vector<Rect> m_glyphsTexCoords;
};

#endif // TEXTAREA_H
