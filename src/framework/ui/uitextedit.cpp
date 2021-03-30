/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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

#include "uitextedit.h"
#include <framework/graphics/bitmapfont.h>
#include <framework/graphics/graphics.h>
#include <framework/platform/platformwindow.h>
#include <framework/core/clock.h>
#include <framework/otml/otmlnode.h>
#include <framework/core/application.h>
#include <framework/input/mouse.h>

UITextEdit::UITextEdit()
{
    m_cursorPos = 0;
    m_textAlign = Fw::AlignTopLeft;
    m_textHidden = false;
    m_shiftNavigation = false;
    m_multiline = false;
    m_cursorVisible = true;
    m_cursorInRange = true;
    m_maxLength = 0;
    m_editable = true;
    m_selectable = true;
    m_autoScroll = true;
    m_changeCursorImage = true;
    m_selectionReference = 0;
    m_selectionStart = 0;
    m_selectionEnd = 0;
    m_updatesEnabled = true;
    m_selectionColor = Color::white;
    m_selectionBackgroundColor = Color::black;
    m_glyphsTextCoordsBuffer.enableHardwareCaching();
    m_glyphsSelectCoordsBuffer.enableHardwareCaching();
    m_glyphsMustRecache = true;
    blinkCursor();
}

void UITextEdit::drawSelf(Fw::DrawPane drawPane)
{
    if((drawPane & Fw::ForegroundPane) == 0)
        return;

    drawBackground(m_rect);
    drawBorder(m_rect);
    drawImage(m_rect);
    drawIcon(m_rect);

    int textLength = m_text.length();
    const TexturePtr& texture = m_font->getTexture();
    if(!texture)
        return;

    bool glyphsMustRecache = m_glyphsMustRecache;
    if(glyphsMustRecache)
        m_glyphsMustRecache = false;

    if(m_color != Color::alpha) {
        if(glyphsMustRecache) {
            m_glyphsTextCoordsBuffer.clear();
            for(int i=0;i<textLength;++i)
                m_glyphsTextCoordsBuffer.addRect(m_glyphsCoords[i], m_glyphsTexCoords[i]);
        }
        g_painter->setColor(m_color);
        g_painter->drawTextureCoords(m_glyphsTextCoordsBuffer, texture);
    }

    if(hasSelection()) {
        if(glyphsMustRecache) {
            m_glyphsSelectCoordsBuffer.clear();
            for(int i=m_selectionStart;i<m_selectionEnd;++i)
                m_glyphsSelectCoordsBuffer.addRect(m_glyphsCoords[i], m_glyphsTexCoords[i]);
        }
        g_painter->setColor(m_selectionBackgroundColor);
        g_painter->drawFillCoords(m_glyphsSelectCoordsBuffer);
        g_painter->setColor(m_selectionColor);
        g_painter->drawTextureCoords(m_glyphsSelectCoordsBuffer, texture);
    }

    // render cursor
    if(isExplicitlyEnabled() && m_cursorVisible && m_cursorInRange && isActive() && m_cursorPos >= 0) {
        assert(m_cursorPos <= textLength);
        // draw every 333ms
        const int delay = 333;
        int elapsed = g_clock.millis() - m_cursorTicks;
        if(elapsed <= delay) {
            Rect cursorRect;
            // when cursor is at 0
            if(m_cursorPos == 0)
                cursorRect = Rect(m_rect.left()+m_padding.left, m_rect.top()+m_padding.top, 1, m_font->getGlyphHeight());
            else
                cursorRect = Rect(m_glyphsCoords[m_cursorPos-1].right(), m_glyphsCoords[m_cursorPos-1].top(), 1, m_font->getGlyphHeight());

            if(hasSelection() && m_cursorPos >= m_selectionStart && m_cursorPos <= m_selectionEnd)
                g_painter->setColor(m_selectionColor);
            else
                g_painter->setColor(m_color);

            g_painter->drawFilledRect(cursorRect);
        } else if(elapsed >= 2*delay) {
            m_cursorTicks = g_clock.millis();
        }
    }

    g_painter->resetColor();
}

