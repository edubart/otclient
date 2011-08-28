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

#include "item.h"
#include "datmanager.h"
#include "spritemanager.h"
#include "thing.h"
#include <framework/platform/platform.h>

Item::Item() : Thing(Otc::Item)
{
    m_count = 0;
    m_lastTicks = g_platform.getTicks();
}

void Item::setCount(int count)
{
    int oldCount = m_count;
    m_count = count;
    onCountChange(oldCount);
}

void Item::onPositionChange(const Position&)
{
    const ThingAttributes& attributes = g_dat.getItemAttributes(m_id);

    if(!attributes.moveable) {
        m_xDiv = m_position.x % attributes.xdiv;
        m_yDiv = m_position.y % attributes.ydiv;
        m_zDiv = m_position.z % attributes.zdiv;
    }
}

void Item::onCountChange(int)
{
    const ThingAttributes& attributes = g_dat.getItemAttributes(m_id);

    if(attributes.stackable) {
        if(m_count < 5) {
            m_xDiv = m_count-1;
            m_yDiv = 0;
        }
        else if(m_count < 10) {
            m_xDiv = 0;
            m_yDiv = 1;
        }
        else if(m_count < 25) {
            m_xDiv = 1;
            m_yDiv = 1;
        }
        else if(m_count < 50) {
            m_xDiv = 2;
            m_yDiv = 1;
        }
        else if(m_count <= 100) {
            m_xDiv = 3;
            m_yDiv = 1;
        }
    }
}

void Item::draw(int x, int y)
{
    const ThingAttributes& attributes = g_dat.getItemAttributes(m_id);

    if(attributes.animcount > 1) {
        if(g_platform.getTicks() - m_lastTicks > 500) {
            if(m_animation+1 == attributes.animcount)
                m_animation = 0;
            else
                m_animation++;

            m_lastTicks = g_platform.getTicks();
        }
    }

    /*if(attributes.group == Otc::ThingSplashGroup || attributes.group == Otc::ThingFluidGroup) {
        //xdiv = m_count % attributes.xdiv;
        //ydiv = m_count / attributes.ydiv;
    }*/

    for(int b = 0; b < attributes.blendframes; b++)
        internalDraw(x, y, b);
}

const ThingAttributes& Item::getAttributes()
{
    return g_dat.getItemAttributes(m_id);
}
