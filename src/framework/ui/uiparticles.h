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

#ifndef UIPARTICLES_H
#define UIPARTICLES_H

#include <framework/graphics/particleeffect.h>
#include <framework/ui/uiwidget.h>

class UIParticles : public UIWidget
{
public:
    UIParticles();

    void drawSelf(Fw::DrawPane drawPane);

    void addEffect(const std::string& name);

    void onStyleApply(const std::string& styleName, const OTMLNodePtr& styleNode);

    void setReferencePos(const PointF& point) { m_referencePos = point; }
    PointF getReferencePos() { return m_referencePos; }

private:
    std::vector<ParticleEffectPtr> m_effects;
    PointF m_referencePos;
};

#endif
