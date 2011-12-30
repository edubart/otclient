/*
 * Copyright (c) 2010-2011 OTClient <https://github.com/edubart/otclient>
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

#include "uimap.h"
#include <otclient/core/map.h>
#include <framework/otml/otml.h>
#include <framework/graphics/graphics.h>
#include <otclient/core/tile.h>
#include <otclient/core/missile.h>
#include <otclient/core/effect.h>
#include <otclient/core/animatedtext.h>

UIMap::UIMap()
{
    m_mapMargin = 0;
}

void UIMap::render()
{
    renderSelf();

    g_painter.setColor(Fw::black);
    g_painter.drawBoundingRect(m_mapRect.expanded(1));
    g_map.draw(m_mapRect);

    renderChildren();
}

void UIMap::onStyleApply(const OTMLNodePtr& styleNode)
{
    for(OTMLNodePtr node : styleNode->children()) {
        if(node->tag() == "map margin")
            m_mapMargin = node->value<int>();
    }

    UIWidget::onStyleApply(styleNode);
}

bool UIMap::onMousePress(const Point& mousePos, Fw::MouseButton button)
{
    if(m_mapRect.contains(mousePos)) {
        Point relativeStretchMousePos = mousePos - m_mapRect.topLeft();
        Size mapSize(g_map.getVibibleSize().width() * Map::NUM_TILE_PIXELS, g_map.getVibibleSize().height() * Map::NUM_TILE_PIXELS);

        PointF stretchFactor(m_mapRect.width() / (float)mapSize.width(), m_mapRect.height() / (float)mapSize.height());
        PointF relativeMousePos = PointF(relativeStretchMousePos.x, relativeStretchMousePos.y) / stretchFactor;

        PointF tilePosF = relativeMousePos / Map::NUM_TILE_PIXELS;
        Position tilePos = Position(1 + (int)tilePosF.x - g_map.getCentralOffset().x, 1 + (int)tilePosF.y - g_map.getCentralOffset().y, 0) + g_map.getCentralPosition();

        TilePtr tile = g_map.getTile(tilePos);
        if(tile)
            tile->useItem();

        // cool testing \/
        if(button == Fw::MouseLeftButton) {
            MissilePtr shot = MissilePtr(new Missile());
            shot->setId(1);
            shot->setPath(g_map.getCentralPosition(), tilePos);
            g_map.addThing(shot, g_map.getCentralPosition());

            AnimatedTextPtr animatedText = AnimatedTextPtr(new AnimatedText);
            animatedText->setPosition(g_map.getCentralPosition());
            animatedText->setColor(12);
            animatedText->setText("text");

            g_map.addThing(animatedText, g_map.getCentralPosition());
        }
        else if(button == Fw::MouseRightButton) {
            EffectPtr effect = EffectPtr(new Effect());
            effect->setId(6);
            g_map.addThing(effect, tilePos);

            AnimatedTextPtr animatedText = AnimatedTextPtr(new AnimatedText);
            animatedText->setPosition(g_map.getCentralPosition());
            animatedText->setColor(12);
            animatedText->setText("8");
            g_map.addThing(animatedText, g_map.getCentralPosition());
        }
    }

    return UIWidget::onMousePress(mousePos, button);
}

void UIMap::onGeometryUpdate(const Rect& oldRect, const Rect& newRect)
{
    Rect mapRect = newRect.expanded(-m_mapMargin-1);
    Size mapSize(g_map.getVibibleSize().width() * Map::NUM_TILE_PIXELS, g_map.getVibibleSize().height() * Map::NUM_TILE_PIXELS);
    mapSize.scale(mapRect.size(), Fw::KeepAspectRatio);

    m_mapRect.resize(mapSize);
    m_mapRect.moveCenter(newRect.center());
}
