/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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

#include "uilineedit.h"
#include <framework/graphics/font.h>
#include <framework/graphics/graphics.h>
#include <framework/platform/platformwindow.h>
#include <framework/core/clock.h>
#include <framework/otml/otmlnode.h>

UILineEdit::UILineEdit()
{
    m_cursorPos = 0;
    m_textAlign = Fw::AlignLeftCenter;
    m_startRenderPos = 0;
    m_textHorizontalMargin = 0;
    m_textHidden = false;
    m_alwaysActive = false;
    blinkCursor();
}

void UILineEdit::drawSelf()
{
    drawBackground(m_rect);
    drawBorder(m_rect);
    drawImage(m_rect);
    drawIcon(m_rect);

    //TODO: text rendering could be much optimized by using vertex buffer or caching the render into a texture

    int textLength = m_text.length();
    const TexturePtr& texture = m_font->getTexture();

    g_painter.setColor(m_color);
    for(int i=0;i<textLength;++i)
        g_painter.drawTexturedRect(m_glyphsCoords[i], texture, m_glyphsTexCoords[i]);

    // render cursor
    if(isExplicitlyEnabled() && (isActive() || m_alwaysActive) && m_cursorPos >= 0) {
        assert(m_cursorPos <= textLength);
        // draw every 333ms
        const int delay = 333;
        if(g_clock.ticksElapsed(m_cursorTicks) <= delay) {
            Rect cursorRect;
            // when cursor is at 0 or is the first visible element
            if(m_cursorPos == 0 || m_cursorPos == m_startRenderPos)
                cursorRect = Rect(m_drawArea.left()-1, m_drawArea.top(), 1, m_font->getGlyphHeight());
            else
                cursorRect = Rect(m_glyphsCoords[m_cursorPos-1].right(), m_glyphsCoords[m_cursorPos-1].top(), 1, m_font->getGlyphHeight());
            g_painter.drawFilledRect(cursorRect);
        } else if(g_clock.ticksElapsed(m_cursorTicks) >= 2*delay) {
            m_cursorTicks = g_clock.ticks();
        }
    }
}

