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
#include <core/engine.h>
#include <graphics/textarea.h>
#include <graphics/graphics.h>

TextArea::TextArea() :
        m_align(AlignLeftCenter),
        m_color(Color::white),
        m_cursorPos(-1),
        m_startRenderPos(0),
        m_cursorVisible(false)
{
}

TextArea::TextArea(FontPtr font,
                   const std::string& text,
                   const Rect& screenCoords,
                   AlignmentFlag align,
                   const Color& color) :
        m_font(font),
        m_text(text),
        m_screenCoords(screenCoords),
        m_align(align),
        m_color(color),
        m_cursorPos(-1),
        m_startRenderPos(0),
        m_cursorVisible(false)
{
    recalculate();
}

void TextArea::draw()
{
    int textLength = m_text.length();
    const TexturePtr& texture = m_font->getTexture();
    for(int i=0;i<textLength;++i) {
        g_graphics.drawTexturedRect(m_glyphsCoords[i], texture, m_glyphsTexCoords[i], m_color);
    }

    // render cursor
    if(m_cursorVisible && m_cursorPos >= 0) {
        assert(m_cursorPos <= textLength);
        const int delay = 500;
        int ticks = g_engine.getCurrentFrameTicks();
        // draw every 500ms
        if(ticks - m_cursorTicks <= delay) {
            Rect cursorRect;
            // when cursor is at 0 or is the first visible element
            if(m_cursorPos == 0 || m_cursorPos == m_startRenderPos)
                cursorRect = Rect(m_drawArea.left()-1, m_drawArea.top(), 1, m_font->getGlyphHeight());
            else
                cursorRect = Rect(m_glyphsCoords[m_cursorPos-1].right(), m_glyphsCoords[m_cursorPos-1].top(), 1, m_font->getGlyphHeight());
            g_graphics.drawFilledRect(cursorRect, m_color);
        } else if(ticks - m_cursorTicks >= 2*delay) {
            m_cursorTicks = g_engine.getCurrentFrameTicks();
        }
    }
}

