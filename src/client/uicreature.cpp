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

#include "uicreature.h"
#include <framework/otml/otml.h>
#include <framework/graphics/graphics.h>

void UICreature::drawSelf(Fw::DrawPane drawPane)
{
    if((drawPane & Fw::ForegroundPane) == 0)
        return;

    UIWidget::drawSelf(drawPane);

    if(m_creature) {
        Rect drawRect = getPaddingRect();
        g_painter->setColor(m_imageColor);
        m_creature->drawOutfit(drawRect, !m_fixedCreatureSize);
    }
}

void UICreature::setOutfit(const Outfit& outfit)
{
    if(!m_creature)
        m_creature = CreaturePtr(new Creature);
    m_creature->setDirection(Otc::South);
    m_creature->setOutfit(outfit);
}

void UICreature::onStyleApply(const std::string& styleName, const OTMLNodePtr& styleNode)
{
    UIWidget::onStyleApply(styleName, styleNode);

    for(const OTMLNodePtr& node : styleNode->children()) {
        if(node->tag() == "fixed-creature-size")
            setFixedCreatureSize(node->value<bool>());
        else if(node->tag() == "outfit-id") {
            Outfit outfit = (m_creature ? m_creature->getOutfit() : Outfit());
            outfit.setId(node->value<int>());
            setOutfit(outfit);
        }
        else if(node->tag() == "outfit-head") {
            Outfit outfit = (m_creature ? m_creature->getOutfit() : Outfit());
            outfit.setHead(node->value<int>());
            setOutfit(outfit);
        }
        else if(node->tag() == "outfit-body") {
            Outfit outfit = (m_creature ? m_creature->getOutfit() : Outfit());
            outfit.setBody(node->value<int>());
            setOutfit(outfit);
        }
        else if(node->tag() == "outfit-legs") {
            Outfit outfit = (m_creature ? m_creature->getOutfit() : Outfit());
            outfit.setLegs(node->value<int>());
            setOutfit(outfit);
        }
        else if(node->tag() == "outfit-feet") {
            Outfit outfit = (m_creature ? m_creature->getOutfit() : Outfit());
            outfit.setFeet(node->value<int>());
            setOutfit(outfit);
        }
    }
}
