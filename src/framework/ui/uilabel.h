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

#ifndef UILABEL_H
#define UILABEL_H

#include "uiwidget.h"

class UILabel : public UIWidget
{
public:
    UILabel();
    virtual void render();

    void resizeToText();

    void setText(const std::string& text);
    void setAlign(Fw::AlignmentFlag align) { m_align = align; }
    void setOffset(const Point& offset) { m_offset = offset; }

    std::string getText() const { return m_text; }
    Fw::AlignmentFlag getAlign() const { return m_align; }
    Point getOffset() const { return m_offset; }

protected:
    virtual void onStyleApply(const OTMLNodePtr& styleNode);

private:
    std::string m_text;
    Point m_offset;
    Fw::AlignmentFlag m_align;
};

#endif