void UITextEdit::update(bool focusCursor)
{
    if(!m_updatesEnabled)
        return;

    std::string text = getDisplayedText();
    m_drawText = text;
    int textLength = text.length();

    // prevent glitches
    if(m_rect.isEmpty())
        return;

    // recache coords buffers
    recacheGlyphs();

    // map glyphs positions
    Size textBoxSize;
    const auto& glyphsPositions = m_font->calculateGlyphsPositions(text, m_textAlign, &textBoxSize);
    const Rect* glyphsTextureCoords = m_font->getGlyphsTextureCoords();
    const Size* glyphsSize = m_font->getGlyphsSize();
    int glyph;

    // update rect size
    if(!m_rect.isValid() || m_textHorizontalAutoResize || m_textVerticalAutoResize) {
        textBoxSize += Size(m_padding.left + m_padding.right, m_padding.top + m_padding.bottom) + m_textOffset.toSize();
        Size size = getSize();
        if(size.width() <= 0 || (m_textHorizontalAutoResize && !m_textWrap))
            size.setWidth(textBoxSize.width());
        if(size.height() <= 0 || m_textVerticalAutoResize)
            size.setHeight(textBoxSize.height());
        setSize(size);
    }

    // resize just on demand
    if(textLength > (int)m_glyphsCoords.size()) {
        m_glyphsCoords.resize(textLength);
        m_glyphsTexCoords.resize(textLength);
    }

    Point oldTextAreaOffset = m_textVirtualOffset;

    if(textBoxSize.width() <= getPaddingRect().width())
        m_textVirtualOffset.x = 0;
    if(textBoxSize.height() <= getPaddingRect().height())
        m_textVirtualOffset.y = 0;

    // readjust start view area based on cursor position
    m_cursorInRange = false;
    if(focusCursor && m_autoScroll) {
        if(m_cursorPos > 0 && textLength > 0) {
                assert(m_cursorPos <= textLength);
                Rect virtualRect(m_textVirtualOffset, m_rect.size() - Size(m_padding.left+m_padding.right, 0)); // previous rendered virtual rect
                int pos = m_cursorPos - 1; // element before cursor
                glyph = (uchar)text[pos]; // glyph of the element before cursor
                Rect glyphRect(glyphsPositions[pos], glyphsSize[glyph]);

                // if the cursor is not on the previous rendered virtual rect we need to update it
                if(!virtualRect.contains(glyphRect.topLeft()) || !virtualRect.contains(glyphRect.bottomRight())) {
                    // calculate where is the first glyph visible
                    Point startGlyphPos;
                    startGlyphPos.y = std::max<int>(glyphRect.bottom() - virtualRect.height(), 0);
                    startGlyphPos.x = std::max<int>(glyphRect.right() - virtualRect.width(), 0);

                    // find that glyph
                    for(pos = 0; pos < textLength; ++pos) {
                        glyph = (uchar)text[pos];
                        glyphRect = Rect(glyphsPositions[pos], glyphsSize[glyph]);
                        glyphRect.setTop(std::max<int>(glyphRect.top() - m_font->getYOffset() - m_font->getGlyphSpacing().height(), 0));
                        glyphRect.setLeft(std::max<int>(glyphRect.left() - m_font->getGlyphSpacing().width(), 0));

                        // first glyph entirely visible found
                        if(glyphRect.topLeft() >= startGlyphPos) {
                            m_textVirtualOffset.x = glyphsPositions[pos].x;
                            m_textVirtualOffset.y = glyphsPositions[pos].y - m_font->getYOffset();
                            break;
                        }
                    }
                }
        } else {
            m_textVirtualOffset = Point(0,0);
        }
        m_cursorInRange = true;
    } else {
        if(m_cursorPos > 0 && textLength > 0) {
            Rect virtualRect(m_textVirtualOffset, m_rect.size() - Size(2*m_padding.left+m_padding.right, 0) ); // previous rendered virtual rect
            int pos = m_cursorPos - 1; // element before cursor
            glyph = (uchar)text[pos]; // glyph of the element before cursor
            Rect glyphRect(glyphsPositions[pos], glyphsSize[glyph]);
            if(virtualRect.contains(glyphRect.topLeft()) && virtualRect.contains(glyphRect.bottomRight()))
                m_cursorInRange = true;
        } else {
            m_cursorInRange = true;
        }
    }

    bool fireAreaUpdate = false;
    if(oldTextAreaOffset != m_textVirtualOffset)
        fireAreaUpdate = true;

    Rect textScreenCoords = m_rect;
    textScreenCoords.expandLeft(-m_padding.left);
    textScreenCoords.expandRight(-m_padding.right);
    textScreenCoords.expandBottom(-m_padding.bottom);
    textScreenCoords.expandTop(-m_padding.top);
    m_drawArea = textScreenCoords;

    if(textScreenCoords.size() != m_textVirtualSize) {
        m_textVirtualSize = textScreenCoords.size();
        fireAreaUpdate = true;
    }

    Size totalSize = textBoxSize;
    if(totalSize.width() < m_textVirtualSize.width())
        totalSize.setWidth(m_textVirtualSize.height());
    if(totalSize.height() < m_textVirtualSize.height())
        totalSize.setHeight(m_textVirtualSize.height());
    if(m_textTotalSize != totalSize) {
        m_textTotalSize = totalSize;
        fireAreaUpdate = true;
    }

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
        if(glyph < 32 && glyph != (uchar)'\n')
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
        if(glyphScreenCoords.bottom() < m_textVirtualOffset.y || glyphScreenCoords.right() < m_textVirtualOffset.x)
            continue;

        // bound glyph topLeft to startRenderPosition
        if(glyphScreenCoords.top() < m_textVirtualOffset.y) {
            glyphTextureCoords.setTop(glyphTextureCoords.top() + (m_textVirtualOffset.y - glyphScreenCoords.top()));
            glyphScreenCoords.setTop(m_textVirtualOffset.y);
        }
        if(glyphScreenCoords.left() < m_textVirtualOffset.x) {
            glyphTextureCoords.setLeft(glyphTextureCoords.left() + (m_textVirtualOffset.x - glyphScreenCoords.left()));
            glyphScreenCoords.setLeft(m_textVirtualOffset.x);
        }

        // subtract startInternalPos
        glyphScreenCoords.translate(-m_textVirtualOffset);

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

    if(fireAreaUpdate)
        onTextAreaUpdate(m_textVirtualOffset, m_textVirtualSize, m_textTotalSize);

    g_app.repaint();
}

