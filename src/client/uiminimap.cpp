/*
 * Copyright (c) 2010-2013 OTClient <https://github.com/edubart/otclient>
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

#include "uiminimap.h"
#include "minimap.h"
#include "game.h"

#include <framework/graphics/painter.h>

UIMinimap::UIMinimap()
{
    m_crossEnabled = true;
    m_zoom = 0;
    m_scale = 1.0f;
    m_minZoom = -5;
    m_maxZoom = 5;
}

void UIMinimap::drawSelf(Fw::DrawPane drawPane)
{
    UIWidget::drawSelf(drawPane);

    if((drawPane & Fw::ForegroundPane) == 0)
        return;

    g_minimap.draw(getPaddingRect(), getCameraPosition(), m_scale);

    // draw a cross in the center
    Rect vRect(0, 0, 2, 10);
    Rect hRect(0, 0, 10, 2);
    vRect.moveCenter(m_rect.center());
    hRect.moveCenter(m_rect.center());
    g_painter->setColor(Color::white);
    g_painter->drawFilledRect(vRect);
    g_painter->drawFilledRect(hRect);
}

bool UIMinimap::setZoom(int zoom)
{
    if(zoom < m_minZoom || zoom > m_maxZoom)
        return false;
    m_zoom = zoom;
    if(m_zoom < 0)
        m_scale = 1.0f / (1 << std::abs(zoom));
    else if(m_zoom > 0)
        m_scale = 1.0f * (1 << std::abs(zoom));
    else
        m_scale = 1;
    return true;
}

void UIMinimap::followCreature(const CreaturePtr& creature)
{
    m_followingCreature = creature;
    m_cameraPosition = Position();
}

void UIMinimap::setCameraPosition(const Position& pos)
{
    m_followingCreature = nullptr;
    m_cameraPosition = pos;
}

Position UIMinimap::getPosition(const Point& mousePos)
{
    return g_minimap.getPosition(mousePos, getPaddingRect(), getCameraPosition(), m_scale);
}

Position UIMinimap::getCameraPosition()
{
    if(m_followingCreature)
        return m_followingCreature->getPosition();
    else
        return m_cameraPosition;
}

void UIMinimap::onStyleApply(const std::string& styleName, const OTMLNodePtr& styleNode)
{
    UIWidget::onStyleApply(styleName, styleNode);
    for(const OTMLNodePtr& node : styleNode->children()) {
        if(node->tag() == "zoom")
            setZoom(node->value<int>());
        else if(node->tag() == "max-zoom")
            setMaxZoom(node->value<int>());
        else if(node->tag() == "min-zoom")
            setMinZoom(node->value<int>());
        else if(node->tag() == "cross")
            setCross(node->value<bool>());
    }
}