void UILineEdit::update()
{
    std::string text = getDisplayedText();
    int textLength = text.length();

    // prevent glitches
    if(m_rect.isEmpty())
        return;

    // map glyphs positions
    Size textBoxSize;
    const std::vector<Point>& glyphsPositions = m_font->calculateGlyphsPositions(text, m_textAlign, &textBoxSize);
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
            m_startInternalPos.y = glyphsPositions[m_cursorPos].y - m_font->getYOffset();
            m_startRenderPos = m_cursorPos;
        } else if(m_cursorPos > m_startRenderPos || // cursor is after the previuos first rendered glyph
                  (m_cursorPos == m_startRenderPos && textLength == m_cursorPos)) // cursor is at the previuos rendered element, and is the last text element
        {
            Rect virtualRect(m_startInternalPos, m_rect.size() - Size(2*m_textHorizontalMargin, 0) ); // previous rendered virtual rect
            int pos = m_cursorPos - 1; // element before cursor
            glyph = (uchar)text[pos]; // glyph of the element before cursor
            Rect glyphRect(glyphsPositions[pos], glyphsSize[glyph]);

            // if the cursor is not on the previous rendered virtual rect we need to update it
            if(!virtualRect.contains(glyphRect.topLeft()) || !virtualRect.contains(glyphRect.bottomRight())) {
                // calculate where is the first glyph visible
                Point startGlyphPos;
                startGlyphPos.y = std::max(glyphRect.bottom() - virtualRect.height(), 0);
                startGlyphPos.x = std::max(glyphRect.right() - virtualRect.width(), 0);

                // find that glyph
                for(pos = 0; pos < textLength; ++pos) {
                    glyph = (uchar)text[pos];
                    glyphRect = Rect(glyphsPositions[pos], glyphsSize[glyph]);
                    glyphRect.setTop(std::max(glyphRect.top() - m_font->getYOffset() - m_font->getGlyphSpacing().height(), 0));
                    glyphRect.setLeft(std::max(glyphRect.left() - m_font->getGlyphSpacing().width(), 0));

                    // first glyph entirely visible found
                    if(glyphRect.topLeft() >= startGlyphPos) {
                        m_startInternalPos.x = glyphsPositions[pos].x;
                        m_startInternalPos.y = glyphsPositions[pos].y - m_font->getYOffset();
                        m_startRenderPos = pos;
                        break;
                    }
                }
            }
        }
    } else {
        m_startInternalPos = Point(0,0);
    }

    Rect textScreenCoords = m_rect;
    textScreenCoords.expandLeft(-m_textHorizontalMargin);
    textScreenCoords.expandRight(-m_textHorizontalMargin);
    m_drawArea = textScreenCoords;

    if(m_textAlign & Fw::AlignBottom) {
        m_drawArea.translate(0, textScreenCoords.height() - textBoxSize.height());
    } else if(m_textAlign & Fw::AlignVerticalCenter) {
        m_drawArea.translate(0, (textScreenCoords.height() - textBoxSize.height()) / 2);
    } else { // AlignTop
    }

    if(m_textAlign & Fw::AlignRight) {
        m_drawArea.translate(textScreenCoords.width() - textBoxSize.width(), 0);
    } else if(m_textAlign & Fw::AlignHorizontalCenter) {
        m_drawArea.translate((textScreenCoords.width() - textBoxSize.width()) / 2, 0);
    } else { // AlignLeft

    }

    for(int i = 0; i < textLength; ++i) {
        glyph = (uchar)text[i];
        m_glyphsCoords[i].clear();

        // skip invalid glyphs
        if(glyph < 32)
            continue;

        // calculate initial glyph rect and texture coords
        Rect glyphScreenCoords(glyphsPositions[i], glyphsSize[glyph]);
        Rect glyphTextureCoords = glyphsTextureCoords[glyph];

        // first translate to align position
        if(m_textAlign & Fw::AlignBottom) {
            glyphScreenCoords.translate(0, textScreenCoords.height() - textBoxSize.height());
        } else if(m_textAlign & Fw::AlignVerticalCenter) {
            glyphScreenCoords.translate(0, (textScreenCoords.height() - textBoxSize.height()) / 2);
        } else { // AlignTop
            // nothing to do
        }

        if(m_textAlign & Fw::AlignRight) {
            glyphScreenCoords.translate(textScreenCoords.width() - textBoxSize.width(), 0);
        } else if(m_textAlign & Fw::AlignHorizontalCenter) {
            glyphScreenCoords.translate((textScreenCoords.width() - textBoxSize.width()) / 2, 0);
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
        glyphScreenCoords.translate(textScreenCoords.topLeft());

        // only render if glyph rect is visible on screenCoords
        if(!textScreenCoords.intersects(glyphScreenCoords))
            continue;

        // bound glyph bottomRight to screenCoords bottomRight
        if(glyphScreenCoords.bottom() > textScreenCoords.bottom()) {
            glyphTextureCoords.setBottom(glyphTextureCoords.bottom() + (textScreenCoords.bottom() - glyphScreenCoords.bottom()));
            glyphScreenCoords.setBottom(textScreenCoords.bottom());
        }
        if(glyphScreenCoords.right() > textScreenCoords.right()) {
            glyphTextureCoords.setRight(glyphTextureCoords.right() + (textScreenCoords.right() - glyphScreenCoords.right()));
            glyphScreenCoords.setRight(textScreenCoords.right());
        }

        // render glyph
        m_glyphsCoords[i] = glyphScreenCoords;
        m_glyphsTexCoords[i] = glyphTextureCoords;
    }
}

void UILineEdit::setTextHorizontalMargin(int margin)
{
    m_textHorizontalMargin = margin;
    update();
}

void UILineEdit::setCursorPos(int pos)
{
    if(pos != m_cursorPos) {
        if(pos < 0)
            m_cursorPos = 0;
        else if((uint)pos >= m_text.length())
            m_cursorPos = m_text.length();
        else
            m_cursorPos = pos;
        update();
    }
}

void UILineEdit::setCursorEnabled(bool enable)
{
    if(enable) {
        m_cursorPos = 0;
            blinkCursor();
    } else
        m_cursorPos = -1;
    update();
}

void UILineEdit::setTextHidden(bool hidden)
{
    m_textHidden = true;
    update();
}

void UILineEdit::setAlwaysActive(bool enable)
{
    m_alwaysActive = enable;
}

void UILineEdit::appendText(std::string text)
{
    if(m_cursorPos >= 0) {
        // replace characters that are now allowed
        boost::replace_all(text, "\n", "");
        boost::replace_all(text, "\r", "    ");

        if(text.length() > 0) {
            m_text.insert(m_cursorPos, text);
            m_cursorPos += text.length();
            blinkCursor();
            update();
            UIWidget::onTextChange(m_text);
        }
    }
}

void UILineEdit::appendCharacter(char c)
{
    if(c == '\n' || c == '\r')
        return;

    if(m_cursorPos >= 0) {
        std::string tmp;
        tmp = c;
        m_text.insert(m_cursorPos, tmp);
        m_cursorPos++;
        blinkCursor();
        update();
        UIWidget::onTextChange(m_text);
    }
}