void UITextEdit::setCursorPos(int pos)
{
    if(pos < 0)
        pos = m_text.length();

    if(pos != m_cursorPos) {
        if(pos < 0)
            m_cursorPos = 0;
        else if((uint)pos >= m_text.length())
            m_cursorPos = m_text.length();
        else
            m_cursorPos = pos;
        update(true);
    }
}

void UITextEdit::setSelection(int start, int end)
{
    if(start == m_selectionStart && end == m_selectionEnd)
        return;

    if(start > end)
        std::swap(start, end);

    if(end == -1)
        end = m_text.length();

    m_selectionStart = stdext::clamp<int>(start, 0, (int)m_text.length());
    m_selectionEnd = stdext::clamp<int>(end, 0, (int)m_text.length());
    recacheGlyphs();
}

void UITextEdit::setTextHidden(bool hidden)
{
    m_textHidden = hidden;
    update(true);
}

void UITextEdit::setTextVirtualOffset(const Point& offset)
{
    m_textVirtualOffset = offset;
    update();
}

void UITextEdit::appendText(std::string text)
{
    if(hasSelection())
        del();

    if(m_cursorPos >= 0) {
        // replace characters that are now allowed
        if(!m_multiline)
            stdext::replace_all(text, "\n", " ");
        stdext::replace_all(text, "\r", "");
        stdext::replace_all(text, "\t", "    ");

        if(text.length() > 0) {
            // only add text if textedit can add it
            if(m_maxLength > 0 && m_text.length() + text.length() > m_maxLength)
                return;

            // only ignore text append if it contains invalid characters
            if(!m_validCharacters.empty()) {
                for(char i: text) {
                    if(m_validCharacters.find(i) == std::string::npos)
                        return;
                }
            }

            std::string tmp = m_text;
            tmp.insert(m_cursorPos, text);
            m_cursorPos += text.length();
            setText(tmp);
        }
    }
}

