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
#include "tile.h"
#include "shadermanager.h"

#include "mapview.h"

#include <framework/core/clock.h>
#include <framework/core/eventdispatcher.h>
#include <framework/graphics/graphics.h>
#include <framework/graphics/framebuffer.h>
#include <framework/core/filestream.h>

Item::Item() : Thing()
{
    m_id = 0;
    m_countOrSubType = 1;
    m_shaderProgram = g_shaders.getDefaultItemShader();
}

ItemPtr Item::create(int id)
{
    ItemPtr item = ItemPtr(new Item);
    if(id < g_thingsType.getFirstItemId() || id > g_thingsType.getMaxItemid())
        g_logger.traceError(stdext::format("invalid item id %d", id));
    else {
        item->setId(id);
    }
    return item;
}
void Item::drawLight(const Point& dest, float scaleFactor, bool animate, MapView* mapview)
{

    uint32_t lightSize = 50*getLightLevel();


    glBlendFunc(GL_ONE, GL_ONE);
    mapview->m_framebuffer->release();
    mapview->m_lightbuffer->bind();

    Color lightColor = getLightColor();

        g_painter->setColor(lightColor);
        g_painter->drawTexturedRect(Rect(dest - Point(lightSize/2,lightSize/2), Size(lightSize,lightSize)), mapview->m_lightTexture);
    mapview->m_lightbuffer->release();
    mapview->m_framebuffer->bind();
    g_painter->refreshState();
}
void Item::draw(const Point& dest, float scaleFactor, bool animate)
{
    if(m_id == 0)
        return;

    // determine animation phase
    int animationPhase = 0;
    if(getAnimationPhases() > 1) {
        if(animate)
            animationPhase = (g_clock.millis() % (Otc::ITEM_TICKS_PER_FRAME * getAnimationPhases())) / Otc::ITEM_TICKS_PER_FRAME;
        else
            animationPhase = getAnimationPhases()-1;
    }

    // determine x,y,z patterns
    int xPattern = 0, yPattern = 0, zPattern = 0;
    if(isStackable() && getNumPatternsX() == 4 && getNumPatternsY() == 2) {
        if(m_countOrSubType <= 0) {
            xPattern = 0;
            yPattern = 0;
        } else if(m_countOrSubType < 5) {
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
        } else {
            xPattern = 3;
            yPattern = 1;
        }
    } else if(isHangable()) {
        const TilePtr& tile = getTile();
        if(tile) {
            if(tile->mustHookSouth())
                xPattern = getNumPatternsX() >= 2 ? 1 : 0;
            else if(tile->mustHookEast())
                xPattern = getNumPatternsX() >= 3 ? 2 : 0;
        }
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
    } else if(isGround() || isOnBottom()) {
        xPattern = m_position.x % getNumPatternsX();
        yPattern = m_position.y % getNumPatternsY();
        zPattern = m_position.z % getNumPatternsZ();
    }

    bool useShader = g_painter->hasShaders() && m_shaderProgram;
    if(useShader) {
        m_shaderProgram->bind();
        m_shaderProgram->setUniformValue(ShaderManager::ITEM_ID_UNIFORM, (int)m_id);

        g_painter->setShaderProgram(m_shaderProgram);
    }

    m_type->draw(dest, scaleFactor, 0, xPattern, yPattern, zPattern, animationPhase);


    if(useShader)
        g_painter->resetShaderProgram();
}

void Item::setId(uint32 id)
{
    if(id < g_thingsType.getFirstItemId() || id > g_thingsType.getMaxItemid()) {
        g_logger.traceError(stdext::format("invalid item id %d", id));
        return;
    }
    m_id = id;
    m_type = g_thingsType.getThingType(m_id, ThingsType::Item);
}



bool Item::unserializeAttr(FileStreamPtr fin)
{
    uint8 attrType;
    while ((attrType = fin->getU8()) != 0)
        readAttr((AttrTypes_t)attrType, fin);

    return true;
}

void Item::readAttr(AttrTypes_t attrType, FileStreamPtr fin)
{
    switch (attrType) {
    case ATTR_COUNT:
        setSubType(fin->getU8());
        break;
    case ATTR_ACTION_ID:
        setActionId(fin->getU16());
        break;
    case ATTR_UNIQUE_ID:
        setUniqueId(fin->getU16());
        break;
    case ATTR_NAME:
        setName(fin->getString());
        break;
    case ATTR_ARTICLE:
        fin->getString();
    case ATTR_ATTACK: // \/ not needed.
    case ATTR_EXTRAATTACK:
    case ATTR_DEFENSE:
    case ATTR_EXTRADEFENSE:
    case ATTR_ARMOR:
    case ATTR_ATTACKSPEED:
    case ATTR_HPPITCHANCE:
    case ATTR_DURATION:
        fin->getU32();
        break;
    case ATTR_SCRIPTPROTECTED:
    case ATTR_DUALWIELD:
    case ATTR_DECAYING_STATE:
    case ATTR_HPPOUSEDOORID:
        fin->getU8();
        break;
    case ATTR_TEXT:
        setText(fin->getString());
        break;
    case ATTR_WRITTENDATE:
        fin->getU32();
        break;
    case ATTR_WRITTENBY:
        fin->getString();
        break;
    case ATTR_DESC:
        setDescription(fin->getString());
        break;
    case ATTR_RUNE_CHARGES:
        fin->getU8();
        break;
    case ATTR_TELE_DEST: // Teleport should read that.
    case ATTR_SLEEPERGUID: // Bed should read that.
    case ATTR_SLEEPSTART:
    case ATTR_CONTAINER_ITEMS:
    case ATTR_ATTRIBUTE_MAP:
    default:
        break;
    }
}
