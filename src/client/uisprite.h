/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
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

#ifndef UISPRITE_H
#define UISPRITE_H

#include "declarations.h"
#include <framework/ui/uiwidget.h>

class UISprite : public UIWidget
{
public:
    UISprite();
    void drawSelf(Fw::DrawPane drawPane);

    void setSpriteId(int id);
    int getSpriteId() { return m_spriteId; }
    void clearSprite() { setSpriteId(0); }

    void setSpriteColor(Color color) { m_spriteColor = color; }

    bool isSpriteVisible() { return m_spriteVisible; }
    void setSpriteVisible(bool visible) { m_spriteVisible = visible; }

    bool hasSprite() { return m_sprite != nullptr; }

protected:
    void onStyleApply(const std::string& styleName, const OTMLNodePtr& styleNode);

    TexturePtr m_sprite;
    uint16 m_spriteId;
    Color m_spriteColor;

    stdext::boolean<true> m_spriteVisible;
};

#endif
