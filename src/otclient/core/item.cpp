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
#include "thingstype.h"
#include "spritemanager.h"
#include "thing.h"
#include <framework/platform/platform.h>

Item::Item() : Thing()
{
    m_data = 0;
    m_lastTicks = g_platform.getTicks();
}

void Item::draw(const Point& p)
{
    const ThingType& type = g_thingsType.getItemType(m_id);

    if(type.animationPhases > 1)
        m_animation = (g_platform.getTicks() % (TICKS_PER_FRAME * type.animationPhases)) / TICKS_PER_FRAME;

    for(int b = 0; b < type.layers; b++)
        internalDraw(p, b);
}

void Item::setData(int count)
{
    int oldData = m_data;
    m_data = count;
    onDataChange(oldData);
}

const ThingType& Item::getType()
{
    return g_thingsType.getItemType(m_id);
}

void Item::onPositionChange(const Position&)
{
    const ThingType& type = g_thingsType.getItemType(m_id);

    if(type.isNotMoveable) {
        m_xPattern = m_position.x % type.xPattern;
        m_yPattern = m_position.y % type.yPattern;
        m_zPattern = m_position.z % type.zPattern;
    }
}

void Item::onDataChange(int)
{
    const ThingType& type = g_thingsType.getItemType(m_id);

    if(type.isStackable && type.xPattern == 4 && type.yPattern == 2) {
        if(m_data < 5) {
            m_xPattern = m_data-1;
            m_yPattern = 0;
        }
        else if(m_data < 10) {
            m_xPattern = 0;
            m_yPattern = 1;
        }
        else if(m_data < 25) {
            m_xPattern = 1;
            m_yPattern = 1;
        }
        else if(m_data < 50) {
            m_xPattern = 2;
            m_yPattern = 1;
        }
        else if(m_data <= 100) {
            m_xPattern = 3;
            m_yPattern = 1;
        }
    }
    else if(type.isHangable) {
        if(type.isHookSouth) {
            m_xPattern = type.xPattern >= 2 ? 1 : 0;
        }
        else if(type.isHookEast) {
            m_xPattern = type.xPattern >= 3 ? 2 : 0;
        }
    }
    else if(type.isSplash || type.isFluidContainer) {
        int var = 0;
        // TODO: find out what the heck does it mean
        switch(m_data) {
        case 0:
            var = 0;
            break;
        case 1:
            var = 1;
            break;
        case 2:
            var = 7;
            break;
        case 3:
            var = 3;
            break;
        case 4:
            var = 3;
            break;
        case 5:
            var = 2;
            break;
        case 6:
            var = 4;
            break;
        case 7:
            var = 3;
            break;
        case 8:
            var = 5;
            break;
        case 9:
            var = 6;
            break;
        case 10:
            var = 7;
            break;
        case 11:
            var = 2;
            break;
        case 12:
            var = 5;
            break;
        case 13:
            var = 3;
            break;
        case 14:
            var = 5;
            break;
        case 15:
            var = 6;
            break;
        case 16:
            var = 3;
            break;
        case 17:
            var = 3;
            break;
        default:
            var = 1;
            break;
        }

        m_xPattern = (var & 3) % type.xPattern;
        m_yPattern = (var >> 2) % type.yPattern;
    }
}
