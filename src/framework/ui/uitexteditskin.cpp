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


#include "uitexteditskin.h"
#include "uitextedit.h"
#include "graphics/fonts.h"

void UITextEditSkin::load(const YAML::Node& node)
{
    UIElementSkin::load(node);
    std::string tmp;

    if(node.FindValue("font")) {
        node["font"] >> tmp;
        m_font = g_fonts.get(tmp);
    } else
        m_font = g_fonts.getDefaultFont();

    if(node.FindValue("text color"))
        node["text color"] >> m_textColor;
    else
        m_textColor = Color::white;

    if(node.FindValue("text margin"))
        node["text margin"] >> m_textMargin;
    else
        m_textMargin = 2;
}

void UITextEditSkin::draw(UIElement* element)
{
    UIElementSkin::draw(element);

    UITextEdit *textEdit = static_cast<UITextEdit*>(element);
    m_font->renderText(textEdit->getText(), textEdit->getRect(), ALIGN_TOP_LEFT, m_textColor);
}
