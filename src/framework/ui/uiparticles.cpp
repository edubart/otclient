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

#include "uiparticles.h"
#include <framework/graphics/particlemanager.h>

UIParticles::UIParticles()
{
    m_referencePos = PointF(-1,-1);
}

void UIParticles::drawSelf(Fw::DrawPane drawPane)
{
    if(drawPane & Fw::ForegroundPane) {
        if(drawPane != Fw::BothPanes) {
            glDisable(GL_BLEND);
            g_painter->setColor(Color::alpha);
            g_painter->drawFilledRect(m_rect);
            glEnable(GL_BLEND);
        }
    }

    if(drawPane & Fw::BackgroundPane) {
        UIWidget::drawSelf(Fw::ForegroundPane);
        g_painter->saveAndResetState();
        g_painter->setColor(Color::white);
        g_painter->setClipRect(getPaddingRect());

        if(m_referencePos.x < 0 && m_referencePos.y < 0)
            g_painter->translate(m_rect.center());
        else
            g_painter->translate(m_rect.x() + m_referencePos.x * m_rect.width(), m_rect.y() + m_referencePos.y * m_rect.height());

        for(auto it = m_effects.begin(), end = m_effects.end(); it != end; ++it)
            (*it)->render();
        g_painter->restoreSavedState();
    }
}

void UIParticles::onStyleApply(const std::string& styleName, const OTMLNodePtr& styleNode)
{
    UIWidget::onStyleApply(styleName, styleNode);

    for(const OTMLNodePtr& node : styleNode->children()) {
        if(node->tag() == "effect")
            addEffect(node->value());
        else if(node->tag() == "reference-pos")
            setReferencePos(node->value<PointF>());
    }
}

void UIParticles::addEffect(const std::string& name)
{
    ParticleEffectPtr effect = g_particles.createEffect(name);
    if(effect)
        m_effects.push_back(effect);
}