void TextArea::recalculate()
{
    int textLength = m_text.length();

    // prevent glitches
    if(!m_screenCoords.isValid() || !m_font)
        return;

    // map glyphs positions
    Size textBoxSize;
    const std::vector<Point>& glyphsPositions = m_font->calculateGlyphsPositions(m_text, m_align, &textBoxSize);
    const Rect *glyphsTextureCoords = m_font->getGlyphsTextureCoords();
    const Size *glyphsSize = m_font->getGlyphsSize();
    int glyph;

    // resize just on demand
    if(textLength > (int)m_glyphsCoords.size()) {
        m_glyphsCoords.resize(textLength);
        m_glyphsTexCoords.resize(textLength);
    }

    // readjust start view area based on cursor position
    if(m_cursorPos >= 0 && textLength > 0) {
        assert(m_cursorPos <= textLength);
        if(m_cursorPos < m_startRenderPos) // cursor is before the previuos first rendered glyph, so we need to update
        {
            m_startInternalPos.x = glyphsPositions[m_cursorPos].x;
            m_startInternalPos.y = glyphsPositions[m_cursorPos].y - m_font->getTopMargin();
            m_startRenderPos = m_cursorPos;
        } else if(m_cursorPos > m_startRenderPos || // cursor is after the previuos first rendered glyph
                  (m_cursorPos == m_startRenderPos && textLength == m_cursorPos)) // cursor is at the previuos rendered element, and is the last text element
        {
            Rect virtualRect(m_startInternalPos, m_screenCoords.size()); // previus rendered virtual rect
            int pos = m_cursorPos - 1; // element before cursor
            glyph = (uchar)m_text[pos]; // glyph of the element before cursor
            Rect glyphRect(glyphsPositions[pos], glyphsSize[glyph]);

            // if the cursor is not on the previus rendered virtual rect we need to update it
            if(!virtualRect.contains(glyphRect.topLeft()) || !virtualRect.contains(glyphRect.bottomRight())) {
                // calculate where is the first glyph visible
                Point startGlyphPos;
                startGlyphPos.y = std::max(glyphRect.bottom() - virtualRect.height(), 0);
                startGlyphPos.x = std::max(glyphRect.right() - virtualRect.width(), 0);

                // find that glyph
                for(pos = 0; pos < textLength; ++pos) {
                    glyph = (uchar)m_text[pos];
                    glyphRect = Rect(glyphsPositions[pos], glyphsSize[glyph]);
                    glyphRect.setTop(std::max(glyphRect.top() - m_font->getTopMargin() - m_font->getGlyphSpacing().height(), 0));
                    glyphRect.setLeft(std::max(glyphRect.left() - m_font->getGlyphSpacing().width(), 0));

                    // first glyph entirely visible found
                    if(glyphRect.topLeft() >= startGlyphPos) {
                        m_startInternalPos.x = glyphsPositions[pos].x;
                        m_startInternalPos.y = glyphsPositions[pos].y - m_font->getTopMargin();
                        m_startRenderPos = pos;
                        break;
                    }
                }
            }
        }
    } else {
        m_startInternalPos = Point(0,0);
    }

    m_drawArea = m_screenCoords;

    if(m_align & AlignBottom) {
        m_drawArea.translate(0, m_screenCoords.height() - textBoxSize.height());
    } else if(m_align & AlignVerticalCenter) {
        m_drawArea.translate(0, (m_screenCoords.height() - textBoxSize.height()) / 2);
    } else { // AlignTop
    }

    if(m_align & AlignRight) {
        m_drawArea.translate(m_screenCoords.width() - textBoxSize.width(), 0);
    } else if(m_align & AlignHorizontalCenter) {
        m_drawArea.translate((m_screenCoords.width() - textBoxSize.width()) / 2, 0);
    } else { // AlignLeft

    }

    for(int i = 0; i < textLength; ++i) {
        glyph = (uchar)m_text[i];
        m_glyphsCoords[i].clear();

        // skip invalid glyphs
        if(glyph < 32)
            continue;

        // calculate initial glyph rect and texture coords
        Rect glyphScreenCoords(glyphsPositions[i], glyphsSize[glyph]);
        Rect glyphTextureCoords = glyphsTextureCoords[glyph];

        // first translate to align position
        if(m_align & AlignBottom) {
            glyphScreenCoords.translate(0, m_screenCoords.height() - textBoxSize.height());
        } else if(m_align & AlignVerticalCenter) {
            glyphScreenCoords.translate(0, (m_screenCoords.height() - textBoxSize.height()) / 2);
        } else { // AlignTop
            // nothing to do
        }

        if(m_align & AlignRight) {
            glyphScreenCoords.translate(m_screenCoords.width() - textBoxSize.width(), 0);
        } else if(m_align & AlignHorizontalCenter) {
            glyphScreenCoords.translate((m_screenCoords.width() - textBoxSize.width()) / 2, 0);
        } else { // AlignLeft
            // nothing to do
        }

        // only render glyphs that are after startRenderPosition
        if(glyphScreenCoords.bottom() < m_startInternalPos.y || glyphScreenCoords.right() < m_startInternalPos.x)
            continue;

        // bound glyph topLeft to startRenderPosition
        if(glyphScreenCoords.top() < m_startInternalPos.y) {
            glyphTextureCoords.setTop(glyphTextureCoords.top() + (m_startInternalPos.y - glyphScreenCoords.top()));
            glyphScreenCoords.setTop(m_startInternalPos.y);
        }
        if(glyphScreenCoords.left() < m_startInternalPos.x) {
            glyphTextureCoords.setLeft(glyphTextureCoords.left() + (m_startInternalPos.x - glyphScreenCoords.left()));
            glyphScreenCoords.setLeft(m_startInternalPos.x);
        }

        // subtract startInternalPos
        glyphScreenCoords.translate(-m_startInternalPos);

        // translate rect to screen coords
        glyphScreenCoords.translate(m_screenCoords.topLeft());

        // only render if glyph rect is visible on screenCoords
        if(!m_screenCoords.intersects(glyphScreenCoords))
            continue;

        // bound glyph bottomRight to screenCoords bottomRight
        if(glyphScreenCoords.bottom() > m_screenCoords.bottom()) {
            glyphTextureCoords.setBottom(glyphTextureCoords.bottom() + (m_screenCoords.bottom() - glyphScreenCoords.bottom()));
            glyphScreenCoords.setBottom(m_screenCoords.bottom());
        }
        if(glyphScreenCoords.right() > m_screenCoords.right()) {
            glyphTextureCoords.setRight(glyphTextureCoords.right() + (m_screenCoords.right() - glyphScreenCoords.right()));
            glyphScreenCoords.setRight(m_screenCoords.right());
        }

        // render glyph
        m_glyphsCoords[i] = glyphScreenCoords;
        m_glyphsTexCoords[i] = glyphTextureCoords;
    }
}

