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

#include "effect.h"
#include "thingstype.h"
#include "map.h"
#include "tile.h"
#include <framework/core/clock.h>
#include <framework/core/eventdispatcher.h>

Effect::Effect() : Thing()
{
    m_animationStartTicks = 0;
}

void Effect::draw(const Point& p)
{
    int animationPhase = (g_clock.ticks() - m_animationStartTicks) / TICKS_PER_FRAME;

    if(animationPhase < getAnimationPhases()) {
        m_animation = animationPhase;
        internalDraw(p, 0);
    }
}

void Effect::startAnimation()
{
    m_animationStartTicks = g_clock.ticks();

    // schedule removal
    auto self = asEffect();
    g_dispatcher.scheduleEvent([self]() {
        g_map.getTile(self->getPosition())->removeEffect(self);
    }, TICKS_PER_FRAME * getAnimationPhases());
}

ThingType *Effect::getType()
{
    return g_thingsType.getThingType(m_id, ThingsType::Effect);
}
