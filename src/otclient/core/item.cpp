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
    m_id = 0;
    m_countOrSubType = 1;
}

ItemPtr Item::create(int id)
{
    if(id < g_thingsType.getFirstItemId() || id > g_thingsType.getMaxItemid()) {
        logTraceError("invalid item id ", id);
        return nullptr;
    }
    ItemPtr item = ItemPtr(new Item);
    item->setId(id);
    return item;
}

PainterShaderProgramPtr itemProgram;

void Item::draw(const Point& dest, float scaleFactor, bool animate)
{
    if(m_id == 0)
        return;

    // determine animation phase
    int animationPhase = 0;
    if(getAnimationPhases() > 1) {
        if(animate)
            animationPhase = (g_clock.ticks() % (Otc::ITEM_TICKS_PER_FRAME * getAnimationPhases())) / Otc::ITEM_TICKS_PER_FRAME;
        else
            animationPhase = getAnimationPhases()-1;
    }

    // determine x,y,z patterns
    int xPattern = 0, yPattern = 0, zPattern = 0;
    if(isGround()) {
        xPattern = m_position.x % getNumPatternsX();
        yPattern = m_position.y % getNumPatternsY();
        zPattern = m_position.z % getNumPatternsZ();
    } else if(isStackable() && getNumPatternsX() == 4 && getNumPatternsY() == 2) {
        if(m_countOrSubType < 5) {
            xPattern = m_countOrSubType-1;
            yPattern = 0;
        } else if(m_countOrSubType < 10) {
            xPattern = 0;
            yPattern = 1;
        } else if(m_countOrSubType < 25) {
            xPattern = 1;
            yPattern = 1;
        } else if(m_countOrSubType < 50) {
            xPattern = 2;
            yPattern = 1;
        } else if(m_countOrSubType <= 100) {
            xPattern = 3;
            yPattern = 1;
        }
    } else if(isHangable()) {
        if(isHookSouth())
            xPattern = getNumPatternsX() >= 2 ? 1 : 0;
        else if(isHookEast())
            xPattern = getNumPatternsX() >= 3 ? 2 : 0;
    } else if(isFluid() || isFluidContainer()) {
        int color = Otc::FluidTransparent;
        switch(m_countOrSubType) {
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

        xPattern = (color % 4) % getNumPatternsX();
        yPattern = (color / 4) % getNumPatternsY();
    }

    // setup item drawing shader
    if(!itemProgram) {
        itemProgram = PainterShaderProgramPtr(new PainterShaderProgram);
        itemProgram->addShaderFromSourceCode(Shader::Vertex, glslMainWithTexCoordsVertexShader + glslPositionOnlyVertexShader);
        itemProgram->addShaderFromSourceFile(Shader::Fragment, "/game_shaders/item.frag");
        assert(itemProgram->link());
    }
    g_painter.setCustomProgram(itemProgram);

    // draw all item layers
    for(int layer = 0; layer < getLayers(); layer++)
        internalDraw(dest, scaleFactor, xPattern, yPattern, zPattern, layer, animationPhase);

    // release draw shader
    g_painter.releaseCustomProgram();
}

void Item::setId(uint32 id)
{
    if(id < g_thingsType.getFirstItemId() || id > g_thingsType.getMaxItemid()) {
        logTraceError("invalid item id ", id);
        return;
    }
    m_id = id;
    m_type = g_thingsType.getThingType(m_id, ThingsType::Item);
}

int Item::getCount()
{
    if(isStackable())
        return m_countOrSubType;
    else
        return 1;
}

int Item::getSubType()
{
    if(isFluid() || isFluidContainer())
        return m_countOrSubType;
    else
        return 0;
}
