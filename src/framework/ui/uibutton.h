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

#ifndef UIBUTTON_H
#define UIBUTTON_H

#include "uiwidget.h"

class UIButton : public UIWidget
{
public:
    virtual void setup();
    virtual void render();

    void setOnClick(const SimpleCallback& onClick) { m_onClick = onClick; }
    void setText(const std::string& text) { m_text = text; }

    SimpleCallback getOnClick() const { return m_onClick; }
    std::string getText() const { return m_text; }

    UIButtonPtr asUIButton() { return std::static_pointer_cast<UIButton>(shared_from_this()); }

protected:
    virtual void onStyleApply(const OTMLNodePtr& styleNode);
    virtual bool onMouseRelease(const Point& mousePos, MouseButton button);

    SimpleCallback m_onClick;
    Point m_textTranslate;
    std::string m_text;
};

#endif
