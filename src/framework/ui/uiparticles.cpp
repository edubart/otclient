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

#include "uiparticles.h"
#include <framework/graphics/particlemanager.h>

void UIParticles::drawSelf(Fw::DrawPane drawPane)
{
    if((drawPane & Fw::ForegroundPane) == 0)
        return;

    for(auto it = m_effects.begin(), end = m_effects.end(); it != end; ++it)
        (*it)->render();
}

void UIParticles::onStyleApply(const std::string& styleName, const OTMLNodePtr& styleNode)
{
    UIWidget::onStyleApply(styleName, styleNode);

    /*for(const OTMLNodePtr& node : styleNode->children()) {
        if(node->tag() == "reference")
            setItemId(node->value<int>());
        else if(node->tag() == "item-count")
            setItemCount(node->value<int>());
        else if(node->tag() == "virtual")
            setVirtual(node->value<bool>());
    }*/
}

void UIParticles::addEffect(const std::string& name)
{
    ParticleEffectPtr effect = g_particles.createEffect(name);
    if(effect)
        m_effects.push_back(effect);
}