void UITextEdit::appendCharacter(char c)
{
    if((c == '\n' && !m_multiline) || c == '\r')
        return;

    if(hasSelection())
        del();

    if(m_cursorPos >= 0) {
        if(m_maxLength > 0 && m_text.length() + 1 > m_maxLength)
            return;

        if(!m_validCharacters.empty() && m_validCharacters.find(c) == std::string::npos)
            return;

        std::string tmp;
        tmp = c;
        std::string tmp2 = m_text;
        tmp2.insert(m_cursorPos, tmp);
        m_cursorPos++;
        setText(tmp2);
    }
}

void UITextEdit::removeCharacter(bool right)
{
    std::string tmp = m_text;
    if(m_cursorPos >= 0 && tmp.length() > 0) {
        if((uint)m_cursorPos >= tmp.length()) {
            tmp.erase(tmp.begin() + (--m_cursorPos));
        } else {
            if(right)
                tmp.erase(tmp.begin() + m_cursorPos);
            else if(m_cursorPos > 0)
                tmp.erase(tmp.begin() + --m_cursorPos);
        }
        setText(tmp);
    }
}

void UITextEdit::blinkCursor()
{
    m_cursorTicks = g_clock.millis();
    g_app.repaint();
}

void UITextEdit::del(bool right)
{
    if(hasSelection()) {
        std::string tmp = m_text;
        tmp.erase(m_selectionStart, m_selectionEnd - m_selectionStart);

        setCursorPos(m_selectionStart);
        clearSelection();
        setText(tmp);
    } else
        removeCharacter(right);
}

void UITextEdit::paste(const std::string& text)
{
    if(hasSelection())
        del();
    appendText(text);
}

std::string UITextEdit::copy()
{
    std::string text;
    if(hasSelection()) {
        text = getSelection();
        g_window.setClipboardText(text);
    }
    return text;
}

std::string UITextEdit::cut()
{
    std::string text = copy();
    del();
    return text;
}

void UITextEdit::wrapText()
{
    setText(m_font->wrapText(m_text, getPaddingRect().width() - m_textOffset.x));
}

void UITextEdit::moveCursorHorizontally(bool right)
{
    if(right) {
        if((uint)m_cursorPos+1 <= m_text.length())
            m_cursorPos++;
        else
            m_cursorPos = 0;
    } else {
        if(m_cursorPos-1 >= 0)
            m_cursorPos--;
        else
            m_cursorPos = m_text.length();
    }

    blinkCursor();
    update(true);
}

void UITextEdit::moveCursorVertically(bool up)
{
    //TODO
}

int UITextEdit::getTextPos(Point pos)
{
    int textLength = m_text.length();

    // find any glyph that is actually on the
    int candidatePos = -1;
    Rect firstGlyphRect, lastGlyphRect;
    for(int i=0;i<textLength;++i) {
        Rect clickGlyphRect = m_glyphsCoords[i];
        if(!clickGlyphRect.isValid())
            continue;
        if(!firstGlyphRect.isValid())
            firstGlyphRect = clickGlyphRect;
        lastGlyphRect = clickGlyphRect;
        clickGlyphRect.expandTop(m_font->getYOffset() + m_font->getGlyphSpacing().height());
        clickGlyphRect.expandLeft(m_font->getGlyphSpacing().width()+1);
        if(clickGlyphRect.contains(pos)) {
            candidatePos = i;
            break;
        }
        else if(pos.y >= clickGlyphRect.top() && pos.y <= clickGlyphRect.bottom()) {
            if(pos.x <= clickGlyphRect.left()) {
                candidatePos = i;
                break;
            } else if(pos.x >= clickGlyphRect.right())
                candidatePos = i+1;
        }
    }

    if(textLength > 0) {
        if(pos.y < firstGlyphRect.top())
            return 0;
        else if(pos.y > lastGlyphRect.bottom())
            return textLength;
    }

    return candidatePos;
}

std::string UITextEdit::getDisplayedText()
{
    std::string text;
    if(m_textHidden)
        text = std::string(m_text.length(), '*');
    else
        text = m_text;

    if(m_textWrap && m_rect.isValid())
        text = m_font->wrapText(text, getPaddingRect().width() - m_textOffset.x);

    return text;
}