void TextArea::setFont(FontPtr font)
{
    if(m_font != font) {
        m_font = font;
        recalculate();
    }
}

void TextArea::setText(const std::string& text)
{
    if(m_text != text) {
        m_text = text;
        if(m_cursorPos >= 0) {
            m_cursorPos = 0;
            m_cursorTicks = g_engine.getCurrentFrameTicks();
        }
        recalculate();
    }
}

void TextArea::setScreenCoords(const Rect& screenCoords)
{
    if(screenCoords != m_screenCoords) {
        m_screenCoords = screenCoords;
        recalculate();
    }
}

void TextArea::setAlign(AlignmentFlag align)
{
    if(m_align != align) {
        m_align = align;
        recalculate();
    }
}

void TextArea::setCursorPos(int pos)
{
    if(pos != m_cursorPos) {
        if(pos < 0)
            m_cursorPos = 0;
        else if((uint)pos >= m_text.length())
            m_cursorPos = m_text.length();
        else
            m_cursorPos = pos;
        recalculate();
    }
}

void TextArea::enableCursor(bool enable)
{
    if(enable) {
        m_cursorPos = 0;
        m_cursorTicks = g_engine.getCurrentFrameTicks();
    } else
        m_cursorPos = -1;
    recalculate();
}

void TextArea::appendCharacter(char c)
{
    if(m_cursorPos >= 0) {
        std::string tmp;
        tmp = c;
        m_text.insert(m_cursorPos, tmp);
        m_cursorPos++;
        m_cursorTicks = g_engine.getCurrentFrameTicks();
        recalculate();
    }
}

void TextArea::removeCharacter(bool right)
{
    if(m_cursorPos >= 0 && m_text.length() > 0) {
        if(right && (uint)m_cursorPos < m_text.length())
            m_text.erase(m_text.begin() + m_cursorPos);
        else if((uint)m_cursorPos == m_text.length()) {
            m_text.erase(m_text.begin() + (--m_cursorPos));
            m_cursorTicks = g_engine.getCurrentFrameTicks();
        }
        recalculate();
    }
}

void TextArea::moveCursor(bool right)
{
    if(right) {
        if((uint)m_cursorPos+1 <= m_text.length()) {
            m_cursorPos++;
            m_cursorTicks = g_engine.getCurrentFrameTicks();
        }
    } else {
        if(m_cursorPos-1 >= 0) {
            m_cursorPos--;
            m_cursorTicks = g_engine.getCurrentFrameTicks();
        }
    }
    recalculate();
}

int TextArea::getTextPos(Point pos)
{
    int textLength = m_text.length();

    // find any glyph that is actually on the
    int candidatePos = -1;
    for(int i=0;i<textLength;++i) {
        Rect clickGlyphRect = m_glyphsCoords[i];
        clickGlyphRect.addTop(m_font->getTopMargin() + m_font->getGlyphSpacing().height());
        clickGlyphRect.addLeft(m_font->getGlyphSpacing().width()+1);
        if(clickGlyphRect.contains(pos))
            return i;
        else if(pos.y >= clickGlyphRect.top() && pos.y <= clickGlyphRect.bottom()) {
            if(pos.x <= clickGlyphRect.left())
                candidatePos = i;
            else if(pos.x >= clickGlyphRect.right())
                candidatePos = i+1;
        }
    }
    return candidatePos;
}
