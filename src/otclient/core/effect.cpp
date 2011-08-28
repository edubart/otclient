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
#include "datmanager.h"
#include "map.h"
#include <framework/platform/platform.h>
#include <framework/core/eventdispatcher.h>

Effect::Effect() : Thing(Otc::Effect)
{
    m_lastTicks = g_platform.getTicks();
    m_animation = 0;
    m_finished = false;
}

void Effect::draw(int x, int y)
{
    if(!m_finished) {
        if(g_platform.getTicks() - m_lastTicks > 75) {
            const ThingAttributes& attributes = getAttributes();
            if(m_animation+1 == attributes.animcount) {
                g_dispatcher.addEvent(std::bind(&Map::removeThingByPtr, &g_map, asThing()));
                m_finished = true;
            }
            else
                m_animation++;
            m_lastTicks = g_platform.getTicks();
        }

        internalDraw(x, y, 0, 0, 0, 0, m_animation);
    }
}

const ThingAttributes& Effect::getAttributes()
{
    return g_dat.getEffectAttributes(m_id);
}
