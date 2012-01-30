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
#include "thingstype.h"
#include "map.h"
#include "tile.h"
#include <framework/core/clock.h>
#include <framework/core/eventdispatcher.h>

void Effect::draw(const Point& dest, float scaleFactor)
{
    internalDraw(dest, scaleFactor, 0);
}

void Effect::startAnimation()
{
    m_animationTimer.restart();

    auto self = asEffect();

    // schedule next animation update
    if(getAnimationPhases() > 1)
        g_dispatcher.scheduleEvent([self]() { self->updateAnimation(); }, Otc::EFFECT_TICKS_PER_FRAME);

    // schedule removal
    g_dispatcher.scheduleEvent([self]() { g_map.removeThing(self); }, Otc::EFFECT_TICKS_PER_FRAME * getAnimationPhases());
}

void Effect::updateAnimation()
{
    int animationPhase = m_animationTimer.ticksElapsed() / Otc::EFFECT_TICKS_PER_FRAME;

    if(animationPhase < getAnimationPhases())
        m_animation = animationPhase;

    if(animationPhase < getAnimationPhases() - 1) {
        //schedule next animation update
        auto self = asEffect();
        g_dispatcher.scheduleEvent([self]() { self->updateAnimation(); }, Otc::EFFECT_TICKS_PER_FRAME);
    }
}
