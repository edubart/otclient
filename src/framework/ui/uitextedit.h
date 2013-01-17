/*
 * Copyright (c) 2010-2013 OTClient <https://github.com/edubart/otclient>
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

#ifndef UITEXTEDIT_H
#define UITEXTEDIT_H

#include "uiwidget.h"

// @bindclass
class UITextEdit : public UIWidget
{
public:
    UITextEdit();

    void drawSelf(Fw::DrawPane drawPane);

private:
    void update(bool focusCursor = false);

public:
    void setCursorPos(int pos);
    void setCursorVisible(bool enable) { m_cursorVisible = enable; }
    void setTextHidden(bool hidden);
    void setValidCharacters(const std::string validCharacters) { m_validCharacters = validCharacters; }
    void setShiftNavigation(bool enable) { m_shiftNavigation = enable; }
    void setMultiline(bool enable) { m_multiline = enable; }
    void setMaxLength(uint maxLength) { m_maxLength = maxLength; }
    void setTextVirtualOffset(const Point& offset);
    void setEditable(bool editable) { m_editable = editable; }

    void moveCursor(bool right);
    void appendText(std::string text);
    void appendCharacter(char c);
    void removeCharacter(bool right);

    void wrapText();
    std::string getDisplayedText();
    int getTextPos(Point pos);
    int getCursorPos() { return m_cursorPos; }
    Point getTextVirtualOffset() { return m_textVirtualOffset; }
    Size getTextVirtualSize() { return m_textVirtualSize; }
    Size getTextTotalSize() { return m_textTotalSize; }
    uint getMaxLength() { return m_maxLength; }
    bool isCursorVisible() { return m_cursorVisible; }
    bool isTextHidden() { return m_textHidden; }
    bool isShiftNavigation() { return m_shiftNavigation; }
    bool isMultiline() { return m_multiline; }
    bool isEditable() { return m_editable; }

protected:
    virtual void onHoverChange(bool hovered);
    virtual void onTextChange(const std::string& text, const std::string& oldText);
    virtual void onFontChange(const std::string& font);
    virtual void onStyleApply(const std::string& styleName, const OTMLNodePtr& styleNode);
    virtual void onGeometryChange(const Rect& oldRect, const Rect& newRect);
    virtual void onFocusChange(bool focused, Fw::FocusReason reason);
    virtual bool onKeyText(const std::string& keyText);
    virtual bool onKeyPress(uchar keyCode, int keyboardModifiers, int autoRepeatTicks);
    virtual bool onMousePress(const Point& mousePos, Fw::MouseButton button);
    virtual void onTextAreaUpdate(const Point& vitualOffset, const Size& virtualSize, const Size& totalSize);

private:
    void blinkCursor();

    Rect m_drawArea;
    int m_cursorPos;
    Point m_textVirtualOffset;
    Size m_textVirtualSize;
    Size m_textTotalSize;
    ticks_t m_cursorTicks;
    bool m_textHidden;
    bool m_shiftNavigation;
    bool m_multiline;
    bool m_cursorInRange;
    bool m_cursorVisible;
    bool m_editable;
    std::string m_validCharacters;
    uint m_maxLength;

    std::vector<Rect> m_glyphsCoords;
    std::vector<Rect> m_glyphsTexCoords;
};

#endif
