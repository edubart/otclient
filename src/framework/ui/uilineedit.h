/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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

#ifndef UILINEEDIT_H
#define UILINEEDIT_H

#include "uiwidget.h"

class UILineEdit : public UIWidget
{
public:
    UILineEdit();

    virtual void render();

    void update();

    void setText(const std::string& text);
    void setTextHidden(bool hidden);
    void setAlign(Fw::AlignmentFlag align);
    void setCursorPos(int pos);
    void setCursorEnabled(bool enable = true);

    void clearText() { setText(""); }
    void moveCursor(bool right);
    void appendText(std::string text);
    void appendCharacter(char c);
    void removeCharacter(bool right);

    void setFont(const FontPtr& font);
    std::string getText() const { return m_text; }
    std::string getDisplayedText();
    int getTextPos(Point pos);
    int getCursorPos() const { return m_cursorPos; }

protected:
    virtual void onStyleApply(const OTMLNodePtr& styleNode);
    virtual void onGeometryUpdate(const Rect& oldRect, const Rect& newRect);
    virtual void onFocusChange(bool focused, Fw::FocusReason reason);
    virtual bool onKeyPress(uchar keyCode, char keyChar, int keyboardModifiers);
    virtual bool onMousePress(const Point& mousePos, Fw::MouseButton button);

private:
    void blinkCursor();

    std::string m_text;
    Rect m_drawArea;
    Fw::AlignmentFlag m_align;
    int m_cursorPos;
    Point m_startInternalPos;
    int m_startRenderPos;
    int m_cursorTicks;
    int m_textHorizontalMargin;
    bool m_textHidden;

    std::vector<Rect> m_glyphsCoords;
    std::vector<Rect> m_glyphsTexCoords;
};

#endif
