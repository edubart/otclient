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

#include "item.h"
#include "thingstype.h"
#include "spritemanager.h"
#include "thing.h"
#include <framework/core/clock.h>
#include <framework/core/eventdispatcher.h>

Item::Item() : Thing()
{
    m_data = 1;
}

ItemPtr Item::create(int id)
{
    ItemPtr item = ItemPtr(new Item);
    item->setId(id);
    return item;
}

void Item::draw(const Point& p, const Rect&)
{
    if(m_type->dimensions[ThingType::AnimationPhases] > 1)
        m_animation = (g_clock.ticks() % (TICKS_PER_FRAME * m_type->dimensions[ThingType::AnimationPhases])) / TICKS_PER_FRAME;

    for(int l = 0; l < m_type->dimensions[ThingType::Layers]; l++)
        internalDraw(p, l);
}

void Item::setPos(const Position& position)
{
    if(m_type->properties[ThingType::IsGround]) {
        m_xPattern = position.x % m_type->dimensions[ThingType::PatternX];
        m_yPattern = position.y % m_type->dimensions[ThingType::PatternY];
        m_zPattern = position.z % m_type->dimensions[ThingType::PatternZ];
    }

    Thing::setPos(position);
}

void Item::setData(int data)
{
    if(m_type->properties[ThingType::IsStackable] && m_type->dimensions[ThingType::PatternX] == 4 && m_type->dimensions[ThingType::PatternY] == 2) {
        if(data < 5) {
            m_xPattern = data-1;
            m_yPattern = 0;
        }
        else if(data < 10) {
            m_xPattern = 0;
            m_yPattern = 1;
        }
        else if(data < 25) {
            m_xPattern = 1;
            m_yPattern = 1;
        }
        else if(data < 50) {
            m_xPattern = 2;
            m_yPattern = 1;
        }
        else if(data <= 100) {
            m_xPattern = 3;
            m_yPattern = 1;
        }
    }
    else if(m_type->properties[ThingType::IsHangable]) {
        if(m_type->properties[ThingType::HookSouth]) {
            m_xPattern = m_type->dimensions[ThingType::PatternX] >= 2 ? 1 : 0;
        }
        else if(m_type->properties[ThingType::HookEast]) {
            m_xPattern = m_type->dimensions[ThingType::PatternX] >= 3 ? 2 : 0;
        }
    }
    else if(m_type->properties[ThingType::IsFluid] || m_type->properties[ThingType::IsFluidContainer]) {
        int color = 0;
        // TODO: find out what the heck does it mean 4, 7, 12, 13, 14, 16, 17. options are already there
        switch(data) {
        case 0:
            color = 0;
            break;
        case Otc::FluidWater:
            color = Otc::FluidBlue;
            break;
        case Otc::FluidMana:
            color = Otc::FluidPurple;
            break;
        case Otc::FluidBeer:
            color = Otc::FluidBrown;
            break;
        case 4:
            color = Otc::FluidBrown; // oil, mud, mead, rum, tea
            break;
        case Otc::FluidBlood:
            color = Otc::FluidRed;
            break;
        case Otc::FluidSlime:
            color = Otc::FluidGreen;
            break;
        case 7:
            color = Otc::FluidBrown; // oil, mud, mead, rum, tea
            break;
        case Otc::FluidLemonade:
            color = Otc::FluidYellow;
            break;
        case Otc::FluidMilk:
            color = Otc::FluidWhite;
            break;
        case Otc::FluidWine:
            color = Otc::FluidPurple;
            break;
        case Otc::FluidHealth:
            color = Otc::FluidRed;
            break;
        case 12:
            color = Otc::FluidYellow; // urine, fruit juice
            break;
        case 13:
            color = Otc::FluidBrown; // oil, mud, mead, rum, tea
            break;
        case 14:
            color = Otc::FluidYellow; // urine, fruit juice
            break;
        case Otc::FluidCoconutMilk:
            color = Otc::FluidWhite;
            break;
        case 16:
            color = Otc::FluidBrown; // oil, mud, mead, rum, tea
            break;
        case 17:
            color = Otc::FluidBrown; // oil, mud, mead, rum, tea
            break;
        default:
            color = 1;
            break;
        }

        m_xPattern = (color % 4) % m_type->dimensions[ThingType::PatternX];
        m_yPattern = (color / 4) % m_type->dimensions[ThingType::PatternY];
    }

    m_data = data;
}

ThingType *Item::getType()
{
    return g_thingsType.getThingType(m_id, ThingsType::Item);
}
