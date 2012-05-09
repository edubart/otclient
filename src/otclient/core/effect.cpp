/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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

#include "effect.h"
#include "map.h"
#include <framework/core/eventdispatcher.h>

void Effect::draw(const Point& dest, float scaleFactor, bool animate)
{
    if(m_id == 0)
        return;

    int animationPhase = 0;
    if(animate)
        animationPhase = std::min((int)(m_animationTimer.ticksElapsed() / Otc::EFFECT_TICKS_PER_FRAME), getAnimationPhases() - 1);
    m_type->draw(dest, scaleFactor, 0, 0, 0, animationPhase);
}

void Effect::startAnimation()
{
    m_animationTimer.restart();

    // schedule removal
    auto self = asEffect();
    g_eventDispatcher.scheduleEvent([self]() { g_map.removeThing(self); }, Otc::EFFECT_TICKS_PER_FRAME * getAnimationPhases());
}

void Effect::setId(uint32 id)
{
    m_id = id;
    m_type = g_thingsType.getThingType(m_id, ThingsType::Effect);
}
