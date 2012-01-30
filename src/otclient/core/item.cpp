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
#include <framework/graphics/graphics.h>
#include <framework/graphics/paintershaderprogram.h>
#include <framework/graphics/paintershadersources.h>

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

PainterShaderProgramPtr itemProgram;

void Item::draw(const Point& dest, float scaleFactor)
{
    if(getAnimationPhases() > 1)
        m_animation = (g_clock.ticks() % (Otc::ITEM_TICKS_PER_FRAME * getAnimationPhases())) / Otc::ITEM_TICKS_PER_FRAME;

    if(!itemProgram) {
        itemProgram = PainterShaderProgramPtr(new PainterShaderProgram);
        itemProgram->addShaderFromSourceCode(Shader::Vertex, glslMainWithTexCoordsVertexShader + glslPositionOnlyVertexShader);
        itemProgram->addShaderFromSourceFile(Shader::Fragment, "/game_shaders/item.frag");
        assert(itemProgram->link());
    }
    g_painter.setCustomProgram(itemProgram);

    for(int layer = 0; layer < getLayers(); layer++)
        internalDraw(dest, scaleFactor, layer);
    g_painter.releaseCustomProgram();
}

void Item::setPosition(const Position& position)
{
    if(isGround()) {
        m_xPattern = position.x % getNumPatternsX();
        m_yPattern = position.y % getNumPatternsY();
        m_zPattern = position.z % getNumPatternsZ();
    }

    Thing::setPosition(position);
}

void Item::setData(int data)
{
    if(isStackable() && getNumPatternsX() == 4 && getNumPatternsY() == 2) {
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
    else if(isHangable()) {
        if(isHookSouth()) {
            m_xPattern = getNumPatternsX() >= 2 ? 1 : 0;
        }
        else if(isHookEast()) {
            m_xPattern = getNumPatternsX() >= 3 ? 2 : 0;
        }
    }
    else if(isFluid() || isFluidContainer()) {
        int color = Otc::FluidTransparent;
        switch(data) {
        case Otc::FluidNone:
            color = Otc::FluidTransparent;
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
        case Otc::FluidOil:
            color = Otc::FluidBrown;
            break;
        case Otc::FluidBlood:
            color = Otc::FluidRed;
            break;
        case Otc::FluidSlime:
            color = Otc::FluidGreen;
            break;
        case Otc::FluidMud:
            color = Otc::FluidBrown;
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
        case Otc::FluidUrine:
            color = Otc::FluidYellow;
            break;
        case Otc::FluidRum:
            color = Otc::FluidBrown;
            break;
        case Otc::FluidFruidJuice:
            color = Otc::FluidYellow;
            break;
        case Otc::FluidCoconutMilk:
            color = Otc::FluidWhite;
            break;
        case Otc::FluidTea:
            color = Otc::FluidBrown;
            break;
        case Otc::FluidMead:
            color = Otc::FluidBrown;
            break;
        default:
            color = Otc::FluidTransparent;
            break;
        }

        m_xPattern = (color % 4) % getNumPatternsX();
        m_yPattern = (color / 4) % getNumPatternsY();
    }

    m_data = data;
}
