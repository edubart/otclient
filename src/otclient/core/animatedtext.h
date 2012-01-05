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

#ifndef ANIMATEDTEXT_H
#define ANIMATEDTEXT_H

#include "thing.h"
#include <framework/graphics/fontmanager.h>

class AnimatedText : public Thing
{
public:
    enum {
        DURATION = 1000
    };

    AnimatedText();

    void start();
    void draw(const Point& p, const Rect& visibleRect);

    void setColor(int color);
    void setText(const std::string& text);

    AnimatedTextPtr asAnimatedText() { return std::static_pointer_cast<AnimatedText>(shared_from_this()); }

private:
    FontPtr m_font;
    Size m_textSize;
    std::string m_text;
    Color m_color;
    double m_startTime;
};

#endif