std::string UITextEdit::getSelection()
{
    if(!hasSelection())
        return std::string();
    return m_text.substr(m_selectionStart, m_selectionEnd - m_selectionStart);
}

void UITextEdit::updateText()
{
    if(m_cursorPos > (int)m_text.length())
        m_cursorPos = m_text.length();

    // any text changes reset the selection
    if(m_selectable) {
        m_selectionEnd = 0;
        m_selectionStart = 0;
    }

    blinkCursor();
    update(true);
}

void UITextEdit::onHoverChange(bool hovered)
{
    if(m_changeCursorImage) {
        if(hovered && !g_mouse.isCursorChanged())
            g_mouse.pushCursor("text");
        else
            g_mouse.popCursor("text");
    }
}

void UITextEdit::onStyleApply(const std::string& styleName, const OTMLNodePtr& styleNode)
{
    UIWidget::onStyleApply(styleName, styleNode);

    for(const OTMLNodePtr& node : styleNode->children()) {
        if(node->tag() == "text") {
            setText(node->value());
            setCursorPos(m_text.length());
        } else if(node->tag() == "text-hidden")
            setTextHidden(node->value<bool>());
        else if(node->tag() == "shift-navigation")
            setShiftNavigation(node->value<bool>());
        else if(node->tag() == "multiline")
            setMultiline(node->value<bool>());
        else if(node->tag() == "max-length")
            setMaxLength(node->value<int>());
        else if(node->tag() == "editable")
            setEditable(node->value<bool>());
        else if(node->tag() == "selectable")
            setSelectable(node->value<bool>());
        else if(node->tag() == "selection-color")
            setSelectionColor(node->value<Color>());
        else if(node->tag() == "selection-background-color")
            setSelectionBackgroundColor(node->value<Color>());
        else if(node->tag() == "selection") {
            Point selectionRange = node->value<Point>();
            setSelection(selectionRange.x, selectionRange.y);
        }
        else if(node->tag() == "cursor-visible")
            setCursorVisible(node->value<bool>());
        else if(node->tag() == "change-cursor-image")
            setChangeCursorImage(node->value<bool>());
        else if(node->tag() == "auto-scroll")
            setAutoScroll(node->value<bool>());
    }
}

void UITextEdit::onGeometryChange(const Rect& oldRect, const Rect& newRect)
{
    update(true);
    UIWidget::onGeometryChange(oldRect, newRect);
}

void UITextEdit::onFocusChange(bool focused, Fw::FocusReason reason)
{
    if(focused) {
        if(reason == Fw::KeyboardFocusReason)
            setCursorPos(m_text.length());
        else
            blinkCursor();
        update(true);
    } else if(m_selectable)
        clearSelection();
    UIWidget::onFocusChange(focused, reason);
}

