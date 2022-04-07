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

#include "uiparticles.h"
#include <framework/graphics/drawpool.h>
#include <framework/graphics/particlemanager.h>

#include "framework/graphics/particleeffect.h"

UIParticles::UIParticles()
{
    m_referencePos = PointF(-1, -1);
}

void UIParticles::drawSelf(Fw::DrawPane /*drawPane*/)
{
    UIWidget::drawSelf(Fw::ForegroundPane);

    g_drawPool.setClipRect(getPaddingRect());
    g_painter->pushTransformMatrix();

    if (m_referencePos.x < 0 && m_referencePos.y < 0)
        g_painter->translate(m_rect.center());
    else
        g_painter->translate(m_rect.x() + m_referencePos.x * m_rect.width(), m_rect.y() + m_referencePos.y * m_rect.height());

    for (const auto& effect : m_effects)
        effect->render();

    g_painter->popTransformMatrix();
    g_drawPool.resetClipRect();
}

void UIParticles::onStyleApply(const std::string& styleName, const OTMLNodePtr& styleNode)
{
    UIWidget::onStyleApply(styleName, styleNode);

    for (const OTMLNodePtr& node : styleNode->children()) {
        if (node->tag() == "effect")
            addEffect(node->value());
        else if (node->tag() == "reference-pos")
            setReferencePos(node->value<PointF>());
    }
}

void UIParticles::addEffect(const std::string& name)
{
    const ParticleEffectPtr effect = g_particles.createEffect(name);
    if (effect)
        m_effects.push_back(effect);
}
