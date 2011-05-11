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
#include <ui/uiwindowskin.h>
#include <ui/uiwindow.h>

void UIWindowSkin::load(const YAML::Node& node)
{
    UIElementSkin::load(node);

    const YAML::Node& headNode = node["head"];
    const YAML::Node& bodyNode = node["body"];

    m_headImage = boost::dynamic_pointer_cast<BorderedImage>(loadImage(headNode));
    m_headHeight = yamlRead(headNode, "height", m_headImage->getDefaultSize().height());
    m_headMargin = yamlRead(headNode, "margin", 0);
    m_titleAlign = parseAlignment(yamlRead(headNode, "text align", std::string("center")));
    m_bodyImage = loadImage(bodyNode);
}

void UIWindowSkin::draw(UIElement* element)
{
    UIElementSkin::draw(element);
    UIWindow *window = static_cast<UIWindow*>(element);

    // draw window head
    Rect headRect = window->getRect();
    headRect.setHeight(m_headHeight);
    m_headImage->draw(headRect);

    // draw window head text
    Rect headTextRect = headRect;
    if(m_titleAlign & AlignLeft)
        headTextRect.addLeft(-m_headMargin);
    else if(m_titleAlign & AlignRight)
        headTextRect.addRight(-m_headMargin);
    getFont()->renderText(window->getTitle(), headTextRect, m_titleAlign, getFontColor());

    // draw window body
    Rect bodyRect = window->getRect();
    bodyRect.setTop(headRect.bottom() + 1);
    m_bodyImage->draw(bodyRect);
}