bool UITextEdit::onKeyPress(uchar keyCode, int keyboardModifiers, int autoRepeatTicks)
{
    if(UIWidget::onKeyPress(keyCode, keyboardModifiers, autoRepeatTicks))
        return true;

    if(keyboardModifiers == Fw::KeyboardNoModifier) {
        if(keyCode == Fw::KeyDelete && m_editable) { // erase right character
            if(hasSelection() || !m_text.empty()) {
                del(true);
                return true;
            }
        } else if(keyCode == Fw::KeyBackspace && m_editable) { // erase left character
            if(hasSelection() || !m_text.empty()) {
                del(false);
                return true;
            }
        } else if(keyCode == Fw::KeyRight && !m_shiftNavigation) { // move cursor right
            clearSelection();
            moveCursorHorizontally(true);
            return true;
        } else if(keyCode == Fw::KeyLeft && !m_shiftNavigation) { // move cursor left
            clearSelection();
            moveCursorHorizontally(false);
            return true;
        } else if(keyCode == Fw::KeyHome) { // move cursor to first character
            if(m_cursorPos != 0) {
                clearSelection();
                setCursorPos(0);
                return true;
            }
        } else if(keyCode == Fw::KeyEnd) { // move cursor to last character
            if(m_cursorPos != (int)m_text.length()) {
                clearSelection();
                setCursorPos(m_text.length());
                return true;
            }
        } else if(keyCode == Fw::KeyTab && !m_shiftNavigation) {
            clearSelection();
            if(UIWidgetPtr parent = getParent())
                parent->focusNextChild(Fw::KeyboardFocusReason, true);
            return true;
        } else if(keyCode == Fw::KeyEnter && m_multiline && m_editable) {
            appendCharacter('\n');
            return true;
        } else if(keyCode == Fw::KeyUp && !m_shiftNavigation && m_multiline) {
            moveCursorVertically(true);
            return true;
        } else if(keyCode == Fw::KeyDown && !m_shiftNavigation && m_multiline) {
            moveCursorVertically(false);
            return true;
        }
    } else if(keyboardModifiers == Fw::KeyboardCtrlModifier) {
        if(keyCode == Fw::KeyV && m_editable) {
            paste(g_window.getClipboardText());
            return true;
        } else if(keyCode == Fw::KeyX && m_editable && m_selectable) {
            if(hasSelection()) {
                cut();
                return true;
            }
        } else if(keyCode == Fw::KeyC && m_selectable) {
            if(hasSelection()) {
                copy();
                return true;
            }
        } else if(keyCode == Fw::KeyA && m_selectable) {
            if(m_text.length() > 0) {
                selectAll();
                return true;
            }
        }
    } else if(keyboardModifiers == Fw::KeyboardShiftModifier) {
        if(keyCode == Fw::KeyTab && !m_shiftNavigation) {
            if(UIWidgetPtr parent = getParent())
                parent->focusPreviousChild(Fw::KeyboardFocusReason, true);
            return true;
        } else if(keyCode == Fw::KeyRight || keyCode == Fw::KeyLeft) {

            int oldCursorPos = m_cursorPos;

            if(keyCode == Fw::KeyRight) // move cursor right
                moveCursorHorizontally(true);
            else if(keyCode == Fw::KeyLeft) // move cursor left
                moveCursorHorizontally(false);

            if(m_shiftNavigation)
                clearSelection();
            else {
                if(!hasSelection())
                    m_selectionReference = oldCursorPos;
                setSelection(m_selectionReference, m_cursorPos);
            }
            return true;
        } else if(keyCode == Fw::KeyHome) { // move cursor to first character
            if(m_cursorPos != 0) {
                setSelection(m_cursorPos, 0);
                setCursorPos(0);
                return true;
            }
        } else if(keyCode == Fw::KeyEnd) { // move cursor to last character
            if(m_cursorPos != (int)m_text.length()) {
                setSelection(m_cursorPos, m_text.length());
                setCursorPos(m_text.length());
                return true;
            }
        }
    }

    return false;
}

bool UITextEdit::onKeyText(const std::string& keyText)
{
    if(m_editable) {
        appendText(keyText);
        return true;
    }
    return false;
}

bool UITextEdit::onMousePress(const Point& mousePos, Fw::MouseButton button)
{
    if(UIWidget::onMousePress(mousePos, button))
        return true;

    if(button == Fw::MouseLeftButton) {
        int pos = getTextPos(mousePos);
        if(pos >= 0) {
            setCursorPos(pos);

            if(m_selectable) {
                m_selectionReference = pos;
                setSelection(pos, pos);
            }
        }
        return true;
    }
    return false;
}

bool UITextEdit::onMouseRelease(const Point& mousePos, Fw::MouseButton button)
{
    return UIWidget::onMouseRelease(mousePos, button);
}

bool UITextEdit::onMouseMove(const Point& mousePos, const Point& mouseMoved)
{
    if(UIWidget::onMouseMove(mousePos, mouseMoved))
        return true;

    if(m_selectable && isPressed()) {
        int pos = getTextPos(mousePos);
        if(pos >= 0) {
            setSelection(m_selectionReference, pos);
            setCursorPos(pos);
        }
        return true;
    }
    return false;
}

bool UITextEdit::onDoubleClick(const Point& mousePos)
{
    if(UIWidget::onDoubleClick(mousePos))
        return true;
    if(m_selectable && m_text.length() > 0) {
        selectAll();
        return true;
    }
    return false;
}

void UITextEdit::onTextAreaUpdate(const Point& offset, const Size& visibleSize, const Size& totalSize)
{
    callLuaField("onTextAreaUpdate", offset, visibleSize, totalSize);
}
