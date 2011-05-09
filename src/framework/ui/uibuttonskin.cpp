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
#include <graphics/fonts.h>
#include <ui/uibuttonskin.h>
#include <ui/uibutton.h>

void UIButtonSkin::load(const YAML::Node& node)
{
    UIElementSkin::load(node);

    m_buttonDownImage = loadImage(node["down state"]);

    if(node["down state"].FindValue("text translate"))
        node["down state"]["text translate"] >> m_buttonDownTranslate;

    if(node.FindValue("mouse over state"))
        m_buttonHoverImage = loadImage(node["mouse over state"]);

    m_font = g_fonts.get(node["font"].Read<std::string>());

    node["text color"] >> m_textColor;
}

void UIButtonSkin::draw(UIElement *element)
{
    UIButton *button = static_cast<UIButton*>(element);

    Rect textRect = button->getRect();

    if(button->getState() == UIButton::ButtonDown && m_buttonDownImage) {
        m_buttonDownImage->draw(element->getRect());
        textRect.translate(m_buttonDownTranslate);
    } else if(button->getState() == UIButton::ButtonMouseOver && m_buttonHoverImage) {
        m_buttonHoverImage->draw(element->getRect());
    } else {
        UIElementSkin::draw(element);
    }

    m_font->renderText(button->getText(), textRect, ALIGN_CENTER, m_textColor);
}
