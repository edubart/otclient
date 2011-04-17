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

#ifndef UIWINDOWSKIN_H
#define UIWINDOWSKIN_H

#include <prerequisites.h>
#include <graphics/font.h>
#include <ui/uielementskin.h>

class UIWindowSkin : public UIElementSkin
{
public:
    UIWindowSkin(const std::string& name) :
        UIElementSkin(name, UI::Window) { }

    void load(const YAML::Node& node);
    void draw(UIElement *element);

    int getHeadHeight() const { return m_headHeight; }

private:
    ImagePtr m_headImage;
    ImagePtr m_bodyImage;
    Font *m_titleFont;
    int m_headHeight;
    Color m_headTextColor;
};

#endif // UIWINDOWSKIN_H
