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


#ifndef UISKIN_H
#define UISKIN_H

#include <global.h>
#include <graphics/texture.h>
#include <ui/uielementskin.h>
#include <graphics/font.h>

class UISkins
{
public:
    UISkins() { }

    void load(const std::string& skinName);
    void terminate();

    UIElementSkinPtr getElementSkin(UI::ElementType elementType, const std::string& name = "default");
    TexturePtr getDefaultTexture() { return m_defaultTexture; }
    FontPtr getDefaultFont() { return m_defaultFont; }
    Color getDefaultFontColor() { return m_defaultFontColor; }

private:
    TexturePtr m_defaultTexture;
    FontPtr m_defaultFont;
    Color m_defaultFontColor;

    std::vector<UIElementSkinPtr> m_elementSkins;
};

extern UISkins g_uiSkins;

#endif // UISKIN_H
