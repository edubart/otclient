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


#include "uielement.h"
#include "uiskins.h"
#include "uielementskin.h"

UIElement::UIElement(UI::EElementType type) :
    UILayout(),
    m_type(type),
    m_skin(NULL),
    m_visible(true),
    m_enabled(true)
{
    // set default skin
    if(type > UI::Container)
        setSkin(g_uiSkins.getElementSkin(type));
}

bool UIElement::setSkin(const std::string& skinName)
{
    setSkin(g_uiSkins.getElementSkin(m_type, skinName));
    return m_skin != NULL;
}

void UIElement::setSkin(UIElementSkin* skin)
{
    if(skin && !getRect().isValid()) {
        setSize(skin->getDefaultSize());
    }
    m_skin = skin;
}

void UIElement::render()
{
    if(m_skin)
        m_skin->draw(this);
}
