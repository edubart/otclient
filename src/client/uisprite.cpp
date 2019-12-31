/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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

#include "uisprite.h"
#include <framework/otml/otml.h>
#include <framework/graphics/graphics.h>
#include <framework/graphics/texturemanager.h>
#include <client/spritemanager.h>

UISprite::UISprite() :
    m_spriteId(0),
    m_spriteColor(Color::white)
{ }

void UISprite::drawSelf(Fw::DrawPane drawPane)
{
    if((drawPane & Fw::ForegroundPane) == 0)
        return;

    // draw style components in order
    if(m_backgroundColor.aF() > Fw::MIN_ALPHA) {
        Rect backgroundDestRect = m_rect;
        backgroundDestRect.expand(-m_borderWidth.top, -m_borderWidth.right, -m_borderWidth.bottom, -m_borderWidth.left);
        drawBackground(m_rect);
    }

    drawImage(m_rect);

    if(m_spriteVisible && m_sprite) {
        g_painter->setColor(m_spriteColor);
        g_painter->drawTexturedRect(getPaddingRect(), m_sprite);
    }

    drawBorder(m_rect);
    drawIcon(m_rect);
    drawText(m_rect);
}

void UISprite::setSpriteId(int id)
{
    if(!g_sprites.isLoaded())
        return;

    m_spriteId = id;
    if(id == 0)
        m_sprite = nullptr;
    else {
        ImagePtr image = g_sprites.getSpriteImage(id);
        if(image)
            m_sprite = new Texture(image);
        else
            m_sprite = nullptr;
    }
}

void UISprite::onStyleApply(const std::string& styleName, const OTMLNodePtr& styleNode)
{
    UIWidget::onStyleApply(styleName, styleNode);

    for(const OTMLNodePtr& node : styleNode->children()) {
        if(node->tag() == "sprite-id")
            setSpriteId(node->value<int>());
        else if(node->tag() == "sprite-visible")
            setSpriteVisible(node->value<bool>());
        else if(node->tag() == "sprite-color")
            setSpriteColor(node->value<Color>());
    }
}