void UILineEdit::removeCharacter(bool right)
{
    if(m_cursorPos >= 0 && m_text.length() > 0) {
        if((uint)m_cursorPos >= m_text.length()) {
            m_text.erase(m_text.begin() + (--m_cursorPos));
        } else {
            if(right)
                m_text.erase(m_text.begin() + m_cursorPos);
            else if(m_cursorPos > 0)
                m_text.erase(m_text.begin() + --m_cursorPos);
        }
        blinkCursor();
        update();
        UIWidget::onTextChange(m_text);
    }
}

void UILineEdit::moveCursor(bool right)
{
    if(right) {
        if((uint)m_cursorPos+1 <= m_text.length()) {
            m_cursorPos++;
            blinkCursor();
        }
    } else {
        if(m_cursorPos-1 >= 0) {
            m_cursorPos--;
            blinkCursor();
        }
    }
    update();
}

int UILineEdit::getTextPos(Point pos)
{
    int textLength = m_text.length();

    // find any glyph that is actually on the
    int candidatePos = -1;
    for(int i=0;i<textLength;++i) {
        Rect clickGlyphRect = m_glyphsCoords[i];
        clickGlyphRect.expandTop(m_font->getYOffset() + m_font->getGlyphSpacing().height());
        clickGlyphRect.expandLeft(m_font->getGlyphSpacing().width()+1);
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

std::string UILineEdit::getDisplayedText()
{
    if(m_textHidden)
        return std::string(m_text.length(), '*');
    else
        return m_text;
}

void UILineEdit::onTextChange(const std::string& text)
{
    m_cursorPos = text.length();
    blinkCursor();
    update();
    UIWidget::onTextChange(text);
}

void UILineEdit::onFontChange(const std::string& font)
{
    update();
    UIWidget::onFontChange(font);
}

void UILineEdit::onStyleApply(const std::string& styleName, const OTMLNodePtr& styleNode)
{
    UIWidget::onStyleApply(styleName, styleNode);

    for(const OTMLNodePtr& node : styleNode->children()) {
        if(node->tag() == "text") {
            setText(node->value());
            setCursorPos(m_text.length());
        } else if(node->tag() == "text-hidden")
            setTextHidden(node->value<bool>());
        else if(node->tag() == "text-margin")
            setTextHorizontalMargin(node->value<int>());
        else if(node->tag() == "always-active")
            setAlwaysActive(node->value<bool>());
        //else if(node->tag() == "disable-arrow-navitation")
        //    setArrowNavigation(node->value<bool>());
    }
}

void UILineEdit::onGeometryChange(const Rect& oldRect, const Rect& newRect)
{
    update();
    UIWidget::onGeometryChange(oldRect, newRect);
}

void UILineEdit::onFocusChange(bool focused, Fw::FocusReason reason)
{
    if(focused && !m_alwaysActive) {
        if(reason == Fw::TabFocusReason)
            setCursorPos(m_text.length());
        else
            blinkCursor();
    }
    UIWidget::onFocusChange(focused, reason);
}

bool UILineEdit::onKeyPress(uchar keyCode, int keyboardModifiers, bool wouldFilter)
{
    if(UIWidget::onKeyPress(keyCode, keyboardModifiers, wouldFilter))
        return true;

    if(!wouldFilter) {
        if(keyCode == Fw::KeyDelete) // erase right character
            removeCharacter(true);
        else if(keyCode == Fw::KeyBackspace) // erase left character {
            removeCharacter(false);
        else if(keyCode == Fw::KeyRight) // move cursor right
            moveCursor(true);
        else if(keyCode == Fw::KeyLeft) // move cursor left
            moveCursor(false);
        else if(keyCode == Fw::KeyHome) // move cursor to first character
            setCursorPos(0);
        else if(keyCode == Fw::KeyEnd) // move cursor to last character
            setCursorPos(m_text.length());
        else if(keyCode == Fw::KeyV && keyboardModifiers == Fw::KeyboardCtrlModifier)
            appendText(g_window.getClipboardText());
        else if(keyCode == Fw::KeyTab) {
            if(!m_alwaysActive) {
                if(UIWidgetPtr parent = getParent())
                    parent->focusNextChild(Fw::TabFocusReason);
            }
        } else
            return false;
        return true;
    }
    return false;
}

bool UILineEdit::onKeyText(const std::string& keyText)
{
    appendText(keyText);
    return true;
}

bool UILineEdit::onMousePress(const Point& mousePos, Fw::MouseButton button)
{
    if(button == Fw::MouseLeftButton) {
        int pos = getTextPos(mousePos);
        if(pos >= 0)
            setCursorPos(pos);
    }
    return true;
}

void UILineEdit::blinkCursor()
{
    m_cursorTicks = g_clock.ticks();
}
