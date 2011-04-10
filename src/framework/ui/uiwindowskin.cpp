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

#include "uiwindowskin.h"
#include "uiwindow.h"
#include "graphics/fonts.h"

void UIWindowSkin::draw(UIElement* element)
{
    UIElementSkin::draw(element);

    UIWindow *window = static_cast<UIWindow*>(element);

    Rect headRect = window->getRect();
    Rect bodyRect = window->getRect();

    headRect.setHeight(m_headHeight);
    bodyRect.setTop(headRect.bottom() + 1);

    m_headImage->draw(headRect);
    m_titleFont->renderText(window->getTitle(),
                            headRect,
                            ALIGN_CENTER,
                            Color(0xFF8F8F8F));

    m_bodyImage->draw(bodyRect);
}

void UIWindowSkin::load(const YAML::Node& node)
{
    UIElementSkin::load(node);

    node["head"]["height"] >> m_headHeight;
    m_headImage = loadImage(node["head"]);
    m_bodyImage = loadImage(node["body"]);

    std::string fontName;
    node["head"]["font"] >> fontName;
    m_titleFont = g_fonts.get(fontName);
}
