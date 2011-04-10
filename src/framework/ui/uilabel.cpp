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


#include "uilabel.h"
#include "../fonts.h"

UILabel::UILabel(const std::string& text, Font* font) :
    UIElement(UI::Label),
    m_text(text),
    m_font(font)
{
    if(!font)
        m_font = g_fonts.get("tibia-10px-antialised");
    setSize(m_font->calculateTextRectSize(text));
}

void UILabel::load(const YAML::Node& node)
{
    UIElement::load(node);

    std::string text;
    node["text"] >> text;
    setText(text);
}

void UILabel::render()
{
    m_font->renderText(m_text, getRect(), ALIGN_LEFT, Color(0xFFBFBFBF));
}

void UILabel::setText(const std::string& text)
{
    setSize(m_font->calculateTextRectSize(text));
    m_text = text;
}

